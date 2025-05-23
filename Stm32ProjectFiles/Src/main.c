/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define RIGHT_MOTOR_PWM TIM_CHANNEL_2
#define LEFT_MOTOR_PWM  TIM_CHANNEL_4

#define GPIO_FR_In GPIOB,GPIO_PIN_6
#define GPIO_LS_In GPIOA,GPIO_PIN_6
#define GPIO_FM_In GPIOA,GPIO_PIN_5
#define GPIO_RS_In GPIOB ,GPIO_PIN_4
#define GPIO_FL_In GPIOA ,GPIO_PIN_12



/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
// Global variables to store encoder counts

// Declare variables to store encoder counts
volatile int EN1 = 0;
volatile int EN2 = 0;
volatile uint8_t right_wall ;
volatile uint8_t front_wall;
volatile uint8_t rightSensorStatus ;
volatile uint8_t leftSensorStatus;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */
void MoveMotorLeftForward(uint8_t speed);
void MoveMotorRightForward(uint8_t speed);
void MoveMotorLeftBackward(uint8_t speed);
void MoveMotorRightBackward(uint8_t speed);
void FollowLine(void);
void set_motor_speed(uint32_t right_speed, uint32_t left_speed);
int read_sensor(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void follow_right_wall(void);

void turn_90_degrees_left(void);

int get_right_encoder_count(void);
int get_left_encoder_count(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// Define constants



void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GPIO_PIN_7)
    {


    	GPIO_PinState pinState = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11);

    	        // Update encoder count based on the state of GPIO_PIN_11
    	        if (pinState == GPIO_PIN_SET)
    	        {
    	            EN1++;
    	        }
    	        else
    	        {
    	            EN1--;
    	        }

    }

    if (GPIO_Pin == GPIO_PIN_0)
    {
        // Read current state of encoder pin PB0

    	GPIO_PinState pinState2 = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8);

    	 // Update encoder count based on the state of GPIO_PIN_11
    	    	        if (pinState2 == GPIO_PIN_SET)
    	    	        {
    	    	            EN2++;
    	    	        }
    	    	        else
    	    	        {
    	    	            EN2--;
    	    	        }
    }
}


void turn_90_degrees_left() {
	EN1 = 0;
	EN2 = 0;
	    while (EN1 <=220 && EN2 <= 220) {

	        MoveMotorRightForward(55);
	        MoveMotorLeftBackward(60);

	    }
}

void stop(){
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, 0);
}



void MoveMotorLeftBackward(uint8_t speed) {
    // Set motor to spin forward
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5,GPIO_PIN_SET ); // BIN1 = High
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET); // BIN2 = Low

    // Set speed

    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, speed);
    //HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); // Ensure PWM is started

}
void MoveMotorLeftForward(uint8_t speed) {
    // Set motor to spin backward
	 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET); // BIN1 = Low
	 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET); // BIN2 = High

    // Set speed

    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, speed);
   // HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); // Ensure PWM is started
}

void MoveMotorRightBackward(uint8_t speed) {
    // Set motor2 to spin forward
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);   //AIN1 = High
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET); // AIN2 = Low

    // Set speed for motor2

    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, speed);

}

void MoveMotorRightForward(uint8_t speed) {
    // Set motor2 to spin backward
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); // IN1 = Low
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);   // BIN2 = High

    // Set speed for motor2

    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, speed);

}
void FollowLine(void) {
    int leftSensorStatus = HAL_GPIO_ReadPin(GPIOA, GPIO_FL_In_Pin);  // Read left IR sensor
    int rightSensorStatus = HAL_GPIO_ReadPin(GPIOB,GPIO_FR_In_Pin); // Read right IR sensor

    if (leftSensorStatus == GPIO_PIN_RESET && rightSensorStatus == GPIO_PIN_RESET) {
        // Both sensors detect the line, move forward
        MoveMotorLeftForward(65);
        MoveMotorRightForward(70);

    } else if (leftSensorStatus == GPIO_PIN_RESET && rightSensorStatus == GPIO_PIN_SET) {
        // Left sensor detects the line, right sen doesnt detect, turn slightly left
        MoveMotorLeftForward(20);
        MoveMotorRightForward(80);

    } else if  ( leftSensorStatus== GPIO_PIN_SET && rightSensorStatus == GPIO_PIN_RESET){

        // Left sensor doesnt detects the line, right sensor detects the line, turn slightly right
        MoveMotorLeftForward(80);
        MoveMotorRightForward(24);


    }


}

void follow_right_wall(void) {


	//GPIO_PIN_RESET - wall detecting
	 right_wall = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6);
     front_wall = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5);
     rightSensorStatus = HAL_GPIO_ReadPin(GPIOB, GPIO_FR_In_Pin);
     leftSensorStatus = HAL_GPIO_ReadPin(GPIOA, GPIO_FL_In_Pin);





    if (right_wall == GPIO_PIN_RESET){


   	//go straight
   	   MoveMotorLeftForward(55);
   	   MoveMotorRightForward(60);

    }else if(right_wall == GPIO_PIN_SET) {
       // Slightly speed up left motor to straighten the robot
   	    MoveMotorLeftForward(80);
   	    MoveMotorRightForward(30);

    }else if(front_wall == GPIO_PIN_RESET){

    	turn_90_degrees_left();

    }else if (rightSensorStatus == GPIO_PIN_SET) {

    	MoveMotorLeftForward(30);
        MoveMotorRightForward(70);
        FollowLine();

    }


}








int read_sensor(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) {
    return HAL_GPIO_ReadPin(GPIOx, GPIO_Pin);
}


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
  MX_TIM2_Init();
  MX_USART2_UART_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  // Initialization code somewhere after configuring the PWM channel


    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); 									// Start PWM on TIM2, Channel 2     (Motor B)
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4); 									// Start PWM on TIM2, Channel       (Motor A)



  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

//	  if((front_wall == GPIO_PIN_SET) &&  (right_wall == GPIO_PIN_SET )  && (left_wall == GPIO_PIN_SET)){
//
//	  			MoveMotorLeftForward(40);
//	  	     	MoveMotorRightForward(60);
//
//	  	    	FollowLine();


	  	    //}+
//	 char msg[64];
//	          int msg_length = sprintf(msg, "Ticks R : %d, Ticks L : %d\r\n", EN1, EN2);
//	          HAL_UART_Transmit(&huart2, (uint8_t*)msg, msg_length, HAL_MAX_DELAY);
//



	  follow_right_wall();




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

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 16;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable MSI Auto calibration
  */
  HAL_RCCEx_EnableMSIPLLMode();
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 31;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 99;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|LD3_Pin|GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA4 PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : GPIO_FM_In_Pin PA6 GPIO_IN_LE_Pin GPIO_IN_RE_Pin
                           GPIO_FL_In_Pin */
  GPIO_InitStruct.Pin = GPIO_FM_In_Pin|GPIO_PIN_6|GPIO_IN_LE_Pin|GPIO_IN_RE_Pin
                          |GPIO_FL_In_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : GPIO_EXTI0_LE_Pin GPIO_EXTI7_RE_Pin */
  GPIO_InitStruct.Pin = GPIO_EXTI0_LE_Pin|GPIO_EXTI7_RE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB1 LD3_Pin PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|LD3_Pin|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : GPIO_FR_In_Pin GPIO_FM_InB6_Pin */
  GPIO_InitStruct.Pin = GPIO_FR_In_Pin|GPIO_FM_InB6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

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
