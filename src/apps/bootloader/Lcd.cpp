#include "Lcd.h"

#include "Delay.h"

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/spi.h>

#include <cmath>
#include <algorithm>

#define LCD_PORT GPIOB
#define LCD_CS GPIO12
#define LCD_RES GPIO14
#define LCD_DC GPIO15
#define LCD_GPIO (LCD_CS | LCD_RES | LCD_DC)

#define LCD_SPI SPI2
#define LCD_SPI_DR SPI2_DR

struct InitCommand {
    uint8_t cmd;
    uint8_t len;
    uint8_t data0;
    uint8_t data1;
};

static InitCommand initCommands[] = {
    { 0xfd, 1, 0x12 }, // Set Command Lock (MCU protection status) | 0x12 = reset
    { 0xb3, 1, 0xd0 }, // Set Front Clock Divider / Oscillator Frequency
    { 0xca, 1, 0x3f }, // Set MUX Ratio | 0x3f = 64mux
    { 0xa2, 1, 0x00 }, // Set Display Offset | 0x00 = reset
    { 0xa1, 1, 0x00 }, // Set Display Start Line, 0x00 = register
    // Set Re-map and Dual COM Line mode
    // 0x14 = Reset except Enable Nibble Re-map, Scan from COM[N-1] to COM0, where N is the Multiplex ratio
    // 0x11 = Reset except Enable Dual COM mode (MUX = 63)
    { 0xa0, 2, 0x14, 0x11 },
    { 0xb5, 1, 0x00 }, // Set GPIO
    { 0xab, 1, 0x01 }, // Function Selection | 0x01 = reset = Enable internal VDD regulator
    // Display Enhancement A
    // 0xa0 = Enable external VSL
    // 0xb5 = Normal (reset)
    { 0xb4, 2, 0xa0, 0xb5 },
    { 0xc1, 1, 0xff }, // Set Contrast Current
    { 0xc7, 1, 0x0f }, // Master Contrast Current Control | 0x0f = no change
    { 0xb9, 0 }, // Select Default Linear Gray Scale table
    { 0xb1, 1, 0xe2 }, // Set Phase Length | 0xe2 = Phase 1 period (reset phase length) = 5 DCLKs, Phase 2 period (first pre-charge phase length) = 14 DCLKs
    { 0xd1, 2, 0xa2, 0x20 }, // Display Enhancement B | 0xa2 = Normal (reset) | 0x20 = n/a
    { 0xbb, 1, 0x1f }, // Set Pre-charge voltage | 0x1f = 0.6 * VCC
    { 0xb6, 1, 0x08 }, // Set Second Precharge Period | 0x08 = 8 dclks
    { 0xbe, 1, 0x07 }, // Set VCOMH | 0x07 = = 0.86 x VCC
    { 0xa6, 0 }, // Set Display Mode = Normal Display
    { 0xa9, 0 }, // Exit Partial Display
    { 0xaf, 0 }, // Set Sleep mode OFF (Display ON),
    { 0x00 }
};


static inline void waitTxDone() {
    while (!(SPI_SR(LCD_SPI) & SPI_SR_TXE));
    while ((SPI_SR(LCD_SPI) & SPI_SR_BSY));
}


void Lcd::init() {
    // init spi pins
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_GPIOC);
    gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO13);   // SCK
    gpio_set_af(GPIOB, GPIO_AF5, GPIO13);
    gpio_mode_setup(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO3);    // MOSI
    gpio_set_af(GPIOC, GPIO_AF5, GPIO3);

    // init spi
    rcc_periph_clock_enable(RCC_SPI2);
    rcc_periph_reset_pulse(RST_SPI2);
    spi_init_master(LCD_SPI,
                    SPI_CR1_BAUDRATE_FPCLK_DIV_4,       // max LCD clock is 10MHz
                    SPI_CR1_CPOL_CLK_TO_1_WHEN_IDLE,
                    SPI_CR1_CPHA_CLK_TRANSITION_2,
                    SPI_CR1_DFF_8BIT,
                    SPI_CR1_MSBFIRST);
    // spi_set_bidirectional_transmit_only_mode(LCD_SPI);

    spi_enable(LCD_SPI);

    // init control pins
    gpio_mode_setup(LCD_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LCD_GPIO);
    gpio_clear(LCD_PORT, LCD_GPIO);

    // setup
    gpio_clear(LCD_PORT, LCD_CS);
    gpio_set(LCD_PORT, LCD_RES);
    Delay::delay_us(1000);

    gpio_clear(LCD_PORT, LCD_RES);
    Delay::delay_us(100);
    gpio_set(LCD_PORT, LCD_RES);
    Delay::delay_us(100);

    // write commands for initialization
    initialize();
}

void Lcd::deinit() {
    rcc_periph_clock_disable(RCC_GPIOB);
    rcc_periph_clock_disable(RCC_SPI2);
    rcc_periph_reset_pulse(RST_GPIOB);
    rcc_periph_reset_pulse(RST_SPI2);
}

void Lcd::draw(uint8_t *frameBuffer) {
    setColAddr(0x1c,0x5b);
    setRowAddr(0x00,0x3f);
    setWrite();

    uint8_t *src = frameBuffer;
    for (int y = 0; y < Height; y++) {
        for (int x = 0; x < Width/2; x++) {
            uint8_t a = *src++;
            uint8_t b = *src++;
            uint8_t c = std::min(b, uint8_t(15)) | (std::min(a, uint8_t(15)) << 4);
            sendData(c);
        }
    }
}

void Lcd::sendCmd(uint8_t cmd) {
    waitTxDone();
    gpio_clear(LCD_PORT, LCD_DC);
    spi_send(LCD_SPI, cmd);
}

void Lcd::sendData(uint8_t data) {
    waitTxDone();
    gpio_set(LCD_PORT, LCD_DC);
    spi_send(LCD_SPI, data);
}

void Lcd::initialize() {
    InitCommand *cmd = initCommands;
    while (cmd->cmd) {
        sendCmd(cmd->cmd);
        if (cmd->len > 0) {
            sendData(cmd->data0);
        }
        if (cmd->len > 1) {
            sendData(cmd->data1);
        }
        ++cmd;
    }
}

void Lcd::setColAddr(uint8_t a, uint8_t b) {
    sendCmd(0x15);
    sendData(a);
    sendData(b);
}

void Lcd::setRowAddr(uint8_t a, uint8_t b) {
    sendCmd(0x75);
    sendData(a);
    sendData(b);
}

void Lcd::setWrite() {
    sendCmd(0x5C);
}
