/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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
#include "rtc.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "stdio.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
RTC_TimeTypeDef sTime;		// estrutura que recebera a hora
RTC_DateTypeDef sDate;		// estrutura que recebera a data


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//--------------------------------------------------- MANDA COMANDO PARA O LCD
	void lcd_comando(uint8_t comando){
	HAL_GPIO_WritePin(GPIOA, RS_Pin,0);  //RS em zero para mandar comando
	GPIOA->BRR = 0xFF; //zera os bits de dados
	GPIOA->BSRR = comando; //seta os bits do comando
	
	HAL_GPIO_WritePin(GPIOA, EN_Pin,1);			//en 1
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOA, EN_Pin,0);			//en 0
	HAL_Delay(1);
	}
	
//--------------------------------------------------- DESLOCA CURSOR LCD
	void lcd_GOTO(int linha, int coluna){
		if (coluna<16){
		if (linha==0)lcd_comando(0x80+coluna);
		if (linha==1)lcd_comando(0xc0+coluna);
		if (linha==2)lcd_comando(0x94+coluna);
		if (linha==3)lcd_comando(0xD4+coluna);
		}
	}
	
	
//--------------------------------------------------- MANDA DADOS PARA ESCREVER NO LCD
	void lcd_dado(char dado){
	HAL_GPIO_WritePin(GPIOA, RS_Pin,1);  //RS em zero para mandar dados
	GPIOA->BRR = 0xFF; //zera os bits de dados
	GPIOA->BSRR = dado; //seta os bits do dado
	
	HAL_GPIO_WritePin(GPIOA, EN_Pin,1);			//en 1
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOA, EN_Pin,0);			//en 0
	HAL_Delay(1);
	}
	

//--------------------------------------------------- ESCREVE STRINGS NO LCD
	void lcd_STRING(char vetor[]){
		int tamanho, i=0;
		tamanho = strlen(vetor);
		for(i=0;i<=tamanho;i++){
			lcd_dado(vetor[i]);
		}
	}
	

//--------------------------------------------------- INICIALIZA�AO DO LCD	
	void lcd_init(void){
		lcd_comando(0x38);
		lcd_comando(0x0e);
		lcd_comando(0x06);
		lcd_comando(0x01);
	  HAL_Delay(100);
	}
	
	

//--------------------------------------------------- L� AS TECLAS DO TECLADO
	int le_teclado(void){
	HAL_Delay(50);
	int i=0;
	for (i=0; i<3;i++){
	if (i==0){
	// ------------ coluna 1 numeros 1, 6, 7 e *
	HAL_GPIO_WritePin(GPIOB,Col1_Pin,1);
	HAL_GPIO_WritePin(GPIOB,Col2_Pin,0);
	HAL_GPIO_WritePin(GPIOB,Col3_Pin,0);
		
	if(HAL_GPIO_ReadPin(GPIOB,Lin1_Pin)) return 1;
	else if(HAL_GPIO_ReadPin(GPIOB,Lin2_Pin)) return 6;
	else if(HAL_GPIO_ReadPin(GPIOB,Lin3_Pin)) return 7;
	else if(HAL_GPIO_ReadPin(GPIOB,Lin4_Pin)) return 77;
	//else return -1;
	}
	else if (i==1){
	// ------------ coluna 2 numeros 2, 5, 8 e 0
	HAL_Delay(50);
	HAL_GPIO_WritePin(GPIOB,Col1_Pin,0);
	HAL_GPIO_WritePin(GPIOB,Col2_Pin,1);
	HAL_GPIO_WritePin(GPIOB,Col3_Pin,0);
		
	if(HAL_GPIO_ReadPin(GPIOB,Lin1_Pin)) return 2;
	else if(HAL_GPIO_ReadPin(GPIOB,Lin2_Pin)) return 5;
	else if(HAL_GPIO_ReadPin(GPIOB,Lin3_Pin)) return 8;
	else if(HAL_GPIO_ReadPin(GPIOB,Lin4_Pin)) return 10;
	//else return -1;
	}
	else if (i==2){
	// ------------ coluna 1 numeros 3, 4, 9 e #
	HAL_Delay(50);
	HAL_GPIO_WritePin(GPIOB,Col1_Pin,0);
	HAL_GPIO_WritePin(GPIOB,Col2_Pin,0);
	HAL_GPIO_WritePin(GPIOB,Col3_Pin,1);
		
	if(HAL_GPIO_ReadPin(GPIOB,Lin1_Pin)) return 3;
	else if(HAL_GPIO_ReadPin(GPIOB,Lin2_Pin)) return 4;
	else if(HAL_GPIO_ReadPin(GPIOB,Lin3_Pin)) return 9;
	else if(HAL_GPIO_ReadPin(GPIOB,Lin4_Pin)) return 99;
	//else return -1;
	}
}i=0;
}

//--------------------------------------------------- APAGA TUDO NO DISLAY
	void lcd_clear(){
	int i=0, j=0;
		for (i=0; i<4;i++){
			for (j=0; j<16;j++){
				lcd_GOTO(i,j);
				lcd_dado(' ');
			}
		}
	}
	
//--------------------------------------------------- TESTA AS TECLAS DO TECLADO
	void testa_teclado (){
		int tecla =0;
		
		lcd_clear();
		lcd_GOTO(0,1);
		lcd_STRING("Teste teclado!");
		
		while (tecla != 99){
		tecla = le_teclado();
		if (tecla == 1){
		lcd_clear();
			lcd_GOTO(1,1);
			lcd_STRING("Tecla 1");
		}
		else if (tecla == 2){
		lcd_clear();
			lcd_GOTO(1,1);
			lcd_STRING("Tecla 2");
		}
		else if (tecla == 3){
		lcd_clear();
			lcd_GOTO(1,1);
			lcd_STRING("Tecla 3");
		}
		else if (tecla == 4){
			lcd_clear();
			lcd_GOTO(1,1);
			lcd_STRING("Tecla 4");
		}
		else if (tecla == 5){
		lcd_clear();
			lcd_GOTO(1,1);
			lcd_STRING("Tecla 5");
		}
		else if (tecla == 6){
		lcd_clear();
			lcd_GOTO(1,1);
			lcd_STRING("Tecla 6");
		}
		else if (tecla == 7){
			lcd_clear();
			lcd_GOTO(1,1);
			lcd_STRING("Tecla 7");
		}
		else if (tecla == 8){
		lcd_clear();
			lcd_GOTO(1,1);
			lcd_STRING("Tecla 8");
		}
		else if (tecla == 9){
		lcd_clear();
			lcd_GOTO(1,1);
			lcd_STRING("Tecla 9");
		}
		else if (tecla == 10){
			lcd_clear();
			lcd_GOTO(1,1);
			lcd_STRING("Tecla 0");
		}
		else if (tecla == 77){
		lcd_clear();
			lcd_GOTO(1,1);
			lcd_STRING("Tecla *");
		}
		else if (tecla == 99){
		lcd_clear();
			lcd_GOTO(1,1);
			lcd_STRING("Tecla #");
		}
	}
		lcd_clear();
	}

//--------------------------------------------------- ESCREVE A HORA NA TELA
	void escreve_hora (){
	char vetor_hora[30];
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
	sprintf(vetor_hora,"%02d:%02d:%02d",sTime.Hours,sTime.Minutes,sTime.Seconds);
	lcd_GOTO(2,1);
	lcd_STRING(vetor_hora);		
	}
	
	//--------------------------------------------------- SETA HORA
	void set_hora(){
	int tecla =0, i=0, hora_vet[6] ={66, 66, 66, 66, 66, 66};
	float ponto =1;
	lcd_clear();
	lcd_GOTO(0,1);
	lcd_STRING("Configura Hora");
	
	lcd_GOTO(1,4);
	lcd_STRING("00:00:00");
	
	for (i=5; i>-1;i--){
	while (hora_vet[i] == 66){
	tecla = le_teclado();
	lcd_GOTO(1,11-i- 2*ponto);
	switch (tecla){
		case 10:
	  lcd_STRING("0");
		hora_vet[i] = 0;
		break;
		case 1:
	  lcd_STRING("1");
		hora_vet[i]= 1;
		break;
		case 2:
	  lcd_STRING("2");
		hora_vet[i]= 2;
		break;
		case 3:
	  lcd_STRING("3");
		hora_vet[i]= 3;
		break;
		case 4:
	  lcd_STRING("4");
		hora_vet[i]= 4;
		break;
		case 5:
	  lcd_STRING("5");
		hora_vet[i] = 5;
		break;
		case 6:
	  lcd_STRING("6");
		hora_vet[i]= 6;
		break;
		case 7:
	  lcd_STRING("7");
		hora_vet[i]= 7;
		break;
		case 8:
	  lcd_STRING("8");
		hora_vet[i]= 8;
		break;
		case 9:
	  lcd_STRING("9");
		hora_vet[i]= 9;
		break;	
	}
}

	ponto = ponto-0.25;
}
	
	if(hora_vet[5]*10 + hora_vet[4] >23) 	sTime.Hours = 23;
	else 	sTime.Hours = hora_vet[5]*10 + hora_vet[4];
 
	if(hora_vet[3]*10 + hora_vet[2] >60) 	sTime.Minutes = 00;
	else 	sTime.Minutes = hora_vet[3]*10 + hora_vet[2];

	if(hora_vet[1]*10 + hora_vet[0] >60) 	sTime.Seconds = 0;
	else 	sTime.Seconds = hora_vet[1]*10 + hora_vet[0];

	HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	
	lcd_clear();

}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	
	char vetor[30], vetor_hora[30],vetor_data[30];
	int umidade = 10, tecla =0, i=1;
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
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
	
		lcd_init();
		set_hora();
	
	
//	sTime.Hours = 23;
//	sTime.Minutes = 23;
////	sTime.Seconds = 23;
//	HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	sDate.Date = 29;
	sDate.Month = 8;
	sDate.WeekDay = 6; 
	sDate.Year = 20;
	HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		
	//	testa_teclado();

		escreve_hora();
		
		
		

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

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
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