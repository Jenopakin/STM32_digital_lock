/*******************************************************************************
 * File Name    : timer_driver.c
 * Description  : TIM2-based 1ms tick counter, used by app layer to measure
 *                button press duration, inter-symbol timeout, and (later)
 *                lockout countdown.
 * Date         : 2026-09-06
 ******************************************************************************/

#include "timer_driver.h"

/* Private define --------------------------------------------------------------
 * Assumes default reset clock config: HSI 16MHz, AHB/APB1 prescaler = 1
 * -> TIM2 input clock = 16 MHz
 * PSC = 15999 -> counter clock = 16MHz / 16000 = 1 kHz (1 tick = 1 ms)
 * ARR = 0     -> update event fires every 1 counter tick = every 1 ms
 * -----------------------------------------------------------------------------*/
#define TIMER_PSC_1MS  (15999UL)
#define TIMER_ARR_1MS  (0UL)

/* Private variables -----------------------------------------------------------*/
static volatile uint32_t s_tickMs = 0UL;

/* Public functions ------------------------------------------------------------*/

/*********************************************************************
 * @fn      		  - TIMER_Init
 * @brief             - Configure TIM2 to generate an update interrupt
 *                       every 1 ms and start it
 * @return            - none
 *////////////////////////////////////////////////////////////////////
void TIMER_Init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    (void)RCC->APB1ENR; /* dummy read-back, same rationale as GPIO clock enable */

    TIM2->PSC = TIMER_PSC_1MS;
    TIM2->ARR = TIMER_ARR_1MS;
    TIM2->CNT = 0UL;

    TIM2->SR &= ~TIM_SR_UIF;   /* clear any stale pending flag before enabling IRQ */
    TIM2->DIER |= TIM_DIER_UIE;

    NVIC_SetPriority(TIM2_IRQn, 2U);
    NVIC_EnableIRQ(TIM2_IRQn);

    TIM2->CR1 |= TIM_CR1_CEN;
}

/*********************************************************************
 * @fn      		  - TIMER_GetTick
 * @brief             - Return current tick count in milliseconds since
 *                       TIMER_Init() was called
 *////////////////////////////////////////////////////////////////////
uint32_t TIMER_GetTick(void)
{
    return s_tickMs;
}

/*********************************************************************
 * @fn      		  - TIMER_DelayMs
 * @brief             - Blocking delay, FOR WEEK-1 BRING-UP TESTING ONLY.
 * @Note              - Must NOT be used inside the final state machine or
 *                       any ISR — it would stall EXTI/UART responsiveness.
 *                       Kept here only to sanity-check TIMER_GetTick().
 *////////////////////////////////////////////////////////////////////
void TIMER_DelayMs(uint32_t ms)
{
    uint32_t const start = TIMER_GetTick();

    while ((TIMER_GetTick() - start) < ms)
    {
        /* busy-wait intentionally, test-only */
    }
}

/*********************************************************************
 * @fn      		  - TIM2_IRQHandler
 * @brief             - ISR: fires every 1 ms, increments tick counter
 * @Note              - TIM update flag (UIF) does NOT clear on read like
 *                       UART RXNE or ADC EOC — must be cleared manually,
 *                       same category as EXTI->PR.
 *////////////////////////////////////////////////////////////////////
void TIM2_IRQHandler(void)
{
    if ((TIM2->SR & TIM_SR_UIF) != 0U)
    {
        TIM2->SR &= ~TIM_SR_UIF;
        s_tickMs++;
    }
}
