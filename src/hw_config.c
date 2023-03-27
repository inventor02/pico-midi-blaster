#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "sd_card.h"
#include "hw_config.h"
#include "diskio.h"

#include "config.h"

static spi_t SPI = {
  .hw_inst = spi1,
  .miso_gpio = GPIO_SD_MISO,
  .mosi_gpio = GPIO_SD_MOSI,
  .sck_gpio = GPIO_SD_SCK,
  .baud_rate = BAUD_SD,
  .mutex = {}
};

static sd_card_t SD_CARD = {
  .pcName = "0:",
  .spi = &SPI,
  .ss_gpio = GPIO_SD_SS,
  .m_Status = STA_NOINIT,
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