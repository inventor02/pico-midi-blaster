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

static bool play = false;

static uint pwm_slice;
static uint pwm_chan;

static midi_file_t midi;

static float vol = 0.2;

void play_pause_callback() {
  printf("play/pause: %d\n", !play);
  play = !play; 
}

void vol_up_down_callback(uint gpio) {
  printf("volume change: old %f, ", vol);

  if (gpio == GPIO_VOL_UP) {
    vol += 0.05F;
    if (vol > 1) vol = 1.0F;
  } else {
    vol -= 0.05F;
    if (vol < 0) vol = 0.0F;
  }

  printf("new %f\n", vol);  
}

void gpio_callback(uint gpio, uint32_t event_mask) {
  switch (gpio) {
    case GPIO_PLAY_PAUSE: play_pause_callback(); return;
    case GPIO_VOL_UP:
    case GPIO_VOL_DOWN: vol_up_down_callback(gpio); return;
    default: // do nowt
  }
}

void init() {
  stdio_init_all();

  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
  gpio_put(PICO_DEFAULT_LED_PIN, 1);

  gpio_set_function(GPIO_PWM_BUZZER, GPIO_FUNC_PWM);
  pwm_slice = pwm_gpio_to_slice_num(GPIO_PWM_BUZZER);
  pwm_chan = pwm_gpio_to_channel(GPIO_PWM_BUZZER);

  gpio_set_irq_enabled_with_callback(GPIO_VOL_DOWN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
  gpio_set_irq_enabled(GPIO_PLAY_PAUSE, GPIO_IRQ_EDGE_FALL, true);
  gpio_set_irq_enabled(GPIO_VOL_UP, GPIO_IRQ_EDGE_FALL, true);

  pwm_set_clkdiv(pwm_slice, clock_get_hz(clk_sys) / PWM_TOP);
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

  uint16_t ms_per_qn = 429; // can get this from the MIDI if you wanted to

  // get your MIDI events

  // play them back in sequence
}

int main() {
  init();
  load_midi();

  // play your song!
  // you have the midi object as a static variable in this translation unit
  // you also have various other configs in config.h

  // some pointers:
  // - you may wish to make some helper functions to handle translating a MIDI frequency into
  //   a duty cycle for the PWM
  // - you may wish to run your PWM clock at *less* than the system clock
  // - a lot of the PWM values are capped as uint16s! don't forget that! I did ;-)
  
  gpio_put(PICO_DEFAULT_LED_PIN, 0);
  for (;;) tight_loop_contents();
}