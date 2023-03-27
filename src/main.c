#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/mutex.h"
#include "hw_config.h"
#include "sd_card.h"
#include "ff.h"
#include "f_util.h"

#include "config.h"

int main() {
  stdio_init_all();
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
  gpio_put(PICO_DEFAULT_LED_PIN, 1);

  printf("Hello!\n\n");

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

  char buf[100] = "";
  fr = f_read(&file, &buf, 100, NULL);

  printf("file contents: %s\n", buf);
  f_close(&file);

  f_unmount(sd->pcName);
  printf("Done\n");

  gpio_put(PICO_DEFAULT_LED_PIN, 0);
  for (;;) tight_loop_contents();
}