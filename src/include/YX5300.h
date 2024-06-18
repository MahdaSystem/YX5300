/**
 **********************************************************************************
 * @file   YX5300.h
 * @author Hossein.M (https://github.com/Hossein-M98)
 * @brief  YX5300 MP3 module driver library
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

/* Define to prevent recursive inclusion ----------------------------------------*/
#ifndef	_YX5300_H_
#define _YX5300_H_

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ---------------------------------------------------------------------*/
#include <stdint.h>


/* Exported Data Types ----------------------------------------------------------*/
/**
 * @brief  Library functions result data type
 */
typedef enum YX5300_Result_e
{
  YX5300_OK              = 0,
  YX5300_FAIL            = 1,
  YX5300_INVALID_PARAM   = 2,
} YX5300_Result_t;


/**
 * @brief  Function type for Initialize/Deinitialize the platform dependent layer.
 * @retval 
 *         -  0: The operation was successful.
 *         - -1: The operation failed. 
 */
typedef int8_t (*YX5300_Platform_InitDeinit_t)(void);

/**
 * @brief  Function type for Delay.
 * @retval 
 *         -  0: The operation was successful.
 *         - -1: The operation failed. 
 */
typedef int8_t (*YX5300_Platform_Delay_t)(uint8_t Delay);

/**
 * @brief  Function type for Send data through UART.
 * @param  Data: Pointer to data to send
 * @param  Len: data len in Bytes
 * @retval 
 *         -  0: The operation was successful.
 *         - -1: Failed to send.
 */
typedef int8_t (*YX5300_Platform_Send_t)(uint8_t *Data,
                                         uint8_t Len);

/**
 * @brief  Platform dependent layer data type
 * @note   It is optional to initialize this functions:
 *         - Init
 *         - DeInit
 * @note   It is mandatory to initialize this functions:
 *         - Send
 *         - Receive
 * @note   If success the functions must return 0 
 */
typedef struct YX5300_Platform_s
{
  // Initialize platform dependent layer
  YX5300_Platform_InitDeinit_t Init;
  // De-initialize platform dependent layer
  YX5300_Platform_InitDeinit_t DeInit;

  // Delay function in ms
  YX5300_Platform_Delay_t Delay;

  // Send data
  YX5300_Platform_Send_t Send;
} YX5300_Platform_t;


/**
 * @brief  Handler data type
 * @note   User must initialize platform dependent layer functions
 */
typedef struct YX5300_Handler_s
{
  // Platform dependent layer
  YX5300_Platform_t Platform;
} YX5300_Handler_t;


/* Exported Macros --------------------------------------------------------------*/
/**
 * @brief  Link platform dependent layer functions to handler
 * @param  HANDLER: Pointer to handler
 * @param  FUNC: Function name
 */
#define YX5300_PLATFORM_LINK_INIT(HANDLER, FUNC) \
  (HANDLER)->Platform.Init = FUNC

/**
 * @brief  Link platform dependent layer functions to handler
 * @param  HANDLER: Pointer to handler
 * @param  FUNC: Function name
 */
#define YX5300_PLATFORM_LINK_DEINIT(HANDLER, FUNC) \
  (HANDLER)->Platform.DeInit = FUNC

/**
 * @brief  Link platform dependent layer functions to handler
 * @param  HANDLER: Pointer to handler
 * @param  FUNC: Function name
 */
#define YX5300_PLATFORM_LINK_DELAY(HANDLER, FUNC) \
  (HANDLER)->Platform.Delay = FUNC

/**
 * @brief  Link platform dependent layer functions to handler
 * @param  HANDLER: Pointer to handler
 * @param  FUNC: Function name
 */
#define YX5300_PLATFORM_LINK_SEND(HANDLER, FUNC) \
  (HANDLER)->Platform.Send = FUNC



/**
 ==================================================================================
                        ##### Initialization Functions #####                       
 ==================================================================================
 */

/**
 * @brief  Initializer function
 * @note   This function must be called after initializing platform dependent
 *         layer and before using other functions.
 * @param  Handler: Pointer to handler
 * @retval YX5300_Result_t
 *         - YX5300_OK: Operation was successful.
 *         - YX5300_FAIL: Failed to send or receive data.
 *         - YX5300_INVALID_PARAM: Invalid parameter.
 */
YX5300_Result_t
YX5300_Init(YX5300_Handler_t *Handler);


/**
 * @brief  Deinitialize function
 * @param  Handler: Pointer to handler
 * @retval YX5300_Result_t
 *         - YX5300_OK: Operation was successful.
 *         - YX5300_FAIL: Failed to send or receive data.
 */
YX5300_Result_t
YX5300_DeInit(YX5300_Handler_t *Handler);



/**
 ==================================================================================
                        ##### Media Control Functions #####                        
 ==================================================================================
 */





#ifdef __cplusplus
}
#endif

#endif //! _YX5300_H_