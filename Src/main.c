/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"
#include "pdm2pcm.h"
#include "usb_host.h"
#include "audioI2S.h"
#include "MY_CS43L22.h"
#include "wav_player.h"
#include "string.h"
#include<stdio.h>
#include "stdbool.h"


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
extern ApplicationTypeDef Appli_state;
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CRC_HandleTypeDef hcrc;

I2C_HandleTypeDef hi2c1;

I2S_HandleTypeDef hi2s3;
DMA_HandleTypeDef hdma_spi3_tx;

UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */
int i = 0;
const char* wavFiles[] = {
		"mu/Track1.wav" ,"mu/Track2.wav" ,"mu/Track3.wav"};
int length = sizeof(wavFiles)/sizeof(wavFiles[0]);

/* USER CODE END PV */
uint8_t j = 150 ;
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2S3_Init(void);
static void MX_CRC_Init(void);
static void MX_USART3_UART_Init(void);
void MX_USB_HOST_Process(void);
void increase(){
	if  (j > 0){
		   CS43_SetVolume(j);
		   j++ ;
	}
}

void decrease(){
      if (j < 255){
		   CS43_SetVolume(j);
		   j-- ;
	}
}

void updateVolumeOnLCD(uint8_t volume) {
    // Convert volume to a string
    char volumeStr[4]; // Assuming volume is between 0 and 255
    sprintf(volumeStr, "%d", volume);

    // Clear the LCD and print the volume level
    lcd16x2_clear();
    lcd16x2_setCursor(0, 0); // Set cursor position
    lcd16x2_printf("Volume: ");
    lcd16x2_setCursor(0, 8); // Set cursor position for volume value
    lcd16x2_printf(volumeStr);
}

bool isMuted = false; // Variable to track mute state

void toggleMute() {
    if (isMuted) {
        // Unmute the player
        CS43_SetMute(false);
        isMuted = false;
    } else {
        // Mute the player
        CS43_SetMute(true);
        isMuted = true;
    }
}

/* USER CODE BEGIN PFP */
//uint8_t SmileyFaceChar[] = {0b00000, 0b01010, 0b11111, 0b11111, 0b11111, 0b01110, 0b00100, 0b00000};
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */


  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_I2S3_Init();
  MX_FATFS_Init();
  MX_USB_HOST_Init();
  MX_CRC_Init();
  MX_PDM2PCM_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */

  lcd16x2_init_4bits(GPIOE, GPIO_PIN_7, GPIO_PIN_8, GPIOE, GPIO_PIN_9, GPIO_PIN_10, GPIO_PIN_11, GPIO_PIN_14);
   	lcd16x2_setCursor(0, 3);
         lcd16x2_printf("Welcome");
         HAL_Delay(1000);
  CS43_Init(hi2c1, MODE_I2S);



   CS43_Enable_RightLeft(CS43_RIGHT_LEFT);

   bool isSdCardMounted=0;
    bool pauseResumeToggle=0;

   audioI2S_setHandle(&hi2s3);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

   while (1)
     {
       /* USER CODE END WHILE */
   	  MX_USB_HOST_Process();

   	  if(Appli_state == APPLICATION_START)
   	      {
   	        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
   	      }
   	      else if(Appli_state == APPLICATION_DISCONNECT)
   	      {
   	        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
   	        f_mount(NULL, (TCHAR const*)"", 0);
   	        isSdCardMounted = 0;
   	      }

   	      if(Appli_state == APPLICATION_READY)
   	      {
   	        if(!isSdCardMounted)
   	        {
   	          f_mount(&USBHFatFS, (const TCHAR*)USBHPath, 0);
   	          isSdCardMounted = 1;
   	        }

   	                       if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8) ==  GPIO_PIN_SET) {
   	                           HAL_Delay(200);

   	                           i = (i + 1) % (sizeof(wavFiles) / sizeof(wavFiles[0]));
   	                           lcd16x2_clear();

   	                                                       lcd16x2_setCursor(0, 0);
   	                                                       lcd16x2_printf("Selected Song:");
   	                                                       lcd16x2_setCursor(1, 2);
   	                                                       lcd16x2_printf(wavFiles[i]);

   	                                                    while (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8) == GPIO_PIN_SET) {
   	                                                     	            	   	   	             	                        HAL_Delay(50);
   	                                                     	            	   	   	             	                    }


   	                       }



   	          if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0))
   	          {
   	         	 HAL_Delay(100);
   	            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
   	            HAL_Delay(500);
   	            wavPlayer_fileSelect(wavFiles[i]);
   	            lcd16x2_clear();
   	                       lcd16x2_setCursor(0, 3);
   	                       lcd16x2_printf("Now Playing:");
   	                       lcd16x2_setCursor(1, 2);
   	                       lcd16x2_printf(wavFiles[i]);
   	            wavPlayer_play();

   	            while (!wavPlayer_isFinished()) {
   	             wavPlayer_process();
   	          if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8)) {
   	                  // Button is pressed, debounce it
   	                  HAL_Delay(50);
   	            	if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8)) {

   	            	 wavPlayer_stop();
   	            	i = (i + 1) % (sizeof(wavFiles) / sizeof(wavFiles[0]));
	                	  HAL_Delay(50);




   	            	   	   	             	                    wavPlayer_fileSelect(wavFiles[i]);

   	            	   	   	             	             HAL_Delay(50);


   	            	   	   	             	             HAL_Delay(50);
   	            	   	   	             	                    lcd16x2_clear();
   	            	   	   	             	                    lcd16x2_setCursor(0, 3);
   	            	   	   	             	                    lcd16x2_printf("Now Playing:");
   	            	   	   	             	                    lcd16x2_setCursor(1, 2);
   	            	   	   	             	                    lcd16x2_printf(wavFiles[i]);
   	            	   	   	             	          wavPlayer_play();


   	            	   	   	             	                    // Wait until the button is released
   	            	   	   	             	                    while (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8) == GPIO_PIN_SET) {
   	            	   	   	             	                        HAL_Delay(50);
   	            	   	   	             	                    }
   	            	   	   	             	                }}


   	             if (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_13))
   	               	                {
   	               	                	  HAL_Delay(50);
   	               	                	  wavPlayer_stop();
   	               	                	  wavPlayer_fileSelect(wavFiles[i]);
   	               	                	   wavPlayer_play();
   	               	                	   while (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_13) == GPIO_PIN_SET) {
   	               	                	  	                        HAL_Delay(50);
   	               	                	  	                    }
   	               	                }

   	             if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15) ==  GPIO_PIN_SET) {

   	            	 toggleMute();
   	            	 while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15) == GPIO_PIN_SET) {

   	            	   	                         	   HAL_Delay(400);
   	            	   	                           }
   	            	   	                       }

   	             if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) ==  GPIO_PIN_SET) {

   	           increase();

   	     updateVolumeOnLCD(j);
   	            	       // Debounce

   	            	     while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == GPIO_PIN_SET) {

   	            	       	                    }}



   	             if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13) ==  GPIO_PIN_SET) {

   	           decrease();
   	        updateVolumeOnLCD(j);
   	            	       // Debounce

   	            	     while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13) == GPIO_PIN_SET) {
   	            	    	 HAL_Delay(200);
   	            	       	                    }
   	            	       	                }






   	              if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0))
   	              {
   	                pauseResumeToggle^=1;
   	                if(pauseResumeToggle)
   	                {
   	                  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
   	                  wavPlayer_pause();
   	                  HAL_Delay(500);
   	                  lcd16x2_clear();
   	                                 lcd16x2_setCursor(0, 5);
   	                                 lcd16x2_printf("Pause");
   	                  HAL_Delay(200);
   	                }
   	                else
   	                {
   	                  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
   	                  HAL_Delay(1000);
   	                  if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0))
   	                  {
   	                    wavPlayer_stop();
   	                  }
   	                  {
   	                    wavPlayer_resume();
   	                    lcd16x2_clear();
   	                                         lcd16x2_setCursor(0, 5);
   	                                         lcd16x2_printf("Now Playing:");
   	                                         lcd16x2_setCursor(1, 2);
   	                                         lcd16x2_printf(wavFiles[i]);


   	                  }
   	                }
   	              }
   	            }
   	            lcd16x2_clear();
   	                                      lcd16x2_setCursor(0, 5);
   	                                      lcd16x2_printf("REsetting");
   	            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);

   	            HAL_Delay(1000);
   	          }
   	        }
   	      }}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CRC Initialization Function
  * @param None
  * @retval None
  */
static void MX_CRC_Init(void)
{

  /* USER CODE BEGIN CRC_Init 0 */

  /* USER CODE END CRC_Init 0 */

  /* USER CODE BEGIN CRC_Init 1 */

  /* USER CODE END CRC_Init 1 */
  hcrc.Instance = CRC;
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    Error_Handler();
  }
  __HAL_CRC_DR_RESET(&hcrc);
  /* USER CODE BEGIN CRC_Init 2 */

  /* USER CODE END CRC_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief I2S3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2S3_Init(void)
{

  /* USER CODE BEGIN I2S3_Init 0 */

  /* USER CODE END I2S3_Init 0 */

  /* USER CODE BEGIN I2S3_Init 1 */

  /* USER CODE END I2S3_Init 1 */
  hi2s3.Instance = SPI3;
  hi2s3.Init.Mode = I2S_MODE_MASTER_TX;
  hi2s3.Init.Standard = I2S_STANDARD_PHILIPS;
  hi2s3.Init.DataFormat = I2S_DATAFORMAT_16B;
  hi2s3.Init.MCLKOutput = I2S_MCLKOUTPUT_ENABLE;
  hi2s3.Init.AudioFreq = I2S_AUDIOFREQ_44K;
  hi2s3.Init.CPOL = I2S_CPOL_LOW;
  hi2s3.Init.ClockSource = I2S_CLOCK_PLL;
  hi2s3.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_DISABLE;
  if (HAL_I2S_Init(&hi2s3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2S3_Init 2 */

  /* USER CODE END I2S3_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0|GPIO_PIN_4, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_10
                          |GPIO_PIN_11, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10
                          |GPIO_PIN_11|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15
                          |GPIO_PIN_4, GPIO_PIN_RESET);

  /*Configure GPIO pins : PC0 PC4 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PA6 PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB2 PB10
                           PB11 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_10
                          |GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PE7 PE8 PE9 PE10
                           PE11 PE14 PE15 */
  GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10
                          |GPIO_PIN_11|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PE12 PE13 */
  GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PB13 PB14 PB15 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PD12 PD13 PD14 PD15
                           PD4 */
  GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15
                          |GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : PC8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
