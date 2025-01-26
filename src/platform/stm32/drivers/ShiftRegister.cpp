#include "ShiftRegister.h"

#include "hal/Delay.h"

#include "core/profiler/Profiler.h"
#include "core/Debug.h"

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/spi.h>

#include <cstring>

#define SR_PORT GPIOC
#define SR_LATCH GPIO5
#define SR_LOAD GPIO4
#define SR_GPIO (SR_LATCH | SR_LOAD)

#define SR_OE_PORT GPIOA
#define SR_OE GPIO4

#define SR_SPI SPI1

#define SPI_PORT GPIOA
#define SPI_SCK GPIO5
#define SPI_MISO GPIO6
#define SPI_MOSI GPIO7
#define SPI_GPIO (SPI_SCK | SPI_MISO | SPI_MOSI)

ShiftRegister::ShiftRegister() {
    _outputs.fill(0u);
    _inputs.fill(0u);
}

void ShiftRegister::init() {
    // init spi pins
    rcc_periph_clock_enable(RCC_GPIOA);
    gpio_mode_setup(SPI_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, SPI_GPIO);
    gpio_set_af(SPI_PORT, GPIO_AF5, SPI_GPIO);
    gpio_set_output_options(SPI_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, SPI_SCK | SPI_MOSI);

    // SPI1 is running on APB2 with 84MHz, maximum clock for 74HCxxx shift registers is 20MHz

    // init spi
    rcc_periph_clock_enable(RCC_SPI1);
    rcc_periph_reset_pulse(RST_SPI1);
    spi_init_master(SR_SPI, SPI_CR1_BAUDRATE_FPCLK_DIV_8, // 84MHz / 8 = 10.5MHz < 20MHz
                    SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
                    SPI_CR1_CPHA_CLK_TRANSITION_1,
                    SPI_CR1_DFF_8BIT,
                    SPI_CR1_MSBFIRST);
    spi_enable(SR_SPI);

    // init control pins
    rcc_periph_clock_enable(RCC_GPIOC);
    gpio_mode_setup(SR_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, SR_GPIO);
    gpio_set_output_options(SR_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, SR_GPIO);

    gpio_clear(SR_PORT, SR_LATCH);
    gpio_set(SR_PORT, SR_LOAD);

    // output enable
    gpio_mode_setup(SR_OE_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, SR_OE);
    gpio_set_output_options(SR_OE_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, SR_OE);
    gpio_set(SR_OE_PORT, SR_OE);
}

void ShiftRegister::process() {
    // trigger load line
    gpio_clear(SR_PORT, SR_LOAD);
    gpio_set(SR_PORT, SR_LOAD);

    // transfer data
    for (int sr = 0; sr < NumRegisters; ++sr) {
        _inputs[sr] = spi_xfer(SR_SPI, _outputs[NumRegisters - sr - 1]);
    }

    // trigger latch line
    gpio_set(SR_PORT, SR_LATCH);
    gpio_clear(SR_PORT, SR_LATCH);
}
