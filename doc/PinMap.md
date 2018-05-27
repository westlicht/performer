# Pin Map

STM32F405RGT6

| Function      | Peripheral| Pin   | AF    | LQFP64 | Notes            |
| :---          | :---:     | :---: | :---: | :---:  | :---             |
| **POWER**     |           |       |       |        |                  |
| 3.3V          |           |       |       |        |                  |
| 5V            |           |       |       |        |                  |
| GND           |           |       |       |        |                  |
| **CONSOLE**   |           |       |       |        |                  |
| SYS_TX        | USART1    | PA9   | AF7   | 42     |                  |
| SYS_RX        | USART1    | PA10  | AF7   | 43     |                  |
| **MIDI**      |           |       |       |        |                  |
| MIDI_TX       | USART6    | PC6   | AF8   | 37     |                  |
| MIDI_RX       | USART6    | PC7   | AF8   | 38     |                  |
| **BLM/GATE**  |           |       |       |        |                  |
| SR_OE         | GPIO      | PA4   |       | 20     | added in rev 0.2 |
| SR_SCK        | SPI1      | PA5   | AF5   | 21     |                  |
| SR_MISO       | SPI1      | PA6   | AF5   | 22     |                  |
| SR_MOSI       | SPI1      | PA7   | AF5   | 23     |                  |
| SR_LATCH      | GPIO      | PC5   |       | 25     |                  |
| SR_LOAD       | GPIO      | PC4   |       | 24     |                  |
| **Encoder**   |           |       |       |        |                  |
| ENC_SW        | GPIO      | PC15  |       | 4      |                  |
| ENC_A         | GPIO      | PC13  |       | 2      |                  |
| ENC_B         | GPIO      | PC14  |       | 3      |                  |
| **LCD**       |           |       |       |        |                  |
| LCD_SCK       | SPI2      | PB13  | AF5   | 34     |                  |
| LCD_MOSI      | SPI2      | PC3   | AF5   | 11     |                  |
| LCD_CS        | GPIO      | PB12  |       | 33     |                  |
| LCD_RES       | GPIO      | PB14  |       | 35     |                  |
| LCD_DC        | GPIO      | PB15  |       | 36     |                  |
| **DAC**       |           |       |       |        |                  |
| DAC_SCK       | SPI3      | PC10  | AF6   | 51     |                  |
| DAC_MOSI      | SPI3      | PB5   | AF6   | 57     |                  |
| DAC_SYNC      | GPIO      | PB0   |       | 26     |                  |
| **ADC**       |           |       |       |        |                  |
| CV1_IN        | ADC1      | PA0   |       | 14     |                  |
| CV2_IN        | ADC1      | PA1   |       | 15     |                  |
| CV3_IN        | ADC1      | PA2   |       | 16     |                  |
| CV4_IN        | ADC1      | PA3   |       | 17     |                  |
| **DIO**       |           |       |       |        |                  |
| CLK_IN        | GPIO      | PB11  |       | 30     |                  |
| RST_IN        | GPIO      | PB10  |       | 29     |                  |
| CLK_OUT       | GPIO      | PB2   |       | 28     |                  |
| RST_OUT       | GPIO      | PB1   |       | 27     |                  |
| **SDCARD**    |           |       |       |        |                  |
| SDIO_D0       | SDIO      | PC8   |       | 39     |                  |
| SDIO_CK       | SDIO      | PC12  |       | 53     |                  |
| SDIO_CMD      | SDIO      | PD2   |       | 54     |                  |
| SDIO_CD       | GPIO      | PC11  |       | 52     | added in rev 0.2 |
| **USB**       |           |       |       |        |                  |
| USB_PWR_EN    | GPIO      | PC9   |       | 40     |                  |
| USB_PWR_FAULT | GPIO      | PA8   |       | 41     |                  |
| USB_D-        | USB       | PA11  | AF10  | 44     |                  |
| USB_D+        | USB       | PA12  | AF10  | 45     |                  |

# Early Prototype Pin Map

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
| DAC_SCK       | SPI5      | PF7   | AF5   | 11    |       |
| DAC_MOSI      | SPI5      | PF9   | AF5   | 56    |       |
| DAC_SYNC      | SPI5      | PF8   |       | 54    |       |
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
