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

static uint pwm_slice;
static uint pwm_chan;

void play_note(float freq, float vol) {
  uint32_t wrap = PWM_TOP / freq;
  pwm_set_wrap(pwm_slice, wrap);

  uint16_t duty = (wrap + 1) * vol - 1;
  pwm_set_chan_level(pwm_slice, pwm_chan, duty);

  printf("wrap=%d, duty=%d\n", wrap, duty);
  pwm_set_enabled(pwm_slice, true);
}

void stop_note() {
  pwm_set_enabled(pwm_slice, false);
}

int main() {
  stdio_init_all();

  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
  gpio_put(PICO_DEFAULT_LED_PIN, 1);

  gpio_set_function(GPIO_PWM_BUZZER, GPIO_FUNC_PWM);
  pwm_slice = pwm_gpio_to_slice_num(GPIO_PWM_BUZZER);
  pwm_chan = pwm_gpio_to_channel(GPIO_PWM_BUZZER);

  pwm_set_clkdiv(pwm_slice, clock_get_hz(clk_sys) / PWM_TOP);

  sd_card_t *sd = sd_get_by_num(0);
  mutex_init(&sd->spi->mutex);
  FRESULT fr = f_mount(&sd->fatfs, sd->pcName, 1);

  if (FR_OK != fr) {
    panic("f_mount error! %s\n", FRESULT_str(fr));
  }

  FIL file;
  fr = f_open(&file, MIDI_FILE_NAME, FA_READ);

  if (FR_OK != fr && FR_EXIST != fr) {
    panic("f_open error! %s\n", FRESULT_str(fr));
  }

  char buf[256] = "";
  fr = f_read(&file, &buf, 256, NULL);
  f_close(&file);

  f_unmount(sd->pcName);
  printf("Done\n");

  midi_file_t midi;
  midi_file_parse(&midi, (uint8_t *)buf, 114);
  printf("start playing\n");

  uint16_t ms_per_qn = 429; // TODO get this from the MIDI

  midi_event_t *events = &(midi.chunks[0].events);
  uint8_t event_count = midi.chunks[0].length;

  for (int i = 0; i < event_count; i++) {
    busy_wait_ms((events[i].delta_time / midi.division) * ms_per_qn);
    
    if (events[i].type == NOTE_ON) {
      uint8_t note = events[i].data[0];
      printf("Playing note %d, %f\n", note, MIDI_FREQS[note]);
      play_note(MIDI_FREQS[note], 0.05);
    } else {
      stop_note();
    }
  }

  printf("done!");
  gpio_put(PICO_DEFAULT_LED_PIN, 0);
  for (;;) tight_loop_contents();
}