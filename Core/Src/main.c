/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usbd_cdc_if.h"
//#include "ht16k33_7seg.h"
#include "ht16k33.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */


#define NUM_GPIO_PINS			7

#define HV_EN_PORT 				GPIOA
#define HV_EN_PIN 				GPIO_PIN_7
#define LED_PORT 				GPIOA
#define LED_PIN 				GPIO_PIN_8

#define RELAY_PORT				GPIOA
#define REL1 					GPIO_PIN_0
#define REL2 					GPIO_PIN_1
#define REL4 					GPIO_PIN_2
#define REL8 					GPIO_PIN_3
#define REL16 					GPIO_PIN_4
#define REL32 					GPIO_PIN_10
#define REL64 					GPIO_PIN_9

const uint8_t MAX_BRIGHT = 15;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

/* USER CODE BEGIN PV */

volatile int rotary_value = 0;



/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */
void convertToUint8Array(int8_t value, uint8_t array[4]);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t *data = "Hello World from USB CDC\n";

int8_t i = -127;
int ht16k33inrange = 0;
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
  MX_I2C1_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */

//  // Turn on the oscillator of the HT16K33
//  ht16k33_write_cmd(HT16K33_CMD_OSCILLATOR_ON);
//
//  // Turn on display and set brightness
//  ht16k33_write_cmd(HT16K33_CMD_DISPLAY_ON | HT16K33_DISPLAY_BRIGHTNESS_MAX);
//
//  // Set display to show all digits
//  ht16k33_write_display_buffer((uint16_t[]) {0, 0, 0, 0});


  	seg7_init(); //initialize display
  	seg7_setBrightness(5);		// 0 .. 15	  0 = off, 15 = max. brightness
  	HAL_Delay(100);
  	seg7_displayOn();	// enable display
  	HAL_GPIO_WritePin(HV_EN_PORT, HV_EN_PIN, SET); // turn on 12V boost.



  //uint8_t buffer[] = "Hello, World!";

  //strcpy((char*)buffer, "Hello, Loop!\n");
  //HAL_GPIO_WritePin(HV_EN_PORT, HV_EN_PIN, GPIO_PIN_SET);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  int i = 0;
	  uint8_t attenuationValue = rotary_value/2;
	  seg7_displayAtt(rotary_value/2);

//	  void seg7_display(uint8_t *array);
	  //CDC_Transmit_FS(buffer, strlen(buffer));
	  //HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
	  uint8_t buffer[20];
	  sprintf(buffer, "integer: %d", i);
	  CDC_Transmit_FS(buffer, sizeof(buffer));
	  setAttenuator(attenuationValue);
	  HAL_Delay(250);
	  i++;
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV2;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
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
  hi2c1.Init.ClockSpeed = 400000;
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
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
                          |GPIO_PIN_8, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA0 PA1 PA2 PA3
                           PA4 PA5 PA6 PA7
                           PA8 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
                          |GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB2 PB10 PB11
                           PB13 PB14 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_2|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_13|GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB5 PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}

/* USER CODE BEGIN 4 */

//void rotary_encoder_update() {
//	HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
//    static uint8_t last_state = 0;
//
//    uint8_t a_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5);
//    uint8_t b_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6);
//    uint8_t current_state = (a_state << 1) | b_state;
//
//    // if state is changed, update the last_debounce_time
//    if(current_state != last_state) {
//        last_debounce_time = HAL_GetTick();
//    }
//
//    // if more than DEBOUNCE_DELAY ms have passed, process the state
//    if((HAL_GetTick() - last_debounce_time) > DEBOUNCE_DELAY) {
//        // look at the changes from the last state to the current state
//        if(last_state == 0b00) {
//            // last_state = 0b00
//            if(current_state == 0b01) rotary_value++;
//            if(current_state == 0b10) rotary_value--;
//        } else if(last_state == 0b01) {
//            // last_state = 0b01
//            if(current_state == 0b11) rotary_value++;
//            if(current_state == 0b00) rotary_value--;
//        } else if(last_state == 0b10) {
//            // last_state = 0b10
//            if(current_state == 0b00) rotary_value++;
//            if(current_state == 0b11) rotary_value--;
//        } else {
//            // last_state = 0b11
//            if(current_state == 0b10) rotary_value++;
//            if(current_state == 0b01) rotary_value--;
//        }
//
//        // clamp the rotary_value to ROTARY_MIN_VALUE and ROTARY_MAX_VALUE
//        if(rotary_value > ROTARY_MAX_VALUE) rotary_value = ROTARY_MAX_VALUE;
//        if(rotary_value < ROTARY_MIN_VALUE) rotary_value = ROTARY_MIN_VALUE;
//    }
//
//    // update the last_state
//    last_state = current_state;
//}


void setAttenuator(uint8_t decimal) {
    if (decimal > 127) {
        //printf("Input out of range (0-127).\n");
        return;
    }

    int binary[NUM_GPIO_PINS] = {0};

    for (int i = NUM_GPIO_PINS - 1; i >= 0; --i) {
        binary[i] = decimal % 2;
        decimal = decimal / 2;
    }

    // Update GPIO states to reflect binary representation
    for (int i = 0; i < NUM_GPIO_PINS; ++i) {
    	switch(i) {
    	        case 0:
    	            if(binary[i] == 1){
    	            	HAL_GPIO_WritePin(RELAY_PORT, REL1, SET);
    	            }
    	            else{
    	            	HAL_GPIO_WritePin(RELAY_PORT, REL1, RESET);
    	            }
    	            break;
    	        case 1:
    	            if(binary[i] == 1){
    	            	HAL_GPIO_WritePin(RELAY_PORT, REL2, SET);
    	            }
    	            else{
    	            	HAL_GPIO_WritePin(RELAY_PORT, REL2, RESET);
    	            }
    	            break;
    	        case 2:
    	            if(binary[i] == 1){
    	            	HAL_GPIO_WritePin(RELAY_PORT, REL4, SET);
    	            }
    	            else{
    	            	HAL_GPIO_WritePin(RELAY_PORT, REL4, RESET);
    	            }
    	            break;
    	        case 3:
    	            if(binary[i] == 1){
    	            	HAL_GPIO_WritePin(RELAY_PORT, REL8, SET);
    	            }
    	            else{
    	            	HAL_GPIO_WritePin(RELAY_PORT, REL8, RESET);
    	            }
    	            break;
    	        case 4:
    	            if(binary[i] == 1){
    	            	HAL_GPIO_WritePin(RELAY_PORT, REL16, SET);
    	            }
    	            else{
    	            	HAL_GPIO_WritePin(RELAY_PORT, REL16, RESET);
    	            }
    	            break;
    	        case 5:
    	            if(binary[i] == 1){
    	            	HAL_GPIO_WritePin(RELAY_PORT, REL32, SET);
    	            }
    	            else{
    	            	HAL_GPIO_WritePin(RELAY_PORT, REL32, RESET);
    	            }
    	            break;
    	        case 6:
    	            if(binary[i] == 1){
    	            	HAL_GPIO_WritePin(RELAY_PORT, REL64, SET);
    	            }
    	            else{
    	            	HAL_GPIO_WritePin(RELAY_PORT, REL64, RESET);
    	            }
    	            break;
    	        default:

    	            break;
    	}
    }
}


void convertToUint8Array(int8_t value, uint8_t array[4])
{
    // Check if the value is negative
    if (value < 0) {
        array[0] = 17;  // Set the minus sign representation
        value *= -1;  // Convert the value to positive
    } else {
        array[0] = 0;  // Set the plus sign representation
    }

    // Convert the absolute value to a 4-element array
    array[3] = (uint8_t)(value % 10);
    array[2] = (uint8_t)((value / 10) % 10);
    array[1] = (uint8_t)((value / 100) % 10);
}

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
