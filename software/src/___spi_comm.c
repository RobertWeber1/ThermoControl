// /* USER CODE BEGIN Header */
// /**
//   ******************************************************************************
//   * @file           : main.c
//   * @brief          : Main program body
//   ******************************************************************************
//   * @attention
//   *
//   * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
//   * All rights reserved.</center></h2>
//   *
//   * This software component is licensed by ST under BSD 3-Clause license,
//   * the "License"; You may not use this file except in compliance with the
//   * License. You may obtain a copy of the License at:
//   *                        opensource.org/licenses/BSD-3-Clause
//   *
//   ******************************************************************************
//   */
// /* USER CODE END Header */

// /* Includes ------------------------------------------------------------------*/
// #include "main.h"

// /* Private includes ----------------------------------------------------------*/
// /* USER CODE BEGIN Includes */
// #include  <errno.h>
// #include  <sys/unistd.h> // STDOUT_FILENO, STDERR_FILENO
// #include <stdio.h>
// /* USER CODE END Includes */

// /* Private typedef -----------------------------------------------------------*/
// /* USER CODE BEGIN PTD */

// /* USER CODE END PTD */

// /* Private define ------------------------------------------------------------*/
// /* USER CODE BEGIN PD */
// /* USER CODE END PD */

// /* Private macro -------------------------------------------------------------*/
// /* USER CODE BEGIN PM */

// /* USER CODE END PM */

// /* Private variables ---------------------------------------------------------*/
// SPI_HandleTypeDef hspi1;
// SPI_HandleTypeDef hspi2;
// SPI_HandleTypeDef hspi3;

// UART_HandleTypeDef huart3;

// /* USER CODE BEGIN PV */

// /* USER CODE END PV */

// /* Private function prototypes -----------------------------------------------*/
// void SystemClock_Config(void);
// static void MX_GPIO_Init(void);
// static void MX_SPI1_Init(void);
// static void MX_SPI2_Init(void);
// static void MX_USART3_UART_Init(void);
// static void MX_SPI3_Init(void);
// /* USER CODE BEGIN PFP */

// /* USER CODE END PFP */

// /* Private user code ---------------------------------------------------------*/
// /* USER CODE BEGIN 0 */
// uint16_t slave1_buffer = 0x0123;


// // void spi_slave1_done(SPI_HandleTypeDef *hspi)
// // {
// //   printf("SPI Slave1 buffer: %04x\n", slave1_buffer);
// //   HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);

// //   HAL_SPI_TransmitReceive_IT(&hspi2, &slave1_buffer, &slave1_buffer, 1);
// // }

// // uint16_t slave2_buffer = 0x4567;

// // void spi_slave2_done(SPI_HandleTypeDef *hspi)
// // {
// //   printf("SPI Slave2 buffer: %04x\n", slave2_buffer);
// //   HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);

// //   HAL_SPI_TransmitReceive_IT(&hspi3, &slave2_buffer, &slave2_buffer, 1);
// // }
// void print_array(uint16_t const* data, uint16_t size)
// {
//   for(uint16_t i = 0; i < size; ++i)
//   {
//     if(i!=0)
//     {
//       printf(", ");
//     }
//     printf("%04x", data[i]);
//   }
// }

// /* USER CODE END 0 */

// /**
//   * @brief  The application entry point.
//   * @retval int
//   */
// int main(void)
// {
//   /* USER CODE BEGIN 1 */

//   /* USER CODE END 1 */


//   /* MCU Configuration--------------------------------------------------------*/

//   /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
//   HAL_Init();

//   /* USER CODE BEGIN Init */

//   /* USER CODE END Init */

//   /* Configure the system clock */
//   SystemClock_Config();

//   /* USER CODE BEGIN SysInit */

//   /* USER CODE END SysInit */

//   /* Initialize all configured peripherals */
//   MX_GPIO_Init();
//   MX_SPI1_Init();
//   MX_SPI2_Init();
//   MX_USART3_UART_Init();
//   MX_SPI3_Init();
//   /* USER CODE BEGIN 2 */

//   puts("\n\n---------------------------------------------\n");


//   HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
//   HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
//   HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET);
//   HAL_GPIO_WritePin(SPI_CHAIN_NSS_GPIO_Port, SPI_CHAIN_NSS_Pin, GPIO_PIN_SET);

//     // char data[] = "Hallo Welt!";
//     // HAL_UART_Transmit(&huart3, data, sizeof(data), 10000);
//   // printf("Setup SlaveSPI2 to receive data\n");

//   // printf(
//   //   "HAL_SPI_RegisterCallback1: %d\n",
//   //   HAL_SPI_RegisterCallback(
//   //     &hspi2,
//   //     HAL_SPI_TX_RX_COMPLETE_CB_ID,
//   //     &spi_slave1_done));

//   // printf(
//   //   "HAL_SPI_TransmitReceive_IT1: %d\n",
//   //   HAL_SPI_TransmitReceive_IT(
//   //     &hspi2,
//   //     &slave1_buffer,
//   //     &slave1_buffer,
//   //     1));

//   // printf(
//   //   "HAL_SPI_RegisterCallback1: %d\n",
//   //   HAL_SPI_RegisterCallback(
//   //     &hspi3,
//   //     HAL_SPI_TX_RX_COMPLETE_CB_ID,
//   //     &spi_slave2_done));

//   // printf(
//   //   "HAL_SPI_TransmitReceive_IT2: %d\n",
//   //   HAL_SPI_TransmitReceive_IT(
//   //     &hspi3,
//   //     &slave2_buffer,
//   //     &slave2_buffer,
//   //     1));

//   /*
// HAL_SPI_TransmitReceive_IT(
//       &hspi2,
//       &slave_send_buffer,
//       &slave_recv_buffer,
//       1)
//   */

//   uint16_t send[2*4] = {0x434D, 0x0003, 0x434D, 0x0002, 0x434D, 0x0001, 0x434D, 0x0000};
//   uint16_t recv[2*4] = {0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff};
//   HAL_GPIO_WritePin(SPI_CHAIN_NSS_GPIO_Port, SPI_CHAIN_NSS_Pin, GPIO_PIN_SET);



//   /* USER CODE END 2 */

//   /* Infinite loop */
//   /* USER CODE BEGIN WHILE */
//   int counter = 1;
//   while (1)
//   {

//     // printf("\n#########################loop\n");
//     HAL_GPIO_WritePin(SPI_CHAIN_NSS_GPIO_Port, SPI_CHAIN_NSS_Pin, GPIO_PIN_RESET);
//     for(int i=0; i<8; i+=2)
//     {
//       HAL_SPI_TransmitReceive(&hspi1, (uint8_t*)&send[i], (uint8_t*)&recv[i], 2, 10000);
//       HAL_Delay(100);
//     }
//     HAL_GPIO_WritePin(SPI_CHAIN_NSS_GPIO_Port, SPI_CHAIN_NSS_Pin, GPIO_PIN_SET);

//     printf("recv: "); print_array(recv, 2*4); printf("\n");

//     // HAL_GPIO_WritePin(SPI_CHAIN_NSS_GPIO_Port, SPI_CHAIN_NSS_Pin, GPIO_PIN_SET);
//     // printf("send: "); print_array(send, 2*4); printf("\n");

//     // ++send[0];
//     // ++send[1];
//     // ++send[2];
//     // ++send[3];
//     // ++send[4];
//     // ++send[5];
//     // ++send[6];
//     // ++send[7];
//     HAL_Delay(1000);
//     counter += 2;

//     if(counter > 7)
//     {
//       counter = 1;
//     }

//     printf("counter: %d\n", counter);
//     for(int i=1; i<=9; i+=2)
//     {
//       printf("i(%d) == counter? -> %d\n", i, (counter == i));
//       send[i] = send[i] & 0x00ff | ((counter == i) ? 0x0100 : 0x0000);
//     }

//     printf("send: "); print_array(send, 2*4); printf("\n");
//     // printf("MAIN slave2 send: %04x, recv: %04x\n", slave_send_buffer, slave_recv_buffer);
//     /* USER CODE END WHILE */

//     /* USER CODE BEGIN 3 */
//   }
//   /* USER CODE END 3 */
// }

// /**
//   * @brief System Clock Configuration
//   * @retval None
//   */
// void SystemClock_Config(void)
// {
//   RCC_OscInitTypeDef RCC_OscInitStruct = {0};
//   RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

//   /** Configure the main internal regulator output voltage
//   */
//   __HAL_RCC_PWR_CLK_ENABLE();
//   __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
//   /** Initializes the CPU, AHB and APB busses clocks
//   */
//   RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
//   RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
//   RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
//   RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
//   RCC_OscInitStruct.PLL.PLLM = 8;
//   RCC_OscInitStruct.PLL.PLLN = 384;
//   RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
//   RCC_OscInitStruct.PLL.PLLQ = 8;
//   RCC_OscInitStruct.PLL.PLLR = 2;
//   if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
//   {
//     Error_Handler();
//   }
//   /** Initializes the CPU, AHB and APB busses clocks
//   */
//   RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
//                               |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
//   RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
//   RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV16;
//   RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
//   RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV16;

//   if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
//   {
//     Error_Handler();
//   }
// }

// /**
//   * @brief SPI1 Initialization Function
//   * @param None
//   * @retval None
//   */
// static void MX_SPI1_Init(void)
// {

//   /* USER CODE BEGIN SPI1_Init 0 */

//   /* USER CODE END SPI1_Init 0 */

//   /* USER CODE BEGIN SPI1_Init 1 */

//   /* USER CODE END SPI1_Init 1 */
//   /* SPI1 parameter configuration*/
//   hspi1.Instance = SPI1;
//   hspi1.Init.Mode = SPI_MODE_MASTER;
//   hspi1.Init.Direction = SPI_DIRECTION_2LINES;
//   hspi1.Init.DataSize = SPI_DATASIZE_16BIT;
//   hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
//   hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
//   hspi1.Init.NSS = SPI_NSS_SOFT;
//   hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
//   hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
//   hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
//   hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
//   hspi1.Init.CRCPolynomial = 10;
//   if (HAL_SPI_Init(&hspi1) != HAL_OK)
//   {
//     Error_Handler();
//   }
//   /* USER CODE BEGIN SPI1_Init 2 */

//   /* USER CODE END SPI1_Init 2 */

// }

// /**
//   * @brief SPI2 Initialization Function
//   * @param None
//   * @retval None
//   */
// static void MX_SPI2_Init(void)
// {

//   /* USER CODE BEGIN SPI2_Init 0 */

//   /* USER CODE END SPI2_Init 0 */

//   /* USER CODE BEGIN SPI2_Init 1 */

//   /* USER CODE END SPI2_Init 1 */
//   /* SPI2 parameter configuration*/
//   hspi2.Instance = SPI2;
//   hspi2.Init.Mode = SPI_MODE_SLAVE;
//   hspi2.Init.Direction = SPI_DIRECTION_2LINES;
//   hspi2.Init.DataSize = SPI_DATASIZE_16BIT;
//   hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
//   hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
//   hspi2.Init.NSS = SPI_NSS_HARD_INPUT;
//   hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
//   hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
//   hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
//   hspi2.Init.CRCPolynomial = 10;
//   if (HAL_SPI_Init(&hspi2) != HAL_OK)
//   {
//     Error_Handler();
//   }
//   /* USER CODE BEGIN SPI2_Init 2 */

//   /* USER CODE END SPI2_Init 2 */

// }

// /**
//   * @brief SPI3 Initialization Function
//   * @param None
//   * @retval None
//   */
// static void MX_SPI3_Init(void)
// {

//   /* USER CODE BEGIN SPI3_Init 0 */

//   /* USER CODE END SPI3_Init 0 */

//   /* USER CODE BEGIN SPI3_Init 1 */

//   /* USER CODE END SPI3_Init 1 */
//   /* SPI3 parameter configuration*/
//   hspi3.Instance = SPI3;
//   hspi3.Init.Mode = SPI_MODE_SLAVE;
//   hspi3.Init.Direction = SPI_DIRECTION_2LINES;
//   hspi3.Init.DataSize = SPI_DATASIZE_16BIT;
//   hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
//   hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
//   hspi3.Init.NSS = SPI_NSS_SOFT;
//   hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
//   hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
//   hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
//   hspi3.Init.CRCPolynomial = 10;
//   if (HAL_SPI_Init(&hspi3) != HAL_OK)
//   {
//     Error_Handler();
//   }
//   /* USER CODE BEGIN SPI3_Init 2 */

//   /* USER CODE END SPI3_Init 2 */

// }

// /**
//   * @brief USART3 Initialization Function
//   * @param None
//   * @retval None
//   */
// static void MX_USART3_UART_Init(void)
// {

//   /* USER CODE BEGIN USART3_Init 0 */

//   /* USER CODE END USART3_Init 0 */

//   /* USER CODE BEGIN USART3_Init 1 */

//   /* USER CODE END USART3_Init 1 */
//   huart3.Instance = USART3;
//   huart3.Init.BaudRate = 115200;
//   huart3.Init.WordLength = UART_WORDLENGTH_8B;
//   huart3.Init.StopBits = UART_STOPBITS_1;
//   huart3.Init.Parity = UART_PARITY_NONE;
//   huart3.Init.Mode = UART_MODE_TX_RX;
//   huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
//   huart3.Init.OverSampling = UART_OVERSAMPLING_16;
//   if (HAL_UART_Init(&huart3) != HAL_OK)
//   {
//     Error_Handler();
//   }
//   /* USER CODE BEGIN USART3_Init 2 */

//   /* USER CODE END USART3_Init 2 */

// }

// /**
//   * @brief GPIO Initialization Function
//   * @param None
//   * @retval None
//   */
// static void MX_GPIO_Init(void)
// {
//   GPIO_InitTypeDef GPIO_InitStruct = {0};

//   /* GPIO Ports Clock Enable */
//   __HAL_RCC_GPIOC_CLK_ENABLE();
//   __HAL_RCC_GPIOH_CLK_ENABLE();
//   __HAL_RCC_GPIOA_CLK_ENABLE();
//   __HAL_RCC_GPIOB_CLK_ENABLE();
//   __HAL_RCC_GPIOD_CLK_ENABLE();
//   __HAL_RCC_GPIOG_CLK_ENABLE();

//   /*Configure GPIO pin Output Level */
//   HAL_GPIO_WritePin(SPI_CHAIN_NSS_GPIO_Port, SPI_CHAIN_NSS_Pin, GPIO_PIN_RESET);

//   /*Configure GPIO pin Output Level */
//   HAL_GPIO_WritePin(GPIOB, LED1_Pin|LED3_Pin|LED2_Pin, GPIO_PIN_RESET);

//   /*Configure GPIO pin Output Level */
//   HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_RESET);

//   /*Configure GPIO pin : PC13 */
//   GPIO_InitStruct.Pin = GPIO_PIN_13;
//   GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
//   GPIO_InitStruct.Pull = GPIO_NOPULL;
//   HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

//   /*Configure GPIO pin : SPI_CHAIN_NSS_Pin */
//   GPIO_InitStruct.Pin = SPI_CHAIN_NSS_Pin;
//   GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//   GPIO_InitStruct.Pull = GPIO_NOPULL;
//   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
//   HAL_GPIO_Init(SPI_CHAIN_NSS_GPIO_Port, &GPIO_InitStruct);

//   /*Configure GPIO pins : LED1_Pin LED3_Pin LED2_Pin */
//   GPIO_InitStruct.Pin = LED1_Pin|LED3_Pin|LED2_Pin;
//   GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//   GPIO_InitStruct.Pull = GPIO_NOPULL;
//   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//   HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

//   /*Configure GPIO pin : PG6 */
//   GPIO_InitStruct.Pin = GPIO_PIN_6;
//   GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//   GPIO_InitStruct.Pull = GPIO_NOPULL;
//   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//   HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

//   /*Configure GPIO pin : PG7 */
//   GPIO_InitStruct.Pin = GPIO_PIN_7;
//   GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//   GPIO_InitStruct.Pull = GPIO_NOPULL;
//   HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

// }

// /* USER CODE BEGIN 4 */
// int _write(int file, char *data, int len)
// {
//    if ((file != STDOUT_FILENO) && (file != STDERR_FILENO))
//    {
//       errno = EBADF;
//       return -1;
//    }

//    // arbitrary timeout 1000
//    HAL_StatusTypeDef status =
//       HAL_UART_Transmit(&huart3, (uint8_t*)data, len, 1000);

//    // return # of bytes written - as best we can tell
//    return (status == HAL_OK ? len : 0);
// }

// /* USER CODE END 4 */

// /**
//   * @brief  This function is executed in case of error occurrence.
//   * @retval None
//   */
// void Error_Handler(void)
// {
//   /* USER CODE BEGIN Error_Handler_Debug */
//   /* User can add his own implementation to report the HAL error return state */

//   /* USER CODE END Error_Handler_Debug */
// }

// #ifdef  USE_FULL_ASSERT
// /**
//   * @brief  Reports the name of the source file and the source line number
//   *         where the assert_param error has occurred.
//   * @param  file: pointer to the source file name
//   * @param  line: assert_param error line source number
//   * @retval None
//   */
// void assert_failed(uint8_t *file, uint32_t line)
// {
//   /* USER CODE BEGIN 6 */
//   /* User can add his own implementation to report the file name and line number,
//      tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
//   /* USER CODE END 6 */
// }
// #endif /* USE_FULL_ASSERT */

// /************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
