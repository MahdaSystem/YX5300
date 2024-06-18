/**
 **********************************************************************************
 * @file   YX5300_platform.c
 * @author Hossein.M (https://github.com/Hossein-M98)
 * @brief  A sample Platform dependent layer for YX5300 Driver
 **********************************************************************************
 *
 * Copyright (c) 2024 Mahda Embedded System (MIT License)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 **********************************************************************************
 */
  
/* Includes ---------------------------------------------------------------------*/
#include "YX5300_platform.h"
#include "sdkconfig.h"
#include "esp_system.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"



/**
 ==================================================================================
                           ##### Private Functions #####                           
 ==================================================================================
 */

static int8_t
YX5300_Platform_Init(void)
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

static int8_t
YX5300_Platform_DeInit(void)
{
  uart_driver_delete(YX5300_UART_NUM);
  return 0;
}

static int8_t
YX5300_Platform_Delay(uint16_t Delay)
{
  vTaskDelay(Delay / portTICK_PERIOD_MS);
  return 0;
}

static int8_t
YX5300_Platform_Send(uint8_t *Data, uint8_t Len)
{
  uart_wait_tx_done(YX5300_UART_NUM, portMAX_DELAY);
  uart_write_bytes(YX5300_UART_NUM, Data, Len);
  
  return 0;
}



/**
 ==================================================================================
                            ##### Public Functions #####                           
 ==================================================================================
 */

/**
 * @brief  Initialize platform device to communicate YX5300.
 * @param  Handler: Pointer to handler
 * @retval None
 */
void
YX5300_Platform_Init(YX5300_Handler_t *Handler)
{
  YX5300_PLATFORM_LINK_INIT(Handler, YX5300_Platform_Init);
  YX5300_PLATFORM_LINK_DEINIT(Handler, YX5300_Platform_DeInit);
  YX5300_PLATFORM_LINK_DELAY(Handler, YX5300_Platform_Delay);
  YX5300_PLATFORM_LINK_SEND(Handler, YX5300_Platform_Send);
}
