#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "sd_card.h"
#include "hw_config.h"

const uint GPIO_SD_CD = 8;
const uint GPIO_SD_MISO = 12;
const uint GPIO_SD_MOSI = 11;
const uint GPIO_SD_SCK = 10;
const uint GPIO_SD_SS = 15;
const uint BAUD_SD = 12500 * 1000;

static const spi_t SPI = {
  .hw_inst = spi1,
  .miso_gpio = GPIO_SD_MISO,
  .mosi_gpio = GPIO_SD_MOSI,
  .sck_gpio = GPIO_SD_SCK,
  .baud_rate = BAUD_SD,
};

static const sd_card_t SD_CARD = {
  .pcName = "",
  .spi = &SPI,
  .ss_gpio = GPIO_SD_SS,
};

size_t spi_get_num() {
  return 1;
}

spi_t *spi_get_by_num(size_t num) {
  if (num == 0) return &SPI;

  panic("There is only one SPI.");
}

size_t sd_get_num() {
  return 1;
}

sd_card_t *sd_get_by_num(size_t num) {
  if (num == 0) return &SD_CARD;

  panic("There is only one SD card.");
}