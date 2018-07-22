### Task Priorities

_higher numbers are higher priorities_

| Task      | Priority  |
| :---      | :---:     |
| Driver    | 5         |
| Engine    | 4         |
| USBH      | 3         |
| UI        | 2         |
| File      | 1         |
| Profiler  | 0         |

### NVIC Priorities

_lower numbers are higher priorities_

| IRQ           | Driver                | Priority  |
| :---          | :---                  | :---:     |
| PendSV        | FreeRTOS              | 15        |
| SysTick       | FreeRTOS              | 15        |
| ---           | ---                   | ---       |
| TIM5          | ClockTimer            | 0         |
| USART1        | Console               | 0         |
| EXT15_10      | Dio                   | 0         |
| TIM2          | HighResolutionTimer   | 0         |
| DMA1_STREAM4  | Lcd                   | 0         |
| USART6        | Midi                  | 0         |


### Driver Interrupt Details

#### HighResolutionTimer

- interrupt only counts a global tick in us resolution
- elapsed = end - start properly handles wrap around

#### ClockTimer

- interrupt calls into sequencer clock

#### Dio

- interrupt calls back into sequencer clock

#### Lcd

- interrupt ends DMA transfer and sets txDone to true to enable next transfer

#### Midi



#### Console
