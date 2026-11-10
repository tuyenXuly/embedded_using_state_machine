/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "qpc.h"
#include "ClockAlarm_SM.h"
#include "LCD_I2C_lib.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BTN_PAD_VALUE_SET       2
#define BTN_PAD_VALUE_OK        1
#define BTN_PAD_VALUE_ABRT      3
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
CLCD_I2C_Name LCD1;

typedef enum{
    NOT_PRESSED,
    BOUNCE,
    PRESSED
}button_state_t;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void Q_onError(char const * const module, int_t const id) {
    (void)module;
    (void)id;
    __disable_irq();
    while (1) {
    }
}

static uint8_t process_button_pad_value(uint8_t btn_pad_value)
{
  static button_state_t btn_sm_state = NOT_PRESSED;
  static uint32_t curr_time = 0;

  switch(btn_sm_state){
    case NOT_PRESSED:{
      if(btn_pad_value){
        btn_sm_state = BOUNCE;
        curr_time = HAL_GetTick();
      }
      break;
    }
    case BOUNCE:{
      if(HAL_GetTick() - curr_time >= 50 ){
        //50ms has passed 
        if(btn_pad_value){
          btn_sm_state = PRESSED;
          return btn_pad_value;
        }
        else
          btn_sm_state = NOT_PRESSED;
      }
      break;
    }
    case PRESSED:{
      if(!btn_pad_value){
        btn_sm_state = BOUNCE;
        curr_time = HAL_GetTick();
      }
      break;
    }

  }

  return 0;
}

    static uint32_t tick_time = 0;
    static uint32_t alarm_check_time = 0;
    uint8_t b1,b2,btn_pad_value;
    QEvt ee;
    
    volatile uint32_t dwt_cycles = 0;
volatile uint32_t dwt_time_us = 0;
volatile uint32_t dwt_max_us = 0;

static void DWT_Init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    
    DWT->CYCCNT = 0U;
    
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
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
  MX_USART2_UART_Init();
  MX_TIM1_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  DWT_Init();
  CLCD_I2C_Init(&LCD1,&hi2c1,0x4e,20,4);
  CLCD_I2C_Clear(&LCD1);
  Clock_Alarm_ctor();
  QHSM_INIT(super_ClockAlarm, (void *)0, 0U);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {      
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    //1. read the button pad status
    b1 = (HAL_GPIO_ReadPin(PIN_BUTTON1_GPIO_Port, PIN_BUTTON1_Pin) == GPIO_PIN_RESET) ? 1U : 0U;
    b2 = (HAL_GPIO_ReadPin(PIN_BUTTON2_GPIO_Port, PIN_BUTTON2_Pin) == GPIO_PIN_RESET) ? 1U : 0U;
  
    btn_pad_value = (b1<<1u)|b2;

    //software button de-bouncing 
    btn_pad_value = process_button_pad_value(btn_pad_value);
  
    while(HAL_GetTick() - alarm_check_time >= 500 ){
    // send TICK event 
    alarm_check_time = HAL_GetTick();
    ee.sig = ALARM_SIG;
    QHSM_DISPATCH(super_ClockAlarm, &ee, 0U);
    }

    if(btn_pad_value){
      if(btn_pad_value == BTN_PAD_VALUE_SET){
        ee.sig  = SET_SIG; 
      }else if(btn_pad_value == BTN_PAD_VALUE_OK){
        ee.sig  = OK_SIG;
      }else if(btn_pad_value == BTN_PAD_VALUE_ABRT){
        ee.sig  = ABRT_SIG;
      }else 
      ee.sig = IGNORE_SIG;
      
    QHSM_DISPATCH(super_ClockAlarm, &ee, 0U);
    }
    
    while(HAL_GetTick() - tick_time >= 50){
    // send TICK event
    tick_time = HAL_GetTick();
    ee.sig = TICK_SIG;
    uint32_t start_cycle = DWT->CYCCNT;
    QHSM_DISPATCH(super_ClockAlarm, &ee, 0U);
    dwt_cycles = DWT->CYCCNT - start_cycle;
    dwt_time_us = dwt_cycles / (SystemCoreClock / 1000000U);
      if (dwt_time_us > dwt_max_us) {
          dwt_max_us = dwt_time_us;
      }
    }
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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
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
#ifdef USE_FULL_ASSERT
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
