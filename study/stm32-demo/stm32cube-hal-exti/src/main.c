#include "stm32f1xx_hal.h"

#define LED_PIN GPIO_PIN_13
#define LED_GPIO_PORT GPIOC
#define LED_GPIO_CLK_ENABLE() __HAL_RCC_GPIOC_CLK_ENABLE()

#define KEY_PIN GPIO_PIN_0
#define KEY_GPIO_PORT GPIOA


int main(void)
{
    HAL_StatusTypeDef ret = HAL_ERROR;

    ret = HAL_Init();
    if (ret != HAL_OK)
    {
        goto END;
    }


    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitTypeDef GPIO_InitStruct2;

    LED_GPIO_CLK_ENABLE();
    GPIO_InitStruct.Pin = LED_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_GPIO_PORT, &GPIO_InitStruct);

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_AFIO_CLK_ENABLE();
    GPIO_InitStruct2.Pin = KEY_PIN;
    GPIO_InitStruct2.Mode = GPIO_MODE_IT_FALLING | GPIO_MODE_AF_INPUT;
    GPIO_InitStruct2.Pull = GPIO_NOPULL;
    GPIO_InitStruct2.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(KEY_GPIO_PORT, &GPIO_InitStruct2);
 

    //HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_0);
    HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
    //HAL_NVIC_SetPendingIRQ(EXTI0_IRQn);
    //HAL_NVIC_EnableIRQ(EXTI0_IRQn);

    while (1)
    {
        ;
    }

    END:
    HAL_DeInit();
}

/*
void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(KEY_PIN);
}*/

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == KEY_PIN)
    {
        HAL_GPIO_TogglePin(LED_GPIO_PORT, LED_PIN);
    }
}

void HAL_MspInit(void)
{
}

void HAL_MspDeInit(void)
{
}

void SysTick_Handler(void)
{
    HAL_IncTick();
}

void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
    while (1)
    {
    }
}

void MemManage_Handler(void)
{
    while (1)
    {
    }
}

void BusFault_Handler(void)
{
    while (1)
    {
    }
}

void UsageFault_Handler(void)
{
    while (1)
    {
    }
}

void SVC_Handler(void)
{
}

void DebugMon_Handler(void)
{
}

void PendSV_Handler(void)
{
}
