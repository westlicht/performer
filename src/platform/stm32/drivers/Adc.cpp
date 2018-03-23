#include "Adc.h"

#include "hal/Delay.h"

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/dma.h>

#include <cstdlib>

#define ADC_PORT GPIOA
#define ADC_GPIO (GPIO0 | GPIO1 | GPIO2 | GPIO3)

void Adc::init() {
    // init pins
    rcc_periph_clock_enable(RCC_GPIOA);
    gpio_mode_setup(ADC_PORT, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, ADC_GPIO);

    // init ADC
    rcc_periph_clock_enable(RCC_ADC1);

    adc_power_off(ADC1);

    adc_set_clk_prescale(ADC_CCR_ADCPRE_BY8);
    adc_set_multi_mode(ADC_CCR_MULTI_INDEPENDENT);
    adc_set_resolution(ADC1, ADC_CR1_RES_12BIT);
    adc_set_left_aligned(ADC1);

    uint8_t channels[] = { 0, 1, 2, 3 };
    static_assert(sizeof(channels) == Channels, "invalid channel count");
    adc_set_regular_sequence(ADC1, Channels, channels);
    adc_set_sample_time_on_all_channels(ADC1, ADC_SMPR_SMP_480CYC);

    adc_enable_scan_mode(ADC1);
    adc_set_continuous_conversion_mode(ADC1);

    // init DMA
    rcc_periph_clock_enable(RCC_DMA2);

    dma_stream_reset(DMA2, DMA_STREAM0);
    dma_set_peripheral_address(DMA2, DMA_STREAM0, reinterpret_cast<uint32_t>(&ADC_DR(ADC1)));
    dma_set_memory_address(DMA2, DMA_STREAM0, reinterpret_cast<uint32_t>(_channels));
    dma_enable_memory_increment_mode(DMA2, DMA_STREAM0);
    dma_set_peripheral_size(DMA2, DMA_STREAM0, DMA_SxCR_PSIZE_16BIT);
    dma_set_memory_size(DMA2, DMA_STREAM0, DMA_SxCR_MSIZE_16BIT);
    dma_set_priority(DMA2, DMA_STREAM0, DMA_SxCR_PL_LOW);
    dma_set_number_of_data(DMA2, DMA_STREAM0, Channels);
    dma_enable_circular_mode(DMA2, DMA_STREAM0);
    dma_set_transfer_mode(DMA2, DMA_STREAM0, DMA_SxCR_DIR_PERIPHERAL_TO_MEM);
    dma_channel_select(DMA2, DMA_STREAM0, DMA_SxCR_CHSEL_0);
    dma_enable_stream(DMA2, DMA_STREAM0);

    adc_enable_dma(ADC1);
    adc_set_dma_continue(ADC1);

    // start scanning
    hal::Delay::delay_us(3); // tSTAB
    adc_power_on(ADC1);
    adc_start_conversion_regular(ADC1);
}
