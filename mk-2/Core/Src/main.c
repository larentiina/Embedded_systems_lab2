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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "kb.h"
#include "sdk_uart.h"
#include "pca9538.h"
#include "oled.h"
#include "fonts.h"
#include "tim.h"
#include "buzzer.h"
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

/* USER CODE BEGIN PV */
uint32_t megalovania_melody[] = {
		N_C4, N_C4, N_D4, N_C4, N_F4, N_E4,
		    0,
		    N_C4, N_C4, N_D4, N_C4, N_G4, N_F4,
		    0,
		    N_C4, N_C4, N_C5, N_A4, N_F4, N_E4, N_D4,
		    0,
		    N_CS4, N_CS4, N_C5, N_A4, N_F4, N_E4, N_D4
};

uint32_t megalovania_delays[] = {
		16, 16, 32, 32, 16, 16, // Happy Birthday to you
		    16, 16, 32, 32, 16, 16, // Happy Birthday to you
		    16, 16, 32, 32, 16, 16, 16, // Happy Birthday dear (Name)
		    16, 16, 32, 32, 16, 16  // Happy Birthday to you
};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void KB_Test( void );
void OLED_TIMER(uint8_t OLED_Keys[4]);
void TIMER_NOW( void );
void oled_Reset( void );
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
  MX_I2C1_Init();
  MX_USART6_UART_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  oled_Init();
  MX_TIM2_Init();
  Buzzer_Init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  TIMER_NOW();
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  // Настройка системной частоты на основе внешнего генератора HSE
  RCC_OscInitTypeDef RCC_OscInitStruct = {0}; // Включение питания
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0}; // Масштабирование напряжения

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks
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

/* USER CODE BEGIN 4 */

void TIMER_NOW( void ){
	// Массив строк для клавиатуры и переменные для отображения данных
	uint8_t Row[4] = {ROW4, ROW3, ROW2, ROW1}, Key, OldKey, OLED_Keys[4] = {0x30,0x30,0x30,0x30};
	// Сброс и начальная инициализация OLED-дисплея
	  oled_Reset();
	  oled_WriteString("Timer: ", Font_11x18,  White);
	  OLED_TIMER(OLED_Keys);
	  oled_UpdateScreen();

	  // Флаги состояния таймера
	  uint8_t isStarted = 0, isPaused = 0;
	  uint8_t pos = 0; // Позиция для ввода цифр времени
	  int count = 0, isStop = 0; // Счетчик и флаг остановки таймера
	  uint32_t time_s = 0; // Общее время в секундах

	  // Переменные для отображения времени
	  uint8_t sec, ten_sec, min, ten_min;

	  while (1){
		// Если таймер запущен
	    if (isStarted){
	    	if (time_s >= 3600) {
	    		time_s = 3600; // Ограничение времени до 1 часа
	    	}
	    // Цикл обратного отсчета времени
	      while (time_s != 0) {
	        HAL_Delay(25);
	        count++;
	        // Если таймер не на паузе и прошло достаточно времени
	        if (!isPaused && count == 1000 / 28) {
	          time_s--;
	          ten_min = time_s / 600; // Вычисляем десятки минут
	          count = 0;
	          min = (time_s - 600 * ten_min) / 60;
	          ten_sec = (time_s - 600 * ten_min - 60 * min) / 10; // Вычисление десятков секунд
	          sec = (time_s - 600 * ten_min - 60 * min - 10 * ten_sec);

	          // Обновление массива OLED_Keys для отображения на экране
	          OLED_Keys[0] = 48 + sec;
	          OLED_Keys[1] = 48 + ten_sec;
	          OLED_Keys[2] = 48 + min;
	          OLED_Keys[3] = 48 + ten_min;
	          OLED_TIMER(OLED_Keys);

	        }
	        // Обработка нажатий клавиш
	          OldKey = Key;
	          Key = Get_Key( Row[0] );
	          if (Key == 0x0C && OldKey != Key) { // Если нажата клавиша для паузы
	            if (isPaused) {
	              isPaused = 0;
	            } else {
	              isPaused = 1;
	            }
	            count = 0;
	          }
	          if (Key == 0x0B) { // Если нажата клавиша для остановки
	        	  isStop = 1;
	        	  break;
	          }
	      }
	      //Сброс значений
	      isStarted = 0;

	      pos = 0;
	      OLED_Keys[0] = 0x30;
	      OLED_Keys[1] = 0x30;
	      OLED_Keys[2] = 0x30;
	      OLED_Keys[3] = 0x30;
	      OLED_TIMER(OLED_Keys);

	      // Если таймер не остановлен, воспроизведение мелодии
	      if (!isStop) {
	    	  Buzzer_Play(megalovania_melody, megalovania_delays, sizeof(megalovania_melody) / sizeof(uint32_t));
	      }
	      isStop = 0;

	    }
	    // Обработка ввода с клавиатуры для задания времени
	    for ( int i = 0; i < 4; i++ ){
	      OldKey = Key;
	      Key = Get_Key( Row[i] );
	      if (Key != OldKey){ // Если клавиша изменилась
	        if (Key >= 0x00 && Key <= 0x0A && !isStarted){ // Если нажата клавиша для ввода времени
	          OLED_Keys[pos] = 0x30 + Key;
	          if (pos == 0) {
	            time_s=0;
	              OLED_Keys[1] = 0x30;
	              OLED_Keys[2] = 0x30;
	              OLED_Keys[3] = 0x30;

	            time_s += Key;

	            pos++;
	          }else  if (pos == 1) {
	            time_s += 10*Key;

	              pos++;
	          }else if (pos == 2) {
	            time_s += 60*Key;

	              pos++;
	          } else if (pos == 3){
	        	  if (Key > 5) { // Ограничение на ввод десятков минут
	        		  Key = 5;
	        		  OLED_Keys[pos] = 0x30 + Key;

	        	  }
	            time_s +=600* Key;

	            pos = 0;
	          }

	          OLED_TIMER(OLED_Keys); // Обновление дисплея с новыми значениями
	        } else if (Key == 0x0B){  // Если нажата клавиша остановки
	            break;
	        } else if (Key == 0x0C){ // Если нажата клавиша старта/паузы
	          if (!isStarted){
	            isStarted  = 1;
	          } else {
	            if (isPaused){
	              isPaused = 0;
	            } else{
	              isPaused = 1;
	            }
	          }
	        }
	      }
	      HAL_Delay(25);
	    }
	  }
}
// Функция для обновления отображения времени на OLED-дисплее
void OLED_TIMER(uint8_t OLED_Keys[4]){
	oled_SetCursor(20, 20); //установка курсора
	for (int i = 3; i >= 0; i--) { // Отображение значений в обратном порядке
			if (i == 1){
				oled_WriteChar(0x3A, Font_11x18, White); // Отображение символа ':'
			}
			oled_WriteChar(OLED_Keys[i], Font_11x18, White); // Отображение цифры
		}
		oled_UpdateScreen();
}

void KB_Test( void ) {
	UART_Transmit( (uint8_t*)"KB test start\n" );
	uint8_t R = 0, C = 0, L = 0, Row[4] = {ROW4, ROW3, ROW2, ROW1}, Key, OldKey, OLED_Keys[12] = {0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30};
	oled_Reset();
	oled_WriteString("From bottom to top", Font_7x10, White);
	OLED_KB(OLED_Keys);
	oled_UpdateScreen();
	for ( int i = 0; i < 4; i++ ) {
		while( !( R && C && L ) ) {
			OldKey = Key;
			Key = Check_Row( Row[i] );
			if ( Key == 0x01 && Key != OldKey) {
				UART_Transmit( (uint8_t*)"Right pressed\n" );
				R = 1;
				OLED_Keys[2+3*i] = 0x31;
				OLED_KB(OLED_Keys);
			} else if ( Key == 0x02 && Key != OldKey) {
				UART_Transmit( (uint8_t*)"Center pressed\n" );
				C = 1;
				OLED_Keys[1+3*i] = 0x31;
				OLED_KB(OLED_Keys);
			} else if ( Key == 0x04 && Key != OldKey) {
				UART_Transmit( (uint8_t*)"Left pressed\n" );
				L = 1;
				OLED_Keys[3*i] = 0x31;
				OLED_KB(OLED_Keys);
			}
		}
		UART_Transmit( (uint8_t*)"Row complete\n" );
		R = C = L = 0;
		HAL_Delay(25);
	}
	UART_Transmit( (uint8_t*)"KB test complete\n");
}
void oled_Reset( void ) {
	oled_Fill(Black);
	oled_SetCursor(0, 0);
	oled_UpdateScreen();
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
