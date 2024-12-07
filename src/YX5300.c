/**
 **********************************************************************************
 * @file   YX5300.c
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

/* Includes ---------------------------------------------------------------------*/
#include "YX5300.h"
#include <stdio.h>


/* Private Constants ------------------------------------------------------------*/
/**
 * @brief  Command bytes for the YX5300 MP3 module
 */
#define YX5300_CMD_START_BYTE       0x7E
#define YX5300_CMD_VERSION          0xFF
#define YX5300_CMD_NOT_FEEDBACK     0x00
#define YX5300_CMD_FEEDBACK         0x01
#define YX5300_CMD_END_BYTE         0xEF

/**
 * @brief  Commands for the YX5300 MP3 module
 */
#define YX5300_CMD_NEXT               0x01
#define YX5300_CMD_PREV               0x02
#define YX5300_CMD_PLAY_INDEX         0x03
#define YX5300_CMD_VOL_UP             0x04
#define YX5300_CMD_VOL_DOWN           0x05
#define YX5300_CMD_VOL_SET            0x06
#define YX5300_CMD_SINGLE_CYCLE       0x08
#define YX5300_CMD_SEL_DEV            0x09
#define YX5300_CMD_SLEEP_MODE         0x0A
#define YX5300_CMD_WAKE_UP            0x0B
#define YX5300_CMD_RESET              0x0C
#define YX5300_CMD_PLAY               0x0D
#define YX5300_CMD_PAUSE              0x0E
#define YX5300_CMD_PLAY_FOLD_FILE     0x0F
#define YX5300_CMD_STOP               0x16
#define YX5300_CMD_PLAY_CYCLE_FOLD    0x17
#define YX5300_CMD_SET_SNGL_CYCL      0x19
#define YX5300_CMD_SET_DAC            0x1A
#define YX5300_CMD_PLAY_WITH_VOL      0x22
#define YX5300_CMD_QUERY_STATUS       0x42
#define YX5300_CMD_QUERY_VOLUME       0x43
#define YX5300_CMD_QUERY_TOT_TRACKS   0x48
#define YX5300_CMD_PLAYING_N          0x4C
#define YX5300_CMD_QUERY_FLDR_TRACKS  0x4E
#define YX5300_CMD_QUERY_FLDR_COUNT   0x4F



/**
 ==================================================================================
                       ##### Private Functions #####
 ==================================================================================
 */

static YX5300_Result_t
YX5300_SendCommand(YX5300_Handler_t *Handler,
                   uint8_t Command, uint8_t Data1, uint8_t Data2)
{
  uint8_t Data[8] = {0};

  Data[0] = YX5300_CMD_START_BYTE;  // Start byte
  Data[1] = YX5300_CMD_VERSION;     // Version
  Data[2] = 0x06;                   // Length
  Data[3] = Command;                // Command
  Data[4] = YX5300_CMD_FEEDBACK;    // Feedback
  Data[5] = Data1;                  // Data1 or High byte of the data
  Data[6] = Data2;                  // Data2 or Low byte of the data
  Data[7] = YX5300_CMD_END_BYTE;    // End byte

  if (Handler->Platform.Send(Data, 8) < 0)
    return YX5300_FAIL;

  Handler->Status.LastCommand = Command;
  Handler->Status.LastCommandData = (Data1 << 8) | Data2;
  Handler->Status.LastResponse = 0;
  Handler->Status.LastResponseData = 0;

  return YX5300_OK;
}


static YX5300_Result_t
YX5300_ParseResponse(YX5300_Handler_t *Handler)
{
  // Response Structure  0x7E 0xFF 0x06 RSP 0x00 0x00 DAT 0xFE 0xBA 0xEF
  // RSP: Response code
  // DAT: Data

  Handler->Status.LastResponse = Handler->Rx.Buffer[3];
  Handler->Status.LastResponseData = (Handler->Rx.Buffer[5] << 8) | Handler->Rx.Buffer[6];

  switch (Handler->Status.LastResponse)
  {
  case 0x3A: // Memory card inserted
    Handler->Status.MemoryInserted = 1;
    break;

  case 0x3D: // Completed play num 'DAT'
    Handler->Status.Track = 0;
    break;

  case 0x40: // Error
    break;

  case 0x41: // Data received correctly
    break;

  case 0x42: // Status 'DAT'
    Handler->Status.StatusByte = Handler->Status.LastResponseData;
    if (Handler->Status.StatusByte == 0x00)
      Handler->Status.Track = 0;
    break;

  case 0x43: // Vol playing 'DAT'
    Handler->Status.Volume = Handler->Status.LastResponseData;
    break;

  case 0x48: // File count 'DAT'
    Handler->Status.Track = Handler->Status.LastResponseData;
    break;

  case 0x4C: // Folder 'DAT'
    break;

  case 0x4E: // Folder file count 'DAT'
    break;

  case 0x4F: // Folder file 'DAT'
    break;

  default: // Unrecognized response
    return YX5300_FAIL;
    break;
  }

  return YX5300_OK;
}



/**
 ==================================================================================
                            ##### Public Functions #####                           
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
YX5300_Init(YX5300_Handler_t *Handler)
{
  if (Handler == NULL)
    return YX5300_INVALID_PARAM;

  if (Handler->Platform.Delay == NULL ||
      Handler->Platform.Send == NULL)
    return YX5300_INVALID_PARAM;

  if (Handler->Platform.Init)
    Handler->Platform.Init();
  Handler->Platform.Delay(500);

  if (YX5300_SendCommand(Handler, YX5300_CMD_RESET, 0, 0) != YX5300_OK)
    return YX5300_FAIL;
  Handler->Platform.Delay(500);

  if (YX5300_SendCommand(Handler, YX5300_CMD_SEL_DEV, 0, 2) != YX5300_OK)
    return YX5300_FAIL;
  Handler->Platform.Delay(500);

  return YX5300_OK;
}


/**
 * @brief  Deinitialize function
 * @param  Handler: Pointer to handler
 * @retval YX5300_Result_t
 *         - YX5300_OK: Operation was successful.
 *         - YX5300_FAIL: Failed to send or receive data.
 */
YX5300_Result_t
YX5300_DeInit(YX5300_Handler_t *Handler)
{
  if (Handler == NULL)
    return YX5300_INVALID_PARAM;

  if (Handler->Platform.DeInit)
    Handler->Platform.DeInit();

  return YX5300_OK;
}


YX5300_Result_t
YX5300_Rx(YX5300_Handler_t *Handler, uint8_t Data)
{
  if (Handler->Rx.BufferIndex >= YX5300_RESPONSE_SIZE)
  {
    Handler->Rx.BufferIndex = 0;
  }
  Handler->Rx.Buffer[Handler->Rx.BufferIndex] = Data;

  if (Handler->Rx.BufferIndex == 0)
  {
    if (Data == YX5300_CMD_START_BYTE)
    {
      Handler->Rx.BufferIndex++;
    }
  }
  else
  {
    Handler->Rx.BufferIndex++;
    if (Data == YX5300_CMD_END_BYTE)
    {
      Handler->Rx.BufferIndex = 0;
      if (YX5300_ParseResponse(Handler) != YX5300_OK)
        return YX5300_FAIL;
      return YX5300_RX_COMPLETE;
    }
  }

  return YX5300_OK;
}


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
YX5300_UpdateStatus(YX5300_Handler_t *Handler)
{
  return YX5300_SendCommand(Handler, YX5300_CMD_QUERY_STATUS, 0, 0);
}


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
YX5300_UpdateVolume(YX5300_Handler_t *Handler)
{
  return YX5300_SendCommand(Handler, YX5300_CMD_QUERY_VOLUME, 0, 0);
}


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
YX5300_UpdateTrack(YX5300_Handler_t *Handler)
{
  return YX5300_SendCommand(Handler, YX5300_CMD_PLAYING_N, 0, 0);
}


/**
 * @brief  Play next track
 * @param  Handler: Pointer to handler
 * @retval YX5300_Result_t
 *         - YX5300_OK: Operation was successful.
 *         - YX5300_FAIL: Failed to send or receive data.
 */
YX5300_Result_t
YX5300_PlayNext(YX5300_Handler_t *Handler)
{
  return YX5300_SendCommand(Handler, YX5300_CMD_NEXT, 0, 0);
}


/**
 * @brief  Play previous track
 * @param  Handler: Pointer to handler
 * @retval YX5300_Result_t
 *         - YX5300_OK: Operation was successful.
 *         - YX5300_FAIL: Failed to send or receive data.
 */
YX5300_Result_t
YX5300_PlayPrev(YX5300_Handler_t *Handler)
{
  return YX5300_SendCommand(Handler, YX5300_CMD_PREV, 0, 0);
}


/**
 * @brief  Increase volume one step
 * @param  Handler: Pointer to handler
 * @retval YX5300_Result_t
 *         - YX5300_OK: Operation was successful.
 *         - YX5300_FAIL: Failed to send or receive data.
 */
YX5300_Result_t
YX5300_VolumeUp(YX5300_Handler_t *Handler)
{
  return YX5300_SendCommand(Handler, YX5300_CMD_VOL_UP, 0, 0);
}


/**
 * @brief  Decrease volume one step
 * @param  Handler: Pointer to handler
 * @retval YX5300_Result_t
 *         - YX5300_OK: Operation was successful.
 *         - YX5300_FAIL: Failed to send or receive data.
 */
YX5300_Result_t
YX5300_VolumeDown(YX5300_Handler_t *Handler)
{
  return YX5300_SendCommand(Handler, YX5300_CMD_VOL_DOWN, 0, 0);
}


/**
 * @brief  Set volume level
 * @param  Handler: Pointer to handler
 * @param  Volume: Volume level (0-30)
 * @retval YX5300_Result_t
 *         - YX5300_OK: Operation was successful.
 *         - YX5300_FAIL: Failed to send or receive data.
 */
YX5300_Result_t
YX5300_SetVolume(YX5300_Handler_t *Handler, uint8_t Volume)
{
  if (Volume > 30)
    Volume = 30;
  return YX5300_SendCommand(Handler, YX5300_CMD_VOL_SET, 0, Volume);
}


/**
 * @brief  Play track by index
 * @param  Handler: Pointer to handler
 * @param  Track: Track number
 * @retval YX5300_Result_t
 *         - YX5300_OK: Operation was successful.
 *         - YX5300_FAIL: Failed to send or receive data.
 */
YX5300_Result_t
YX5300_PlayTrack(YX5300_Handler_t *Handler, uint16_t Track)
{
  return YX5300_SendCommand(Handler, YX5300_CMD_PLAY_INDEX,
                            (uint8_t)(Track >> 8), (uint8_t)(Track & 0xFF));
}

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
YX5300_PlayFolderFile(YX5300_Handler_t *Handler, uint8_t Folder, uint8_t File)
{
  return YX5300_SendCommand(Handler, YX5300_CMD_PLAY_FOLD_FILE, Folder, File);
}


/**
 * @brief  Pause playback
 * @param  Handler: Pointer to handler
 * @retval YX5300_Result_t
 *         - YX5300_OK: Operation was successful.
 *         - YX5300_FAIL: Failed to send or receive data.
 */
YX5300_Result_t
YX5300_Resume(YX5300_Handler_t *Handler)
{
  return YX5300_SendCommand(Handler, YX5300_CMD_PLAY, 0, 0);
}


/**
 * @brief  Pause playback
 * @param  Handler: Pointer to handler
 * @retval YX5300_Result_t
 *         - YX5300_OK: Operation was successful.
 *         - YX5300_FAIL: Failed to send or receive data.
 */
YX5300_Result_t
YX5300_Pause(YX5300_Handler_t *Handler)
{
  return YX5300_SendCommand(Handler, YX5300_CMD_PAUSE, 0, 0);
}


/**
 * @brief  Stop playback
 * @param  Handler: Pointer to handler
 * @retval YX5300_Result_t
 *         - YX5300_OK: Operation was successful.
 *         - YX5300_FAIL: Failed to send or receive data.
 */
YX5300_Result_t
YX5300_Stop(YX5300_Handler_t *Handler)
{
  return YX5300_SendCommand(Handler, YX5300_CMD_STOP, 0, 0);
}
