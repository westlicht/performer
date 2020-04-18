#include "Dac.h"

#include "hal/Delay.h"

#include "core/Debug.h"

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/spi.h>

#define DAC_SPI SPI3

#define DAC_PORT GPIOB
#define DAC_SYNC GPIO0

#define WRITE_INPUT_REGISTER            0
#define UPDATE_OUTPUT_REGISTER          1
#define WRITE_INPUT_REGISTER_UPDATE_ALL 2
#define WRITE_INPUT_REGISTER_UPDATE_N   3
#define POWER_DOWN_UP_DAC               4
#define LOAD_CLEAR_CODE_REGISTER        5
#define LOAD_LDAC_REGISTER              6
#define RESET_POWER_ON                  7
#define SETUP_INTERNAL_REF              8

Dac::Dac(Type type)
{
    switch (type) {
    case Type::DAC8568C: _dataShift = 0; break;
    case Type::DAC8568A: _dataShift = 1; break;
    }
}

void Dac::init() {

    // init spi pins
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_GPIOC);
    gpio_mode_setup(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO10);   // SCK
    gpio_set_af(GPIOC, GPIO_AF6, GPIO10);
    gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO5);    // MOSI
    gpio_set_af(GPIOB, GPIO_AF6, GPIO5);

    // init spi
    rcc_periph_clock_enable(RCC_SPI3);
    spi_init_master(DAC_SPI,
                    SPI_CR1_BAUDRATE_FPCLK_DIV_2,
                    SPI_CR1_CPOL_CLK_TO_1_WHEN_IDLE,
                    SPI_CR1_CPHA_CLK_TRANSITION_1,
                    SPI_CR1_DFF_8BIT,
                    SPI_CR1_MSBFIRST);
    spi_set_unidirectional_mode(DAC_SPI);
    spi_disable_crc(DAC_SPI);

    spi_enable_ss_output(DAC_SPI);
    spi_enable(DAC_SPI);

    // init gpio
    rcc_periph_clock_enable(RCC_GPIOB);
    gpio_mode_setup(DAC_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, DAC_SYNC);
    gpio_set(DAC_PORT, DAC_SYNC);
    hal::Delay::delay_ns<80>(); // t4 in timing diagram

    // initialize DAC8568
    reset();
    setClearCode(ClearIgnore);
    setInternalRef(true);
    writeDac(POWER_DOWN_UP_DAC, 0, 0, 0xff);
}

void Dac::write(int channel) {
    writeDac(WRITE_INPUT_REGISTER_UPDATE_N, channel, _values[channel], 15);
}

void Dac::write() {
    for (int channel = 0; channel < Channels; ++channel) {
        writeDac(channel == 7 ? WRITE_INPUT_REGISTER_UPDATE_ALL : WRITE_INPUT_REGISTER, channel, _values[channel], 0);
    }
}

void Dac::writeDac(uint8_t command, uint8_t address, uint16_t data, uint8_t function) {
    // Shift data by one bit for DAC8568A
    data <<= _dataShift;

    uint8_t b1 = command;
    uint8_t b2 = (address << 4) | (data >> 12);
    uint8_t b3 = data >> 4;
    uint8_t b4 = (data & 0xf) << 4 | function;

    gpio_clear(DAC_PORT, DAC_SYNC);

    hal::Delay::delay_ns<13>(); // t5 in timing diagram

    spi_send(DAC_SPI, b1);
    spi_send(DAC_SPI, b2);
    spi_send(DAC_SPI, b3);
    spi_send(DAC_SPI, b4);

    while (!(SPI_SR(DAC_SPI) & SPI_SR_TXE));

    // hal::Delay::delay_ns<10>(); // t8 in timing diagram
    hal::Delay::delay_ns<200>(); // TODO not sure why we need 200ns instead of the 10ns in the datasheet

    gpio_set(DAC_PORT, DAC_SYNC);
    hal::Delay::delay_ns<80>(); // t4 in timing diagram
}

void Dac::reset() {
    writeDac(RESET_POWER_ON, 0, 0, 0);
    hal::Delay::delay_us(50);
}

void Dac::setInternalRef(bool enabled) {
    writeDac(SETUP_INTERNAL_REF, 0, 0, enabled ? 1 : 0);
}

void Dac::setClearCode(ClearCode code) {
    writeDac(LOAD_CLEAR_CODE_REGISTER, 0, 0, code);
}
