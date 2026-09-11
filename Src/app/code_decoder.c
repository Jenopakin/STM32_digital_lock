/*******************************************************************************
 * File Name    : code_decoder.c
 * Description  : SHORT/LONG classification from a raw press duration.
 *                Week 1 scope: classification only. Sequence buffering,
 *                inter-symbol timeout, and full decode state machine are
 *                added in Week 2.
 * Date         : 2026-09-06
 ******************************************************************************/

#include "code_decoder.h"

/* Public functions ------------------------------------------------------------*/

/*********************************************************************
 * @fn      		  - CodeDecoder_Classify
 * @brief             - Classify a measured press duration as SHORT or LONG
 * @param[in]         - durationMs : press duration in milliseconds, as
 *                       measured by timer_driver (release tick - press tick)
 * @return            - SYMBOL_SHORT if durationMs < SHORT_MAX_MS, else
 *                       SYMBOL_LONG
 * @Note              - Strict less-than: a duration exactly equal to
 *                       SHORT_MAX_MS is classified as LONG. Kept as a single
 *                       comparison so MISRA reviewers can verify the
 *                       boundary condition at a glance.
 *////////////////////////////////////////////////////////////////////
Symbol_t CodeDecoder_Classify(uint32_t durationMs)
{
    Symbol_t result;

    if (durationMs < SHORT_MAX_MS)
    {
        result = SYMBOL_SHORT;
    }
    else
    {
        result = SYMBOL_LONG;
    }

    return result;
}
