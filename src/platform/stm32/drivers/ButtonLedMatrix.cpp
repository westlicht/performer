#include "ButtonLedMatrix.h"

#include "core/profiler/Profiler.h"

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/spi.h>

#include <cstring>

#define SR_PORT GPIOE
#define SR_LATCH GPIO3
#define SR_LOAD GPIO4
#define SR_GPIO (SR_LATCH | SR_LOAD)

#define SR_SPI SPI4

#define SPI_PORT GPIOE
#define SPI_SCK GPIO2
#define SPI_MISO GPIO5
#define SPI_MOSI GPIO6
#define SPI_GPIO (SPI_SCK | SPI_MISO | SPI_MOSI)

#define ENC_PORT GPIOD
#define ENC_SWITCH GPIO13
#define ENC_A GPIO12
#define ENC_B GPIO11
#define ENC_GPIO (ENC_SWITCH | ENC_A | ENC_B)

PROFILER_INTERVAL(ButtonLedMatrixProcess, "ButtonLedMatrix.process")
PROFILER_INTERVAL(ButtonLedMatrixInterval, "ButtonLedMatrix.interval")

ButtonLedMatrix::ButtonLedMatrix() :
    _row(0)
{
}

void ButtonLedMatrix::init() {
    // enable clocks
    rcc_periph_clock_enable(RCC_GPIOE);
    rcc_periph_clock_enable(RCC_GPIOD);
    rcc_periph_clock_enable(RCC_SPI4);

    // init spi pins
    gpio_mode_setup(SPI_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, SPI_GPIO);
    gpio_set_af(SPI_PORT, GPIO_AF5, SPI_GPIO);

    // init spi
    spi_init_master(SR_SPI, SPI_CR1_BAUDRATE_FPCLK_DIV_8,
                    SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
                    SPI_CR1_CPHA_CLK_TRANSITION_1,
                    SPI_CR1_DFF_8BIT,
                    SPI_CR1_MSBFIRST);
    spi_enable(SR_SPI);

    // init control pins
    gpio_mode_setup(SR_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, SR_GPIO);
    gpio_clear(SR_PORT, SR_GPIO);

    // init encoder pins
    gpio_mode_setup(ENC_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, ENC_GPIO);

    // init state
    std::memset(_buttonState, 0, sizeof(_buttonState));
    std::memset(_ledState, 0, sizeof(_ledState));
}



#if 0
static void wait(int n) {
    for (volatile int i = 0; i < n; ++i) {
        asm("nop");
    }
}
#endif

void ButtonLedMatrix::process() {
    PROFILER_INTERVAL_END(ButtonLedMatrixInterval);
    PROFILER_INTERVAL_BEGIN(ButtonLedMatrixInterval);

    PROFILER_INTERVAL_BEGIN(ButtonLedMatrixProcess);

    processEncoder();

    uint8_t nextRow = (_row + 1) % Rows;

    uint8_t rowData = ~(1 << nextRow);
    uint8_t ledData = 0;
    for (int col = 0; col < ColsLed; ++col) {
        int index = col * Rows + nextRow;
        if (_ledState[index].red.update()) {
            ledData |= (1 << (col * 2));
        }
        if (_ledState[index].green.update()) {
            ledData |= (1 << (col * 2 + 1));
        }
    }

    // trigger load line
    gpio_clear(SR_PORT, SR_LOAD);
    // wait(10);
    gpio_set(SR_PORT, SR_LOAD);

    // write row & leds
    uint8_t buttonData = spi_xfer(SR_SPI, ledData);
    spi_xfer(SR_SPI, rowData);

    // wait(10000);

    // trigger latch line
    gpio_set(SR_PORT, SR_LATCH);
    // wait(10);
    gpio_clear(SR_PORT, SR_LATCH);

    for (int col = 0; col < ColsButton; ++col) {
        int buttonIndex = col * Rows + _row;
        auto &state = _buttonState[buttonIndex].state;
        bool newState = !(buttonData & (1 << col));
        if (newState != state) {
            state = newState;
            _events.write(Event(state ? KeyDown : KeyUp, buttonIndex));
        }
    }

    _row = nextRow;

    PROFILER_INTERVAL_END(ButtonLedMatrixProcess);
}

void ButtonLedMatrix::processEncoder() {
    auto updateEncoder = [this] (int pin, bool state) {
        if (state != _encoderState[pin]) {
            _encoderState[pin] = state;
            if (!_encoderState[0] && !_encoderState[1]) {
                _events.write(Event(Encoder, pin ? -1 : 1));
            }
        }
    };

    bool state = !gpio_get(ENC_PORT, ENC_SWITCH);
    if (state != _encoderSwitch) {
        _encoderSwitch = state;
        _events.write(Event(state ? KeyDown : KeyUp, Rows * ColsButton));
    }
    updateEncoder(0, !gpio_get(ENC_PORT, ENC_A));
    updateEncoder(1, !gpio_get(ENC_PORT, ENC_B));
}
