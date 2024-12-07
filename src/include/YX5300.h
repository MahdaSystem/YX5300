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


/* Exported Constants -----------------------------------------------------------*/
#define YX5300_RESPONSE_SIZE          10


/* Exported Data Types ----------------------------------------------------------*/
/**
 * @brief  Library functions result data type
 */
typedef enum YX5300_Result_e
{
  YX5300_OK              = 0,
  YX5300_FAIL            = 1,
  YX5300_INVALID_PARAM   = 2,
  YX5300_RX_COMPLETE     = 3,
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
typedef int8_t (*YX5300_Platform_Delay_t)(uint16_t Delay);

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

  // Rx Handler
  struct
  {
    uint8_t Buffer[YX5300_RESPONSE_SIZE];
    uint8_t BufferIndex;
    uint8_t InFrame;
  } Rx;

  // Status
  struct
  {
    uint8_t  LastCommand;
    uint8_t  LastResponse;
    uint16_t LastCommandData;
    uint16_t LastResponseData;

    uint8_t Volume;
    uint16_t Track; // if 0, it means no track is playing
    uint8_t StatusByte; // 0x00: Stop, 0x01: Play, 0x02: Pause

    uint8_t MemoryInserted;
  } Status;
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
 * @brief  Rx callback function
 * @param  Handler: Pointer to handler
 * @param  Data: Received data
 * @retval YX5300_Result_t
 *         - YX5300_OK: Operation was successful.
 *         - YX5300_FAIL: Failed to send or receive data.
 *         - YX5300_RX_COMPLETE: Frame received successfully and status updated.
 */
YX5300_Result_t
YX5300_Rx(YX5300_Handler_t *Handler, uint8_t Data);



/**
 ==================================================================================
                            ##### Status Functions #####                           
 ==================================================================================
 */

/**
 * @brief  Update status in handler
 * @note   After calling this function, user should wait for YX5300_RX_COMPLETE of
 *         YX5300_Rx callback function to be called. Then user can check the
 *         StatusByte in handler to see the current status.
 * @param  Handler: Pointer to handler
 * @retval YX5300_Result_t
 *         - YX5300_OK: Operation was successful.
 *         - YX5300_FAIL: Failed to send or receive data.
 */
YX5300_Result_t
YX5300_UpdateStatus(YX5300_Handler_t *Handler);


/**
 * @brief  Update current volume level
 * @note   After calling this function, user should wait for YX5300_RX_COMPLETE of
 *         YX5300_Rx callback function to be called. Then user can check the
 *         Volume in handler to see the current volume level.
 * @param  Handler: Pointer to handler
 * @retval YX5300_Result_t
 *         - YX5300_OK: Operation was successful.
 *         - YX5300_FAIL: Failed to send or receive data.
 *         - YX5300_INVALID_PARAM: Invalid parameter.
 */
YX5300_Result_t
YX5300_UpdateVolume(YX5300_Handler_t *Handler);


/**
 * @brief  Update current track number
 * @note   After calling this function, user should wait for YX5300_RX_COMPLETE of
 *         YX5300_Rx callback function to be called. Then user can check the
 *         Track in handler to see the current track number.
 * @param  Handler: Pointer to handler
 * @retval YX5300_Result_t
 *         - YX5300_OK: Operation was successful.
 *         - YX5300_FAIL: Failed to send or receive data.
 *         - YX5300_INVALID_PARAM: Invalid parameter.
 */
YX5300_Result_t
YX5300_UpdateTrack(YX5300_Handler_t *Handler);



/**
 ==================================================================================
                        ##### Media Control Functions #####                        
 ==================================================================================
 */

/**
 * @brief  Play next track
 * @param  Handler: Pointer to handler
 * @retval YX5300_Result_t
 *         - YX5300_OK: Operation was successful.
 *         - YX5300_FAIL: Failed to send or receive data.
 */
YX5300_Result_t
YX5300_PlayNext(YX5300_Handler_t *Handler);


/**
 * @brief  Play previous track
 * @param  Handler: Pointer to handler
 * @retval YX5300_Result_t
 *         - YX5300_OK: Operation was successful.
 *         - YX5300_FAIL: Failed to send or receive data.
 */
YX5300_Result_t
YX5300_PlayPrev(YX5300_Handler_t *Handler);


/**
 * @brief  Increase volume one step
 * @param  Handler: Pointer to handler
 * @retval YX5300_Result_t
 *         - YX5300_OK: Operation was successful.
 *         - YX5300_FAIL: Failed to send or receive data.
 */
YX5300_Result_t
YX5300_VolumeUp(YX5300_Handler_t *Handler);


/**
 * @brief  Decrease volume one step
 * @param  Handler: Pointer to handler
 * @retval YX5300_Result_t
 *         - YX5300_OK: Operation was successful.
 *         - YX5300_FAIL: Failed to send or receive data.
 */
YX5300_Result_t
YX5300_VolumeDown(YX5300_Handler_t *Handler);


/**
 * @brief  Set volume level
 * @param  Handler: Pointer to handler
 * @param  Volume: Volume level (0-30)
 * @retval YX5300_Result_t
 *         - YX5300_OK: Operation was successful.
 *         - YX5300_FAIL: Failed to send or receive data.
 */
YX5300_Result_t
YX5300_SetVolume(YX5300_Handler_t *Handler, uint8_t Volume);


/**
 * @brief  Play track by index
 * @param  Handler: Pointer to handler
 * @param  Track: Track number
 * @retval YX5300_Result_t
 *         - YX5300_OK: Operation was successful.
 *         - YX5300_FAIL: Failed to send or receive data.
 */
YX5300_Result_t
YX5300_PlayTrack(YX5300_Handler_t *Handler, uint16_t Track);


/**
 * @brief  Play track by folder and file number
 * @param  Handler: Pointer to handler
 * @param  Folder: Folder number
 * @param  File: File number
 * @retval YX5300_Result_t
 *         - YX5300_OK: Operation was successful.
 *         - YX5300_FAIL: Failed to send or receive data.
 */
YX5300_Result_t
YX5300_PlayFolderFile(YX5300_Handler_t *Handler, uint8_t Folder, uint8_t File);


/**
 * @brief  Pause playback
 * @param  Handler: Pointer to handler
 * @retval YX5300_Result_t
 *         - YX5300_OK: Operation was successful.
 *         - YX5300_FAIL: Failed to send or receive data.
 */
YX5300_Result_t
YX5300_Resume(YX5300_Handler_t *Handler);


/**
 * @brief  Pause playback
 * @param  Handler: Pointer to handler
 * @retval YX5300_Result_t
 *         - YX5300_OK: Operation was successful.
 *         - YX5300_FAIL: Failed to send or receive data.
 */
YX5300_Result_t
YX5300_Pause(YX5300_Handler_t *Handler);


/**
 * @brief  Stop playback
 * @param  Handler: Pointer to handler
 * @retval YX5300_Result_t
 *         - YX5300_OK: Operation was successful.
 *         - YX5300_FAIL: Failed to send or receive data.
 */
YX5300_Result_t
YX5300_Stop(YX5300_Handler_t *Handler);



#ifdef __cplusplus
}
#endif

#endif //! _YX5300_H_