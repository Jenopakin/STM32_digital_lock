/*******************************************************************************
 * File Name    : code_decoder.h
 * Description  : Owns the SHORT/LONG classification logic and its timing
 *                threshold. Driver layer (timer_driver, exti_driver) only
 *                measures raw duration in ms - it has no idea what SHORT or
 *                LONG even means. This module is the only place that decides.
 * Date         : 2026-09-06
 ******************************************************************************/
#ifndef CODE_DECODER_H
#define CODE_DECODER_H

#include <stdint.h>
#include "stm32f411xe.h"

/* Exported enum -------------------------------------------------------------*/
typedef enum
{
    SYMBOL_SHORT = 0,
    SYMBOL_LONG  = 1
} Symbol_t;

/* Exported define -----------------------------------------------------------
 * Threshold that separates a SHORT press from a LONG press.
 * NOTE: this is a placeholder value for Week-1 bring-up. Once real timing
 * data is collected from the UART debug log (actual human press durations),
 * update this single value here - nowhere else in the codebase should
 * hardcode a duration threshold.
 * -----------------------------------------------------------------------------*/
#define SHORT_MAX_MS   (400UL)

/* Exported function prototypes ----------------------------------------------*/
Symbol_t CodeDecoder_Classify(uint32_t durationMs);

#endif /* CODE_DECODER_H */
