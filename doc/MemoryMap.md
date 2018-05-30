# Memory Map

## STM32F405RGT6 Flash Sectors

| Sector | Address    | Size   |
| :---   | :---       | :---   |
| 0      | 0x08000000 | 16 KB  |
| 1      | 0x08004000 | 16 KB  |
| 2      | 0x08008000 | 16 KB  |
| 3      | 0x0800C000 | 16 KB  |
| 4      | 0x08010000 | 64 KB  |
| 5      | 0x08020000 | 128 KB |
| 6      | 0x08040000 | 128 KB |
| 7      | 0x08060000 | 128 KB |
| 8      | 0x08080000 | 128 KB |
| 9      | 0x080A0000 | 128 KB |
| 10     | 0x080C0000 | 128 KB |
| 11     | 0x080E0000 | 128 KB |

## STM32F405RGT6 Ram Regions

| Region | Address    | Size   |
| :---   | :---       | :---   |
| CCMRAM | 0x10000000 | 64 KB  |
| RAM    | 0x20000000 | 128 KB |

## Flash Memory Map

| Region                  | Size   | Description          |
| :---                    | :---   | :---                 |
| 0x08000000 - 0x08007FFF | 32 KB  | Bootloader           |
| 0x08008000 - 0x0800BFFF | 16 KB  | Hardware Settings    |
| 0x0800C000 - 0x0800FFFF | 16 KB  | Application Settings |
| 0x08010000 - 0x080DFFFF | 960 KB | Application          |
