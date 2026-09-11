/*******************************************************************************
 * File Name    : uart_debug.c
 * Description  : USART2 @115200 baud, interrupt-driven TX with a small ring
 *                buffer, PA2 = TX (AF07). Debug-only utility for Week 1.
 * Date         : 2026-09-06
 ******************************************************************************/

#include "uart_debug.h"
#include "stm32f411xe.h"

/* Private define ---------------------------------------------------------------
 * BRR value for 115200 baud @ APB1=16MHz (HSI default, no PLL configured yet)
 * ------------------------------------------------------------------------------*/
#define USART2_BRR_115200   (139UL)
#define TX_BUF_SIZE         (128U)   /* power of 2 -> allows mask instead of modulo */
#define TX_BUF_MASK         (TX_BUF_SIZE - 1U)

/* Private variables --------------------------------------------------------------*/
static volatile char     s_txBuf[TX_BUF_SIZE];
static volatile uint16_t s_txHead = 0U;   /* next free slot to write into   */
static volatile uint16_t s_txTail = 0U;   /* next byte to send out          */

/* Public functions -----------------------------------------------------------*/

/*********************************************************************
 * @fn      		  - UART_Debug_Init
 * @brief             - Configure PA2 as USART2_TX (AF07), 115200-8N1, TX only
 *////////////////////////////////////////////////////////////////////
void UART_Debug_Init(void)
{
    /* 1. GPIOA clock + PA2 alternate function mode */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    (void)RCC->AHB1ENR;

    GPIOA->MODER &= ~(0x3UL << (2U * 2U));
    GPIOA->MODER |=  (0x2UL << (2U * 2U));           /* AF mode on PA2 */
    GPIOA->AFR[0] &= ~(0xFUL << (2U * 4U));
    GPIOA->AFR[0] |=  (0x7UL << (2U * 4U));           /* AF07 = USART2 */
    GPIOA->OSPEEDR |= (0x3UL << (2U * 2U));           /* high speed for clean edges */

    /* 2. USART2 clock + config */
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    (void)RCC->APB1ENR;

    USART2->BRR = USART2_BRR_115200;
    USART2->CR1 = 0U;                 /* clear first: fresh known state */
    USART2->CR1 |= USART_CR1_TE;      /* transmitter enable */

    NVIC_SetPriority(USART2_IRQn, 3U);
    NVIC_EnableIRQ(USART2_IRQn);

    USART2->CR1 |= USART_CR1_UE;      /* USART enable (must be last) */
}

/*********************************************************************
 * @fn      		  - UART_Debug_Print
 * @brief             - Push a null-terminated string into the TX ring buffer
 *                       and kick off transmission via TXE interrupt.
 * @Note              - Debug-only: no overflow protection beyond silently
 *                       dropping bytes if the buffer is full. Fine for
 *                       occasional bring-up prints, NOT acceptable for the
 *                       final audit_log module (Week 4 needs proper handling).
 *////////////////////////////////////////////////////////////////////
void UART_Debug_Print(const char * const pMsg)
{
    uint16_t i = 0U;

    while (pMsg[i] != '\0')
    {
        uint16_t const nextHead = (uint16_t)((s_txHead + 1U) & TX_BUF_MASK);

        if (nextHead != s_txTail)   /* buffer not full */
        {
            s_txBuf[s_txHead] = pMsg[i];
            s_txHead = nextHead;
        }

        i++;
    }

    USART2->CR1 |= USART_CR1_TXEIE;  /* make sure ISR starts pumping bytes out */
}

/*********************************************************************
 * @fn      		  - USART2_IRQHandler
 * @brief             - Fires when TDR is empty; pushes next byte from ring
 *                       buffer, or disables TXEIE when buffer is drained.
 * @Note              - TXE clears automatically on write to DR, unlike
 *                       EXTI->PR / TIM->SR which need a manual clear.
 *////////////////////////////////////////////////////////////////////
void USART2_IRQHandler(void)
{
    if ((USART2->SR & USART_SR_TXE) != 0U)
    {
        if (s_txTail != s_txHead)
        {
            USART2->DR = (uint32_t)s_txBuf[s_txTail];
            s_txTail = (uint16_t)((s_txTail + 1U) & TX_BUF_MASK);
        }
        else
        {
            USART2->CR1 &= ~USART_CR1_TXEIE;  /* nothing left to send */
        }
    }
}
