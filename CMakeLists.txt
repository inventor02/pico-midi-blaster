cmake_minimum_required(VERSION 3.12)
set(CMAKE_C_STANDARD 11)

include(lib/pico-sdk/pico_sdk_init.cmake)

add_subdirectory(lib/fatfs_spi/FatFs_SPI)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

project(pico-midi-blaster)

pico_sdk_init()

add_executable(blaster)
target_link_libraries(blaster
  pico_stdlib
  pico_sync
  pico_time
  hardware_spi
  hardware_pwm
  FatFs_SPI
)
pico_add_extra_outputs(blaster)

target_sources(blaster PRIVATE src/main.c src/hw_config.c src/midi.c)