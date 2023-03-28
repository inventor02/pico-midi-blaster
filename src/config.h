#ifndef _CONFIG_H
#define _CONFIG_H

#include "pico/stdlib.h"

static const uint GPIO_SD_CD = 8;
static const uint GPIO_SD_MISO = 12;
static const uint GPIO_SD_MOSI = 11;
static const uint GPIO_SD_SCK = 10;
static const uint GPIO_SD_SS = 15;
static const uint BAUD_SD = 30 * 1000 * 1000;

static const uint GPIO_PWM_BUZZER = 18;
static const uint GPIO_PLAY_PAUSE = 21;
static const uint GPIO_VOL_UP = 22;
static const uint GPIO_VOL_DOWN = 20;

static const uint PWM_TOP = 1000000UL;

static const char MIDI_FILE_NAME[] = "midi.mid";

#endif