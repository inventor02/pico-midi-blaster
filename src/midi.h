#ifndef _MIDI_H
#define _MIDI_H

#include <stdint.h>

#include "pico/stdlib.h"

#include "config.h"

#define STAT_CHAN       1
// if you wanted to distinguish between channel voice
// and channel mode, you could do it here as 2
#define STAT_SYS_EX     3
#define STAT_SYS_COM    4
#define STAT_SYS_RT     5
#define STAT_META       6 // only in SMF, "meta events"

typedef enum midi_event_type {
  OTHER = -1,
  NOTE_ON,
  NOTE_OFF,
  END_OF_TRACK,
} midi_event_type_t;

typedef struct midi_event {
  midi_event_type_t type;
  uint8_t channel;
  uint32_t delta_time;
  uint8_t data[3];
} midi_event_t;

typedef struct midi_chunk {
  uint32_t length;
  midi_event_t events[1024];
} midi_chunk_t;

typedef struct midi_file {
  uint16_t format;
  uint16_t ntrks;
  uint16_t division;
  midi_chunk_t chunks[1];
} midi_file_t;

int midi_chunk_parse(midi_chunk_t *chunk, uint8_t contents[], uint contents_length);

void midi_file_parse(midi_file_t *file, uint8_t contents[], uint contents_length);

#endif