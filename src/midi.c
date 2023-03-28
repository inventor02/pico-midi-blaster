#include "midi.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include "pico/stdlib.h"

uint16_t bytes_to_i16(uint8_t bytes[]) {
  return (uint16_t)bytes[0] << 8 |
    (uint16_t)bytes[1];
}

uint32_t bytes_to_i32(uint8_t bytes[]) {
  return (uint32_t)bytes[0] << 24 |
    (uint32_t)bytes[1] << 16 |
    (uint32_t)bytes[2] << 8 |
    (uint32_t)bytes[3];
}

uint32_t varlen_bytes_to_i32(uint8_t bytes[]) {
  uint i = 0;
  bool more = true;
  uint32_t result = 0;

  while (more) {
    uint8_t byte = bytes[i];

    // if the most significant bit is a 1, we have more
    // bytes to read, if a 0, stop
    if ((byte >> 7) & 0b1) more = false;

    // clear most significant bit
    byte = byte & 0b01111111;

    // shift the existing result left by 7 bits and then
    // bitwise OR with our new byte (now the LSB)
    result = (result << (i * 7)) | byte;

    // we could get stuck here, so if we are reading far too
    // many bytes we will panic - i > 4 means we cannot fit
    // in to an int32 anyway!
    if (i > 4) panic("varlen_bytes_to_i32 runaway, i=%d", i);

    // scan the next byte
    i++;
  }

  return result;
}

uint8_t varlen_bytes_count(uint32_t varlen) {
  if (varlen <= 0x7F) return 1;
  if (varlen <= 0x3FFF) return 2;
  if (varlen <= 0x1FFFFF) return 3;
  return 4;
}

int midi_event_data_len(uint8_t status) {
  uint8_t msg = status & 0xF0;

  switch (msg) {
    case 0x80: return 2;
    case 0x90: return 2;
    case 0xA0: return 2;
    case 0xB0: return 2;
    case 0xC0: return 1;
    case 0xD0: return 1;
    case 0xE0: return 2;
  }
}

int midi_event_get_chan(uint8_t status) {
  return status & 0x0F;
}

int midi_event_stat_type(uint8_t status) {
  uint8_t o1 = status & 0xF0;
  uint8_t o2 = status & 0x0F;

  if (o1 == 0xF0) { // system
    if (o2 == 0x00) return STAT_SYS_EX;
    if ((o2 & 0x8) == 0x0) return STAT_SYS_COM;
    return STAT_SYS_RT;
  }

  /*
    There is a caveat here! 0xB0 can be either channel voice or channel mode,
    but as we do not care about these messages in our implementation we just
    assume it's channel voice - note that this is *wrong* though.
  */
  return STAT_CHAN_VOICE;
}

void midi_file_parse(midi_file_t *file, uint8_t contents[], uint contents_length) {
  puts("Parsing MIDI file.");

  // for the header we need at least 24 bytes
  assert(contents_length >= 24);

  // keep track of the offset
  uint o = 0;

  // allocate a buffer of 5 bytes for the header type
  char header_type[5];
  strncpy(header_type, contents, 4);
  o += 4;

  // assert the header type is that of a SMF file
  if(strncmp(header_type, "MThd", 4)) panic("incorrect header type %s", header_type);

  // get the length of the header
  uint32_t header_length = bytes_to_i32(contents + o);
  o += 4;

  // check the length of the header is 6
  // if it's not, we have a problem
  if (header_length != 6) panic("incorrect header length %d", header_length);

  // get the type of the file
  uint16_t file_type = bytes_to_i16(contents + o);
  o += 2;

  // if the type is not 0, we aren't worthy
  if (file_type != 0) panic("incorrect file format %d", file_type);

  file->format = 0;

  // get the number of tracks
  uint16_t tracks = bytes_to_i16(contents + o);
  o += 2;

  // if this is a format 0 file and there are != 1 tracks,
  // the file is malformed
  if (tracks != 1) panic("files must have one track! found %d", tracks);

  file->ntrks = tracks;

  // get the division
  uint16_t division = bytes_to_i16(contents + o);
  o += 2;

  file->division = division;

  printf("Parsed header:\nlength=%d\ntype %d\n%d tracks\ndivision is %d\n", header_length, file_type, tracks, division);
}

int midi_chunk_parse(midi_chunk_t *chunk, uint8_t contents[], uint contents_length) {
  puts("Parsing MIDI chunk");

  if (contents_length < 8) panic("not enough bytes for chunk header!");

  uint o = 0;

  char header_type[5];
  strncpy(header_type, contents, 4);
  o += 4;

  if (strncmp(header_type, "MTrk", 4)) {
    // this is not a track chunk, so we can ignore it
    return 1;
  }

  uint32_t raw_length = bytes_to_i32(contents + o);
  o += 4;

  bool more = true;
  uint8_t status = 0;
  uint8_t stat_type = 0;
  while (more) {
    uint32_t delta_time = varlen_bytes_to_i32(contents + o);
    o += varlen_bytes_count(delta_time);

    uint8_t stat = *(contents + o);
    uint8_t chan = 0;

    if (stat <= 0x7F) {
      // this is actually another data byte for the previous message
      // but we don't know what the message was, so we ignore it
      o++;
      continue;
    } else {
      status = stat;
      o++;

      stat_type = midi_event_stat_type(stat);
      
      // clear the buffer if a system exclusive or system common message
      // is received
      if (stat_type == STAT_SYS_EX || stat_type == STAT_SYS_COM) {
        status = 0;
        stat_type = 0;
      }
    }

    chan = midi_event_get_chan(status);
  }
}