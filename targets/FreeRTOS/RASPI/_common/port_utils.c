//
// Copyright (c) .NET Foundation and Contributors
// Copyright (c) Kentaro Sekimoto
// See LICENSE file in the project root for full license information.
//

#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"

#include "hardware/exception.h"

void DEBUG_PRINTF(const char *fmt, ...);

//
// FreeRTOS
//
#if configUSE_MALLOC_FAILED_HOOK
void vApplicationMallocFailedHook(void) {
    __asm ("bkpt #0");
}
#endif

#if configCHECK_FOR_STACK_OVERFLOW
void vApplicationStackOverflowHook( TaskHandle_t xTask, char * pcTaskName ) {
    (void)xTask;
    (void)pcTaskName;
    __asm ("bkpt #0");
}
#endif

// get task's stack size
// #define configUSE_TRACE_FACILITY                1
// #define configUSE_STATS_FORMATTING_FUNCTIONS    1
#define STACK_INFO_SIZE	320		// number of tasks x 40 bytes
static uint8_t m_stack_info[STACK_INFO_SIZE];
void print_task_stack_size(void) {
    vTaskList((char *)m_stack_info);
    DEBUG_PRINTF("%s\n", (char *)m_stack_info);
}

// defined in hardfault.c
void HardFault_Handler(void);

void set_hardfault_handler(void) {
    exception_set_exclusive_handler(HARDFAULT_EXCEPTION, HardFault_Handler);
}

// __get_IPSR()
uint32_t __get_IPSR(void)
{
  uint32_t result;

  __asm volatile ("MRS %0, ipsr" : "=r" (result) );
  return(result);
}
