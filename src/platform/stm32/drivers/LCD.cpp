#include "LCD.h"

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/dma.h>

#include <cmath>
#include <algorithm>

#define LCD_PORT GPIOC
#define LCD_CS GPIO8
#define LCD_RES GPIO9
#define LCD_DC GPIO11
#define LCD_GPIO (LCD_CS | LCD_RES | LCD_DC)

#define LCD_SPI SPI3
#define LCD_SPI_DR SPI3_DR

#define LCD_DMA DMA1
#define LCD_DMA_CHANNEL DMA_SxCR_CHSEL_0
#define LCD_DMA_STREAM DMA_STREAM5

#define SPI_PORT GPIOC
#define SPI_SCK GPIO10
#define SPI_MOSI GPIO12
#define SPI_GPIO (SPI_SCK | SPI_MOSI)

// #define USE_DMA

static void wait(int n) {
    for (volatile int i = 0; i < n; ++i) {
        asm("nop");
    }
}

void LCD::init() {
    rcc_periph_clock_enable(RCC_GPIOC);

    // init spi pins
    gpio_mode_setup(SPI_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, SPI_GPIO);
    gpio_set_af(SPI_PORT, GPIO_AF6, SPI_GPIO);

    // init spi
    rcc_periph_clock_enable(RCC_SPI3);
    spi_init_master(LCD_SPI, SPI_CR1_BAUDRATE_FPCLK_DIV_2,
                    SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
                    SPI_CR1_CPHA_CLK_TRANSITION_1,
                    SPI_CR1_DFF_8BIT,
                    SPI_CR1_MSBFIRST);
    spi_set_unidirectional_mode(LCD_SPI);
    spi_disable_crc(LCD_SPI);
    spi_enable_ss_output(LCD_SPI);
    spi_enable(LCD_SPI);

#ifdef USE_DMA
    // init dma
    rcc_periph_clock_enable(RCC_DMA1);
    nvic_enable_irq(NVIC_DMA1_STREAM5_IRQ);
    dma_stream_reset(LCD_DMA, LCD_DMA_STREAM);
#endif

    // init control pins
    gpio_mode_setup(LCD_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LCD_GPIO);
    gpio_clear(LCD_PORT, LCD_GPIO);

    // setup
    gpio_clear(LCD_PORT, LCD_CS);
    gpio_set(LCD_PORT, LCD_RES);
    wait(100);
    gpio_clear(LCD_PORT, LCD_RES);
    wait(100);
    gpio_set(LCD_PORT, LCD_RES);
    wait(100);

    // write init sequence
    sendCmd(0xFD); // Set Command Lock (MCU protection status)
    sendData(0x12); // = Reset

    sendCmd(0xB3); // Set Front Clock Divider / Oscillator Frequency
    sendData(0xD0); // = reset / 1100b

    sendCmd(0xCA); // Set MUX Ratio
    sendData(0x3F); // = 63d = 64MUX

    sendCmd(0xA2); // Set Display Offset
    sendData(0x00); // = RESET

    sendCmd(0xA1); // Set Display Start Line
    sendData(0x00); // = register 00h

    sendCmd(0xA0); // Set Re-map and Dual COM Line mode
    sendData(0x14); // = Reset except Enable Nibble Re-map, Scan from COM[N-1] to COM0, where N is the Multiplex ratio
    sendData(0x11); // = Reset except Enable Dual COM mode (MUX = 63)

    sendCmd(0xB5); // Set GPIO
    sendData(0x00); // = GPIO0, GPIO1 = HiZ, Input Disabled

    sendCmd(0xAB); // Function Selection
    sendData(0x01); // = reset = Enable internal VDD regulator

    sendCmd(0xB4); // Display Enhancement A
    sendData(0xA0); // = Enable external VSL
    sendData(0xB5); // = Normal (reset)

    sendCmd(0xC1); // Set Contrast Current
    // sendData(0x7F); // = reset
    sendData(0xff);

    sendCmd(0xC7); // Master Contrast Current Control
    sendData(0x0F); // = no change

    sendCmd(0xB9); // Select Default Linear Gray Scale table

    sendCmd(0xB1); // Set Phase Length
    sendData(0xE2); // = Phase 1 period (reset phase length) = 5 DCLKs, Phase 2 period (first pre-charge phase length) = 14 DCLKs

    sendCmd(0xD1); // Display Enhancement B
    sendData(0xA2); // = Normal (reset)
    sendData(0x20); // n/a

    sendCmd(0xBB); // Set Pre-charge voltage
    sendData(0x1F); // = 0.60 x VCC

    sendCmd(0xB6); // Set Second Precharge Period
    sendData(0x08); // = 8 dclks [reset]

    sendCmd(0xBE); // Set VCOMH
    sendData(0x07); // = 0.86 x VCC

    sendCmd(0xA6); // Set Display Mode = Normal Display

    sendCmd(0xA9); // Exit Partial Display

    sendCmd(0xAF); // Set Sleep mode OFF (Display ON)
}

void LCD::draw(uint8_t *frameBuffer) {
#ifdef USE_DMA
    // wait for transfer to complete
    // while (DMA_SCR(LCD_DMA, LCD_DMA_STREAM) & DMA_SxCR_EN);
    // dma_disable_stream(LCD_DMA, LCD_DMA_STREAM);
    // dma_disable_stream(LCD_DMA, LCD_DMA_STREAM);
    dma_stream_reset(LCD_DMA, LCD_DMA_STREAM);
    // dma_clear_interrupt_flags(LCD_DMA, LCD_DMA_STREAM, DMA_TCIF);
    // spi_disable_tx_dma(LCD_SPI);
#endif

    uint8_t *src = frameBuffer;
    uint8_t *dst = reinterpret_cast<uint8_t *>(_frameBuffer);
    for (int i = 0; i < Cols * Rows / 2; ++i) {
        uint8_t a = *src++;
        uint8_t b = *src++;
        *dst++ = std::min(b, uint8_t(15)) | (std::min(a, uint8_t(15)) << 4);
        // *dst++ = (b >> 4) | ((a >> 4) << 4);
    }


#ifdef USE_DMA
    setColAddr(0x1c,0x5b);
    setRowAddr(0x00,0x3f);
    setWrite();
    gpio_set(LCD_PORT, LCD_DC);

    dma_set_peripheral_address(LCD_DMA, LCD_DMA_STREAM, uint32_t(&LCD_SPI_DR));
    dma_set_memory_address(LCD_DMA, LCD_DMA_STREAM, uint32_t(_frameBuffer));
    dma_set_number_of_data(LCD_DMA, LCD_DMA_STREAM, sizeof(_frameBuffer));
    dma_channel_select(LCD_DMA, LCD_DMA_STREAM, LCD_DMA_CHANNEL);
    dma_set_priority(LCD_DMA, LCD_DMA_STREAM, DMA_SxCR_PL_HIGH);

    dma_set_transfer_mode(LCD_DMA, LCD_DMA_STREAM, DMA_SxCR_DIR_MEM_TO_PERIPHERAL);
    dma_enable_direct_mode(LCD_DMA, LCD_DMA_STREAM);
    dma_set_memory_size(LCD_DMA, LCD_DMA_STREAM, DMA_SxCR_MSIZE_8BIT);
    dma_set_peripheral_size(LCD_DMA, LCD_DMA_STREAM, DMA_SxCR_PSIZE_8BIT);

    dma_enable_memory_increment_mode(LCD_DMA, LCD_DMA_STREAM);
    dma_disable_peripheral_increment_mode(LCD_DMA, LCD_DMA_STREAM);
    dma_set_memory_burst(LCD_DMA, LCD_DMA_STREAM, DMA_SxCR_PBURST_SINGLE);

    dma_enable_stream(LCD_DMA, LCD_DMA_STREAM);
    spi_enable_tx_dma(LCD_SPI);

    while (DMA_SCR(LCD_DMA, LCD_DMA_STREAM) & DMA_SxCR_EN);

    dma_disable_stream(LCD_DMA, LCD_DMA_STREAM);
    spi_disable_tx_dma(LCD_SPI);

    gpio_clear(LCD_PORT, LCD_DC);

#else
    setColAddr(0x1c,0x5b);
    setRowAddr(0x00,0x3f);
    setWrite();

    src = reinterpret_cast<uint8_t *>(_frameBuffer);
    for (int y = 0; y < Rows; y++) {
        for (int x = 0; x < Cols/2; x++) {
            sendData(*src++);
        }
    }
#endif
}


void LCD::sendCmd(uint8_t cmd) {
    gpio_clear(LCD_PORT, LCD_DC);
    spi_xfer(LCD_SPI, cmd);
}

void LCD::sendData(uint8_t data) {
    gpio_set(LCD_PORT, LCD_DC);
    spi_xfer(LCD_SPI, data);
}

void LCD::setColAddr(uint8_t a, uint8_t b) {
    sendCmd(0x15);
    sendData(a);
    sendData(b);
}

void LCD::setRowAddr(uint8_t a, uint8_t b) {
    sendCmd(0x75);
    sendData(a);
    sendData(b);
}

void LCD::setWrite() {
    sendCmd(0x5C);
}
