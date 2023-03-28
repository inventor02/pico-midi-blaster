#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/mutex.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"
#include "hw_config.h"
#include "sd_card.h"
#include "ff.h"
#include "f_util.h"

#include "config.h"
#include "midi.h"
#include "music.h"

// up here you may want to keep track of some stuff like the PWM channel
// and slice, or whether you're currently playing, etc.

static midi_file_t midi;

void init() {
  stdio_init_all();

  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
  gpio_put(PICO_DEFAULT_LED_PIN, 1);

  // you'll need to initialise the PWM system here

  // for the buttons, you might also want some interrupts on the GPIO pins
}

void load_midi() {
  sd_card_t *sd = sd_get_by_num(0);
  mutex_init(&sd->spi->mutex);
  FRESULT fr = f_mount(&sd->fatfs, sd->pcName, 1);

  if (FR_OK != fr) {
    panic("f_mount error! %s\n", FRESULT_str(fr));
  }

  FIL file;
  fr = f_open(&file, MIDI_FILE_NAME, FA_READ);

  uint16_t size = f_size(&file);

  if (FR_OK != fr && FR_EXIST != fr) {
    panic("f_open error! %s\n", FRESULT_str(fr));
  }

  char buf[1024] = "";
  fr = f_read(&file, &buf, size, NULL);
  f_close(&file);

  f_unmount(sd->pcName);

  midi_file_parse(&midi, (uint8_t *)buf, size);
  printf("midi loaded\n");
}

void play_song() {
  printf("start playing\n");

  // milliseconds per quarter note - with this and the division, as well as the delta time, you can
  // determine the delay between each event
  // (you can get this from the MIDI itself if you wanted to)
  uint16_t ms_per_qn = 429;

  // get your MIDI events

  // play them back in sequence
}

int main() {
  init();
  load_midi();

  // play your song!
  // you have the midi object as a static variable in this translation unit
  // you also have various other configs in config.h
  play_song(); // I pulled this out into a function, but feel free to remove it

  // some pointers:
  // - you may wish to make some helper functions to handle translating a MIDI frequency into
  //   a duty cycle for the PWM
  // - you may wish to run your PWM clock at *less* than the system clock
  // - a lot of the PWM values are capped as uint16s! don't forget that! I did ;-)
  
  gpio_put(PICO_DEFAULT_LED_PIN, 0);
  for (;;) tight_loop_contents();
}