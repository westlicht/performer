#include "DAC.h"

#include "hal/Delay.h"

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/spi.h>

#define DAC_SPI SPI5

#define DAC_PORT GPIOF
#define DAC_SYNC GPIO8

#define SPI_PORT GPIOF
#define SPI_SCK GPIO7
#define SPI_MOSI GPIO9
#define SPI_GPIO (SPI_SCK | SPI_MOSI)

#define WRITE_INPUT_REGISTER            0
#define UPDATE_OUTPUT_REGISTER          1
#define WRITE_INPUT_REGISTER_UPDATE_ALL 2
#define WRITE_INPUT_REGISTER_UPDATE_N   3
#define POWER_DOWN_UP_DAC               4
#define LOAD_CLEAR_CODE_REGISTER        5
#define LOAD_LDAC_REGISTER              6
#define RESET_POWER_ON                  7
#define SETUP_INTERNAL_REF              8

void DAC::init() {
    rcc_periph_clock_enable(RCC_GPIOF);

    // init spi pins
    gpio_mode_setup(SPI_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, SPI_GPIO);
    gpio_set_af(SPI_PORT, GPIO_AF5, SPI_GPIO);

    // init spi
    rcc_periph_clock_enable(RCC_SPI5);
    spi_init_master(DAC_SPI,
                    SPI_CR1_BAUDRATE_FPCLK_DIV_4, // TODO should DIV_2 still work?
                    SPI_CR1_CPOL_CLK_TO_1_WHEN_IDLE,
                    SPI_CR1_CPHA_CLK_TRANSITION_1,
                    SPI_CR1_DFF_8BIT,
                    SPI_CR1_MSBFIRST);
    spi_set_unidirectional_mode(DAC_SPI);
    spi_disable_crc(DAC_SPI);

    // spi_enable_ss_output(DAC_SPI);
    spi_enable(DAC_SPI);

    // init gpio
    gpio_mode_setup(DAC_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, DAC_SYNC);
    gpio_set(DAC_PORT, DAC_SYNC);

    // initialize DAC8568
    reset();
    setClearCode(ClearIgnore);
    setInternalRef(true);
}

void DAC::write(int channel) {
    writeDAC(WRITE_INPUT_REGISTER_UPDATE_N, channel, _values[channel], 15);
}

void DAC::write() {
    for (int i = 0; i < 7; ++i) {
        write(i);
    }
}

void DAC::writeDAC(uint8_t command, uint8_t address, uint16_t data, uint8_t function) {
    uint8_t b1 = command;
    uint8_t b2 = address << 4 | data >> 12;
    uint8_t b3 = data >> 4;
    uint8_t b4 = 0xf0 & (data << 4) >> 8 | function;

    gpio_clear(DAC_PORT, DAC_SYNC);
    hal::Delay::delay_ns<13>(); // t5 in timing diagram

    spi_send(DAC_SPI, b1);
    spi_send(DAC_SPI, b2);
    spi_send(DAC_SPI, b3);
    spi_send(DAC_SPI, b4);

    while (!(SPI_SR(DAC_SPI) & SPI_SR_TXE));

    hal::Delay::delay_ns<10>(); // t8 in timing diagram
    gpio_set(DAC_PORT, DAC_SYNC);
    hal::Delay::delay_ns<80>(); // t4 in timing diagram
}

void DAC::reset() {
    writeDAC(RESET_POWER_ON, 0, 0, 0);
}

void DAC::setInternalRef(bool enabled) {
    writeDAC(SETUP_INTERNAL_REF, 0, 0, enabled ? 1 : 0);
}

void DAC::setClearCode(ClearCode code) {
    writeDAC(LOAD_CLEAR_CODE_REGISTER, 0, 0, code);
}
