# Prototype Pin Mappings

NUCLEO-F429ZI

| Function      | Peripheral| Pin   | AF    | CN11  | CN12  | Conflict  |
| :---          | :---:     | :---: | :---: | :---: | :---: | :---:     |
| **POWER**     |           |       |       |       |       |
| 3.3V          |           |       |       | 16    |       |
| 5V            |           |       |       | 18    |       |
| GND           |           |       |       | 20    |       |
| **CONSOLE**   |           |       |       |       |       |
| CONSOLE_TX    | USART3    | PD8   | AF7   |       |       |
| CONSOLE_RX    | USART3    | PD9   | AF7   |       |       |
| **MIDI**      |           |       |       |       |       |
| MIDI_TX       | USART2    | PD5   | AF7   | 41    |       |
| MIDI_RX       | USART2    | PD6   | AF7   | 43    |       |
| **BLM/GATE**  |           |       |       |       |       |
| SR_SCK        | SPI4      | PE2   | AF5   | 46    |       |
| SR_MISO       | SPI4      | PE5   | AF5   | 50    |       |
| SR_MOSI       | SPI4      | PE6   | AF5   | 62    |       |
| SR_LATCH      | GPIO      | PE3   |       | 47    |       |
| SR_LOAD       | GPIO      | PE4   |       | 48    |       |
| **Encoder**   |           |       |       |       |       |
| GND           |           |       |       |       | 39    |
| ENC_SWITCH    | GPIO      | PD13  |       |       | 41    |
| ENC_A         | GPIO      | PD12  |       |       | 43    |
| ENC_B         | GPIO      | PD11  |       |       | 45    |
| **LCD**       |           |       |       |       |       |
| LCD_SCK       | SPI1      | PA5   |       |       | 11    | Alt
| LCD_MOSI      | SPI1      | PA7   |       |       | 15    | Alt
| LCD_SCK       | SPI3      | PC10  |       | 1     |       | Cur
| LCD_MOSI      | SPI3      | PC12  |       | 3     |       | Cur
| LCD_CS        | GPIO      | PC8   |       |       | 2     |
| LCD_RES       | GPIO      | PC9   |       |       | 1     |
| LCD_DC        | GPIO      | PC11  |       | 2     |       |
| **DAC**       |           |       |       |       |       |
| DAC_SCK       | SPI5      | PF7   |       | 11    |       |
| DAC_MISO      | SPI5      | PF8   |       | 54    |       |
| DAC_MOSI      | SPI5      | PF9   |       | 56    |       |
| **ADC**       |           |       |       |       |       |
| ADC_0         | ADC0      | PD3   |       | 40    |       |
| ADC_1         | ADC0      | PD4   |       | 39    |       |
| ADC_2         | ADC0      | PD5   |       | 41    |       |
| ADC_3         | ADC0      | PD6   |       | 43    |       |
| **DIO**       |           |       |       |       |       |
| CLK_IN        | GPIO      | PE10  |       |       | 47    |
| RST_IN        | GPIO      | PE11  |       |       | 56    |
| CLK_OUT       | GPIO      | PE12  |       |       | 49    |
| RST_OUT       | GPIO      | PE13  |       |       | 55    |
| **SDCARD**    |           |       |       |       |       |
| SD_D0         | SDIO      | PC8   |       |       | 2     | LCD
| SD_CK         | SDIO      | PC12  |       | 3     |       | LCD
| SD_CMD        | SDIO      | PD2   |       | 4     |       |

