#ifndef _CONFIG_H
#define _CONFIG_H

#include "pico/stdlib.h"

static const uint GPIO_SD_CD = 8;
static const uint GPIO_SD_MISO = 12;
static const uint GPIO_SD_MOSI = 11;
static const uint GPIO_SD_SCK = 10;
static const uint GPIO_SD_SS = 15;
static const uint BAUD_SD = 30 * 1000 * 1000;

static const char MIDI_FILE_NAME[] = "midi.mid";

#endif