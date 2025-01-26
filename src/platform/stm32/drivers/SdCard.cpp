#include "SdCard.h"

#include "SystemConfig.h"

#include "os/os.h"

#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/sdio.h>

void SdCard::init() {
    rcc_periph_clock_enable(RCC_SDIO);
    rcc_periph_clock_enable(RCC_DMA2);
    powerOff();

    // Pin mappings
    // PC8 - SDIO_D0
    // PC12 - SDIO_CK
    // PD2 - SDIO_CMD
    // PC11 - SDIO_CD

    rcc_periph_clock_enable(RCC_GPIOC);
    rcc_periph_clock_enable(RCC_GPIOD);
    gpio_mode_setup(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO8 | GPIO12);
    gpio_mode_setup(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2);
    gpio_set_output_options(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO8 | GPIO12);
    gpio_set_output_options(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO2);
    gpio_set_af(GPIOC, GPIO_AF12, GPIO8 | GPIO12);
    gpio_set_af(GPIOD, GPIO_AF12, GPIO2);

    // card detect pin
    gpio_mode_setup(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO11);
}

bool SdCard::available() {
#if CONFIG_SDCARD_USE_CARD_DETECT
    if (_initialized) {
        _initialized = cardDetect();
    }
#endif

    if (_initialized) {
        _initialized = waitDataReady();
    }

    if (!_initialized) {
        _initialized = initCard();
    }

    return _initialized;
}

bool SdCard::writeProtected() {
    return false;
}

bool SdCard::read(uint8_t *buf, uint32_t sector, uint8_t count) {
    // DBG("read(sector=%d,count=%d)", sector, count);
    uint8_t *data = buf;
    for (uint32_t i = 0; i < count; ++i) {
        if (!readBlock(sector + i, data)) {
            return false;
        }
        data += 512;
    }
    return true;
}

bool SdCard::write(const uint8_t *buf, uint32_t sector, uint8_t count) {
    // DBG("write(sector=%d,count=%d)", sector, count);
    const uint8_t *data = buf;
    for (uint32_t i = 0; i < count; ++i) {
        if (!writeBlock(sector + i, data)) {
            return false;
        }
        data += 512;
    }
    return true;
}

bool SdCard::cardDetect() const {
    return gpio_get(GPIOC, GPIO11);
}

void SdCard::powerOn() {
    SDIO_POWER = SDIO_POWER_PWRCTRL_PWRON;
    while (SDIO_POWER != SDIO_POWER_PWRCTRL_PWRON);
    SDIO_CLKCR = SDIO_CLKCR_CLKEN | SDIO_CLKCR_WIDBUS_1 | 118;
}

void SdCard::powerOff() {
    SDIO_POWER = SDIO_POWER_PWRCTRL_PWROFF;
    rcc_periph_reset_pulse(RST_SDIO);
}

void SdCard::sendCommand(uint32_t cmd, uint32_t arg) {
    cmd &= SDIO_CMD_CMDINDEX_MASK;
    uint32_t waitresp = SDIO_CMD_WAITRESP_SHORT;
    if (cmd == 0) {
        waitresp = SDIO_CMD_WAITRESP_NO_0;
    } else if (cmd == 2 || cmd == 9 || cmd == 10) {
        waitresp = SDIO_CMD_WAITRESP_LONG;
    }

    /* If a data transaction is in progress, wait for it to finish */
#if 0
    if ((cmd != 12) && (SDIO_STA & (SDIO_STA_RXACT | SDIO_STA_TXACT))) {
        // XXX: This should be an error, we don't have multithread
        tmp_val |= SDIO_CMD_WAITPEND;
    }
#endif

    SDIO_ICR = 0x7ff; // Reset all signals we use (and some we don't).
    SDIO_ARG = arg; // The arg must be set before the command.
    // Set the command and associated bits.
    SDIO_CMD = (cmd | SDIO_CMD_CPSMEN | waitresp);
}

SdCard::Error SdCard::commandResult() {
    uint32_t status = SDIO_STA & 0xfff;

    if (status & SDIO_STA_CMDACT) {
        return InProgress;
    }

    if (status & SDIO_STA_CMDREND) {
        SDIO_ICR = SDIO_STA_CMDREND;
        return Success;
    }

    if (status & SDIO_STA_CMDSENT) {
        SDIO_ICR = SDIO_STA_CMDSENT;
        return Success;
    }

    if (status & SDIO_STA_CTIMEOUT) {
        SDIO_ICR = SDIO_STA_CTIMEOUT;
        return Timeout;
    }

    if (status & SDIO_STA_CCRCFAIL) {
        SDIO_ICR = SDIO_STA_CCRCFAIL;
        return CRCFail;
    }

    return Unknown;
}

SdCard::Error SdCard::sendCommandWait(uint32_t cmd, uint32_t arg) {
    Error result;
    sendCommand(cmd, arg);
    while ((result = commandResult()) == InProgress) {
        // allow other tasks to run
        os::this_task::yield();
    }
    return result;
}

SdCard::Error SdCard::sendCommandRetry(uint32_t cmd, uint32_t arg, int maxRetries) {
    Error result = Unknown;
    for (int i = 0; i < maxRetries; ++i) {
        result = sendCommandWait(cmd, arg);
        if (result == Success) {
            break;
        }
    }
    return result;
}

// SdCard::Error SdCard::select() {
//     return sendCommandRetry(7, sd_card_info.rca << 16);
// }

SdCard::Error SdCard::sendAppCommand(uint32_t cmd, uint32_t arg, int maxRetries) {
    Error result = Unknown;
    Error expected = (cmd == 41) ? CRCFail : Success;
    for (int i = 0; i < maxRetries; ++i) {
        result = sendCommandWait(55, _cardInfo.rca << 16);
        if (result != Success) {
            continue;
        }
        result = sendCommandWait(cmd, arg);
        if (result == expected) {
            break;
        }
    }
    return result;
}

bool SdCard::initCard() {
    powerOn();

#if CONFIG_SDCARD_USE_CARD_DETECT
    if (!cardDetect()) {
        powerOff();
        return false;
    }
#endif

    _cardInfo = CardInfo();

    if (sendCommandRetry(0, 0) != Success) {
        return false;
    }

    Error result;

    bool hcs = false;
    result = sendCommandRetry(8, 0x1f1);
    if (result == Success && SDIO_RESP1 == 0x1f1) {
        hcs = true;
    } else if (result == Timeout) {
        hcs = false;
    } else {
        powerOff();
        return false;
    }

    const uint32_t OCR_BUSY = 0x80000000;
    const uint32_t OCR_HCS = 0x40000000;
    const uint32_t OCR_CCS = 0x40000000;

    bool acmd41_success = false;
    uint32_t timeout = os::ticks() + os::time::ms(2000);
    while (os::ticks() < timeout) {
        result = sendAppCommand(41, 0x100000 | (hcs ? OCR_HCS : 0));
        uint32_t response = SDIO_RESP1;
        if (result == CRCFail && (response & OCR_BUSY) != 0) {
            _cardInfo.ccs = (response & OCR_CCS) != 0;
            acmd41_success = true;
            break;
        }
    }
    if (!acmd41_success) {
        powerOff();
        return false;
    }

    if (sendCommandRetry(2, 0) != Success) {
        powerOff();
        return false;
    }

    bool cmd3_success = false;
    // for (int i = 0; i < max_retries; ++i) {
        if (sendCommandRetry(3, 0) == Success) {
            uint32_t response = SDIO_RESP1;
            _cardInfo.rca = response >> 16;
            if (_cardInfo.rca != 0) {
                cmd3_success = true;
                // break;
            }
        }
    // }
    if (!cmd3_success) {
        powerOff();
        return false;
    }

    if (sendCommandRetry(9, _cardInfo.rca << 16) != Success) {
        powerOff();
        return false;
    }

    // Get the size of the card from the CSD. There are two versions.
    // For V1: byte_size = BLOCK_LEN * MULT * (C_SIZE + 1) bytes.
    // For V2: byte_size = (C_SIZE + 1) * 512K bytes.
    // We address the card in 512 byte blocks so we set size = byte_size / 512.
    uint32_t csd_version = SDIO_RESP1 >> 30;
    if (csd_version == 0) {
        // Until I find an old card, this is untested.
        uint32_t read_bl_len = (SDIO_RESP2 >> 16) & 0xF;
        uint32_t c_size = ((SDIO_RESP2 & 0x3FF) << 2) | (SDIO_RESP3 >> 30);
        uint32_t c_size_mult = (SDIO_RESP3 >> 15) & 0x7;
        uint32_t mult = 1 << (c_size_mult + 2);
        uint32_t blocknr = (c_size + 1) * mult;
        uint32_t block_len = 1 << read_bl_len;
        _cardInfo.size = (block_len * blocknr) >> 9;
    } else if (csd_version == 1) {
        uint32_t c_size = ((SDIO_RESP2 & 0x3F) << 16) | (SDIO_RESP3 >> 16);
        // (c_size + 1) * 512K / 512 = (c_size + 1) * 1024 = (c_size + 1) << 10.
        _cardInfo.size = (c_size + 1) << 10;
    } else {
        powerOff();
        return false;
    }

    if (sendCommandRetry(7, _cardInfo.rca << 16) != Success) {
        powerOff();
        return false;
    }

    DBG("card size = %lu", _cardInfo.size);

    return true;
}


bool SdCard::waitDataReady() {
    uint32_t timeout = os::ticks() + os::time::ms(1000);
    while (os::ticks() < timeout) {
        if (sendCommandWait(13, _cardInfo.rca << 16) == Success && (SDIO_RESP1 & 0x100) != 0) {
            return true;
        }
    }
    return false;
}

bool SdCard::readBlock(uint32_t address, void *buffer) {
    ASSERT(buffer >= (void *)0x20000000, "buffer not in SRAM");
    // DBG("readBlock(address=%lu, buffer=%p)", address, buffer);
    if (!waitDataReady()) {
        return false;
    }

    if (!_cardInfo.ccs) {
        address *= 512;
        if (sendCommandRetry(16, 512) != Success) {
            return false;
        }
    }

    SDIO_DCTRL = 0;

#if 1
    dma_stream_reset(DMA2, DMA_STREAM3);
    dma_channel_select(DMA2, DMA_STREAM3, DMA_SxCR_CHSEL_4);
    dma_set_memory_size(DMA2, DMA_STREAM3, DMA_SxCR_MSIZE_32BIT);
    dma_set_peripheral_size(DMA2, DMA_STREAM3, DMA_SxCR_PSIZE_32BIT);
    dma_enable_memory_increment_mode(DMA2, DMA_STREAM3);
    dma_disable_peripheral_increment_mode(DMA2, DMA_STREAM3);
    dma_set_transfer_mode(DMA2, DMA_STREAM3, DMA_SxCR_DIR_PERIPHERAL_TO_MEM);
    dma_set_peripheral_address(DMA2, DMA_STREAM3, (uint32_t)&SDIO_FIFO);
    dma_set_memory_address(DMA2, DMA_STREAM3, (uint32_t)buffer);
    dma_set_number_of_data(DMA2, DMA_STREAM3, 0);
    dma_set_priority(DMA2, DMA_STREAM3, DMA_SxCR_PL_VERY_HIGH);
    // dma_set_peripheral_flow_control(DMA2, DMA_STREAM3);

    dma_set_memory_burst(DMA2, DMA_STREAM3, DMA_SxCR_MBURST_INCR4);
    dma_set_peripheral_burst(DMA2, DMA_STREAM3, DMA_SxCR_PBURST_INCR4);
    dma_disable_double_buffer_mode(DMA2, DMA_STREAM3);
    // dma_enable_circular_mode(DMA2, DMA_STREAM3);

    dma_enable_fifo_mode(DMA2, DMA_STREAM3);
    dma_set_fifo_threshold(DMA2, DMA_STREAM3, DMA_SxFCR_FTH_4_4_FULL);
    dma_set_peripheral_flow_control(DMA2, DMA_STREAM3);

    dma_enable_stream(DMA2, DMA_STREAM3);
#endif

    // A 100ms timeout expressed as ticks in the 24Mhz bus clock.
    SDIO_DTIMER = 2400000;

    // These two registers must be set before SDIO_DCTRL.
    SDIO_DLEN = 512;
    SDIO_DCTRL = SDIO_DCTRL_DBLOCKSIZE_9 | SDIO_DCTRL_DMAEN |
                 SDIO_DCTRL_DTDIR | SDIO_DCTRL_DTEN;

    if (sendCommandWait(17, address) != Success) {
        return false;
    }

    const uint32_t DATA_RX_ERROR_FLAGS = (SDIO_STA_STBITERR |
                                          SDIO_STA_RXOVERR |
                                          SDIO_STA_DTIMEOUT |
                                          SDIO_STA_DCRCFAIL);
    const uint32_t DATA_RX_SUCCESS_FLAGS = (SDIO_STA_DBCKEND |
                                            SDIO_STA_DATAEND);

    while (!dma_get_interrupt_flag(DMA2, DMA_STREAM3, DMA_TCIF)) {
        // allow other tasks to run
        os::this_task::yield();
    }

    while (true) {
        volatile uint32_t result = SDIO_STA;
        // DBG("STA = 0x%x", result);
        // DBG("FIFOCNT = %d", SDIO_FIFOCNT);
        if (result & (DATA_RX_SUCCESS_FLAGS | DATA_RX_ERROR_FLAGS)) {
            if (result & DATA_RX_ERROR_FLAGS) {
                return false;
            } else if (result & DATA_RX_SUCCESS_FLAGS) {
                break;
            }
        }

        // allow other tasks to run
        os::this_task::yield();
    }

    return true;
}

bool SdCard::writeBlock(uint32_t address, const void *buffer) {
    ASSERT(buffer >= (void *)0x20000000, "buffer not in SRAM");
    // DBG("writeBlock(address=%lu, buffer=%p)", address, buffer);
    if (!waitDataReady()) {
        return false;
    }

    if (!_cardInfo.ccs) {
        address *= 512;
        if (sendCommandRetry(16, 512) != Success) {
            return false;
        }
    }

    if (sendCommandWait(24, address) != Success) {
        return false;
    }

    SDIO_DCTRL = 0;

#if 1
    dma_stream_reset(DMA2, DMA_STREAM3);
    dma_channel_select(DMA2, DMA_STREAM3, DMA_SxCR_CHSEL_4);
    dma_set_memory_size(DMA2, DMA_STREAM3, DMA_SxCR_MSIZE_32BIT);
    dma_set_peripheral_size(DMA2, DMA_STREAM3, DMA_SxCR_PSIZE_32BIT);
    dma_enable_memory_increment_mode(DMA2, DMA_STREAM3);
    dma_disable_peripheral_increment_mode(DMA2, DMA_STREAM3);
    dma_set_transfer_mode(DMA2, DMA_STREAM3, DMA_SxCR_DIR_MEM_TO_PERIPHERAL);
    dma_set_peripheral_address(DMA2, DMA_STREAM3, (uint32_t)&SDIO_FIFO);
    dma_set_memory_address(DMA2, DMA_STREAM3, (uint32_t)buffer);
    dma_set_number_of_data(DMA2, DMA_STREAM3, 0);
    dma_set_priority(DMA2, DMA_STREAM3, DMA_SxCR_PL_VERY_HIGH);
    // dma_set_peripheral_flow_control(DMA2, DMA_STREAM3);

    dma_set_memory_burst(DMA2, DMA_STREAM3, DMA_SxCR_MBURST_INCR4);
    dma_set_peripheral_burst(DMA2, DMA_STREAM3, DMA_SxCR_PBURST_INCR4);
    dma_disable_double_buffer_mode(DMA2, DMA_STREAM3);
    // dma_enable_circular_mode(DMA2, DMA_STREAM3);

    dma_enable_fifo_mode(DMA2, DMA_STREAM3);
    dma_set_fifo_threshold(DMA2, DMA_STREAM3, DMA_SxFCR_FTH_4_4_FULL);
    dma_set_peripheral_flow_control(DMA2, DMA_STREAM3);

    dma_enable_stream(DMA2, DMA_STREAM3);
#endif

    // A 500ms timeout expressed as ticks in the 24Mhz bus clock.
    SDIO_DTIMER = 12000000;
    // These two registers must be set before SDIO_DCTRL.
    SDIO_DLEN = 512;
    SDIO_DCTRL = SDIO_DCTRL_DBLOCKSIZE_9 | SDIO_DCTRL_DMAEN | SDIO_DCTRL_DTEN;

    const uint32_t DATA_TX_ERROR_FLAGS = (SDIO_STA_STBITERR |
                                          SDIO_STA_TXUNDERR |
                                          SDIO_STA_DTIMEOUT |
                                          SDIO_STA_DCRCFAIL);
    const uint32_t DATA_TX_SUCCESS_FLAGS = (SDIO_STA_DBCKEND |
                                            SDIO_STA_DATAEND);

    while (!dma_get_interrupt_flag(DMA2, DMA_STREAM3, DMA_TCIF)) {
        // allow other tasks to run
        os::this_task::yield();
    }

    while (true) {
        volatile uint32_t result = SDIO_STA;
        // DBG("STA = 0x%x", result);
        // DBG("FIFOCNT = %d", SDIO_FIFOCNT);
        if (result & (DATA_TX_SUCCESS_FLAGS | DATA_TX_ERROR_FLAGS)) {
            if (result & DATA_TX_ERROR_FLAGS) {
                return false;
            } else if (result & DATA_TX_SUCCESS_FLAGS) {
                break;
            }
        }

        // allow other tasks to run
        os::this_task::yield();
    }

    return true;
}
