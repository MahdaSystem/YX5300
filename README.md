# YX5300 Library
Driver for YX5300 MP3 Player Module.


## Hardware Support
It is easy to port this library to any platform. But now it is ready for use in:
- ESP32 (esp-idf)


## How To Use
1. Add `YX5300.h` and `YX5300.c` files to your project.  It is optional to use `YX5300_platform.h` and `YX5300_platform.c` files (open and config `YX5300_platform.h` file).
2. Initialize platform-dependent part of handler.
4. Call `YX5300_Init()`.
5. Call other functions and enjoy.


## Example
<details>
<summary>Using YX5300_platform files</summary>

```c
#include <stdio.h>
#include "YX5300.h"
#include "YX5300_platform.h"

int main(void)
{
  YX5300_Handler_t Handler = {0};

  YX5300_Platform_Init(&Handler);
  YX5300_Init(&Handler);
  YX5300_SetVolume(&Handler, 30);
  YX5300_PlayTrack(&Handler, 1);

  while (1)
  {
    DelayMs(10000);
    YX5300_PlayNext(&Handler);
  }

  YX5300_DeInit(&Handler);
  return 0;
}
```
</details>


<details>
<summary>Without using YX5300_platform files (esp-idf)</summary>

```c
#include <stdio.h>
#include <stdint.h>
#include "sdkconfig.h"
#include "esp_system.h"
#include "esp_err.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "YX5300.h"

#define YX5300_UART_NUM       UART_NUM_2
#define YX5300_UART_TXD_GPIO  GPIO_NUM_23
#define YX5300_UART_RXD_GPIO  GPIO_NUM_19

static int8_t YX5300_Platform_Init(void)
{
  uart_config_t uart_config = {
      .baud_rate = 9600,
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
      .source_clk = UART_SCLK_APB};
  uart_param_config(YX5300_UART_NUM, &uart_config);
  uart_set_pin(YX5300_UART_NUM, YX5300_UART_TXD_GPIO, YX5300_UART_RXD_GPIO, -1, -1);
  uart_driver_install(YX5300_UART_NUM, 256, 256, 0, NULL, 0);

  return 0;
}

static int8_t YX5300_Platform_DeInit(void)
{
  uart_driver_delete(YX5300_UART_NUM);
  return 0;
}

static int8_t YX5300_Platform_Delay(uint16_t Delay)
{
  vTaskDelay(Delay / portTICK_PERIOD_MS);
  return 0;
}

static int8_t YX5300_Platform_Send(uint8_t *Data, uint8_t Len)
{
  uart_wait_tx_done(YX5300_UART_NUM, portMAX_DELAY);
  uart_write_bytes(YX5300_UART_NUM, Data, Len);
  
  return 0;
}


void app_main(void)
{
  YX5300_Handler_t Handler = {0};

  YX5300_PLATFORM_LINK_INIT(&Handler, YX5300_Platform_Init);
  YX5300_PLATFORM_LINK_DEINIT(&Handler, YX5300_Platform_DeInit);
  YX5300_PLATFORM_LINK_DELAY(&Handler, YX5300_Platform_Delay);
  YX5300_PLATFORM_LINK_SEND(&Handler, YX5300_Platform_Send);

  YX5300_Init(&Handler);
  YX5300_SetVolume(&Handler, 30);
  YX5300_PlayTrack(&Handler, 1);

  while (1)
  {
    DelayMs(10000);
    YX5300_PlayNext(&Handler);
  }

  YX5300_DeInit(&Handler);
  return 0;
}
```
</details>