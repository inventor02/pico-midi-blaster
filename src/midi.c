#include "midi.h"

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