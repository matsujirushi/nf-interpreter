//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#include "ff.h"
#include <nanoHAL_Windows_Storage.h>
#include <Target_Windows_Storage.h>
#include <nanoHAL_v2.h>

// need to declare this here as extern
extern void PostManagedEvent(uint8_t category, uint8_t subCategory, uint16_t data1, uint32_t data2);

#define SD_CARD_DRIVE_INDEX             "0"
#define SD_CARD_DRIVE_INDEX_NUMERIC     (0)


///////////////////////////////////////////
// code specific to SD Card

// FS for SD Card mounted and ready
bool sdCardFileSystemReady;

static FATFS sdCard_FS;

/*!
* call back function for SD card detect.
*
* @param isInserted  true,  indicate the card is insert.
*                    false, indicate the card is remove.
* @param userData
*/

static void SDCARD_DetectCallBack(bool isInserted, void *userData)
{
    (void)userData;
    // ToDo: implement
}

static void CardDetectTask(void *pvParameters)
{
    // ToDo: implement
}

void SdCardThread(void * argument)
{
    (void)argument;
    
    // ToDo: implement

    xTaskCreate(CardDetectTask, "CardDetectTask", configMINIMAL_STACK_SIZE + 500, NULL, configMAX_PRIORITIES - 2, NULL);

    vTaskDelete(NULL);
}


///////////////////////////////////////////

