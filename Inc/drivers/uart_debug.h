/*******************************************************************************
 * File Name    : uart_debug.h
 * Description  : Minimal interrupt-driven UART TX for bring-up debug output.
 *                Not the final audit-log UART driver (that comes Week 4) -
 *                this exists only to verify timing during Week 1 testing.
 * Date         : 2026-09-06
 ******************************************************************************/
#ifndef UART_DEBUG_H
#define UART_DEBUG_H

#include "stm32f411xe.h"
#include <stdint.h>

void UART_Debug_Init(void);
void UART_Debug_Print(const char * const pMsg);

#endif /* UART_DEBUG_H */
