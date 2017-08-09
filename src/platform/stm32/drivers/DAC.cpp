#include "DAC.h"

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/spi.h>

#define DAC_PORT GPIOC
#define DAC_CS GPIO8
#define DAC_RES GPIO9
#define DAC_DC GPIO11
#define DAC_DEBUG GPIO13
#define DAC_GPIO (DAC_CS | DAC_RES | DAC_DC | DAC_DEBUG)

#define DAC_SPI SPI3

#define SPI_PORT GPIOC
#define SPI_SCK GPIO10
#define SPI_MOSI GPIO12
#define SPI_GPIO (SPI_SCK | SPI_MOSI)

void DAC::init() {

}

void DAC::write(int channel) {
    // const auto &value = _values[channel];
    // spi_send(DAC_SPI, 0x10 | (channel & 0xf));
    // spi_send(DAC_SPI, value >> 8);
    // spi_send(DAC_SPI, value);
}

void DAC::write() {
    for (int i = 0; i < 8; ++i) {
        write(i);
    }
}
