#ifndef _MIDI_H
#define _MIDI_H

#include <stdint.h>

#include "pico/stdlib.h"

#include "config.h"

typedef enum midi_event {
  NOTE_ON,
  NOTE_OFF
} midi_event_t;

typedef struct midi_chunk {
  uint32_t length;
  uint data[];
} midi_chunk_t;

typedef struct midi_file {
  uint16_t format;
  uint16_t ntrks;
  uint16_t division;
  midi_chunk_t chunks[];
} midi_file_t;

void midi_file_parse(midi_file_t *file, uint contents[]);

#endif