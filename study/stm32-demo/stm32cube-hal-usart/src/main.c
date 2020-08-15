#include "stm32f1xx_hal.h"

/* USART1 PA9 - TX，  PA10 - RX */

uint8_t rData[1];
uint8_t tData[] = "this is test data.";
USART_HandleTypeDef usart1;

int main(void) {

  HAL_Init();
  // SystemClock_Config();

  /* 第二步： HAL_USART_Init */
  usart1.Instance = USART1;
  usart1.State = HAL_USART_STATE_RESET;
  usart1.Init.BaudRate = 9600;
  usart1.Init.WordLength = USART_WORDLENGTH_8B;
  usart1.Init.StopBits = USART_STOPBITS_1;
  usart1.Init.Parity = USART_PARITY_NONE;
  usart1.Init.Mode = USART_MODE_TX_RX;
  usart1.Init.CLKPolarity = USART_POLARITY_HIGH;
  usart1.Init.CLKPhase = USART_PHASE_1EDGE;
  usart1.Init.CLKLastBit = USART_LASTBIT_DISABLE;

  if (HAL_USART_Init(&usart1) != HAL_OK) {
    //_Error_Handler(__FILE__, __LINE__);
  }

  HAL_UART_Transmit(&usart1, tData, sizeof(tData), 1);

//  while (1) {
    
  //  HAL_Delay(5);
 // }
}

void HAL_USART_MspInit(USART_HandleTypeDef *husart) {
  GPIO_InitTypeDef GPIO_InitStruct;

  if (husart->Instance == USART1) {

    /* 第一步： 配置USART1 IO及clock等 */
    __HAL_RCC_USART1_CLK_ENABLE(); //__HAL_RCC_USART1_CLK_DISABLE
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* USART1 GPIO Tx Rx */
    GPIO_InitStruct.Pin = GPIO_PIN_9; // tx  pa9, rx  pa10
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10; // tx  pa9, rx  pa10
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);


    /* USER CODE BEGIN USART1_MspInit 1 */
    /* 开启接收中断，并初始化串口接收指针 */
    // HAL_UART_Receive_IT(husart, rData, 1);
    /* USER CODE BEGIN USART1_MspInit 1 */

  }


}

