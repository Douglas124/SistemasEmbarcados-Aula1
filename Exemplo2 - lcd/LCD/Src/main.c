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
#include "adc.h"
#include "rtc.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "stdio.h"

#define SDA (1<<10) //PA.10 - PLACA stm32f103
#define SCL (1<<11) //PA.11 - PLACA stm32f103
#define SDA0 GPIOA->BSRR = 1<<(10+16)    // bit set/reset register 32 bit- 16 primeiros set, 16 ultimos reset: [0000010000000000 0000000000000000]
#define SDA1 GPIOA->BSRR = SDA					 // bit set/reset register 32 bit- 16 primeiros set, 16 ultimos reset: [0000000000000000 0000010000000000]
#define SCL0 GPIOA->BSRR = 1<<(11+16)
#define SCL1 GPIOA->BSRR = SCL


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

//--------------------------------------------------- I2C (diferente) START
void start_i2c(void){
SCL0;
SDA1;
HAL_Delay(1);//1 milisegundo
SCL1;				// 																				 |--- start---|
HAL_Delay(1);//1 milisegundo							SCL (SCK)  ____|--|_|--|_____
SDA0;				//														SDA (DATA) -----|_____|-----|___
HAL_Delay(1);//1 milisegundo
SCL0;
HAL_Delay(1);//1 milisegundo
SCL1;
HAL_Delay(1);//1 milisegundo
SDA1;
HAL_Delay(1);//1 milisegundo
SCL0;
HAL_Delay(1);//1 milisegundo
SDA0;
	
}

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
	

//--------------------------------------------------- INICIALIZAÇAO DO LCD	
	void lcd_init(void){
		lcd_comando(0x38);
		lcd_comando(0x0e);
		lcd_comando(0x06);
		lcd_comando(0x01);
	  HAL_Delay(100);
	}
	
	

//--------------------------------------------------- LÊ AS TECLAS DO TECLADO
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
	lcd_GOTO(1,4);
	lcd_STRING(vetor_hora);		
	}
	
//--------------------------------------------------- SETA HORA
	void set_hora(){
	int tecla =0, i=0,var_OK=0, hora_vet[6] ={66, 66, 66, 66, 66, 66};
	float ponto =1;
	lcd_GOTO(0,1);
	lcd_STRING("Configura Hora");
	
	lcd_GOTO(1,4);
	lcd_STRING("00:00:00");
	lcd_GOTO(3,0);
	lcd_STRING("*: ESC     #: OK");
	
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
	
		while (var_OK != 66){
		tecla = le_teclado();
		if (tecla == 77){
			lcd_GOTO(3,0);
			lcd_STRING("   CANCELADA!   ");
			var_OK = 66;
		}
			else if (tecla == 99){
			if(hora_vet[5]*10 + hora_vet[4] >23) 	sTime.Hours = 23;
			else 	sTime.Hours = hora_vet[5]*10 + hora_vet[4];
		 
			if(hora_vet[3]*10 + hora_vet[2] >60) 	sTime.Minutes = 00;
			else 	sTime.Minutes = hora_vet[3]*10 + hora_vet[2];

			if(hora_vet[1]*10 + hora_vet[0] >60) 	sTime.Seconds = 0;
			else 	sTime.Seconds = hora_vet[1]*10 + hora_vet[0];

			HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
			lcd_GOTO(3,0);
			lcd_STRING("  CONFIG SALVA! ");
			var_OK = 66;
			}
	}
		


	
	lcd_clear();

}
	
//--------------------------------------------------- Envia 1 pelo I2C
void envia_1_i2c(void){
SDA1;
HAL_Delay(1);//1 milisegundo
SCL1;
HAL_Delay(1);//1 milisegundo
SCL0;
HAL_Delay(1);//1 milisegundo
}

//--------------------------------------------------- Envia 1 pelo I2C
void envia_0_i2c(void){
SDA0;
HAL_Delay(1);//1 milisegundo
SCL1;
HAL_Delay(1);//1 milisegundo
SCL0;
HAL_Delay(1);//1 milisegundo
}

//--------------------------------------------------- ACK i2c
int ack_i2c(void){
int x;																// muda a config do pino de saida para entrada para poder ler o ack depois muda para sada de novo
GPIO_InitTypeDef GPIO_InitStruct;
GPIO_InitStruct.Pin = GPIO_PIN_10; // SDA => PA.10
GPIO_InitStruct.Mode = GPIO_MODE_INPUT; //FAZ SDA COMO ENTRADA
GPIO_InitStruct.Pull = GPIO_PULLUP;
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
SCL1;
HAL_Delay(1);//1 milisegundo
x = HAL_GPIO_ReadPin(GPIOA,SDA); //L^E O PINO
SCL0;
HAL_Delay(1);//1 milisegundo
GPIO_InitStruct.Pin = GPIO_PIN_10; // SDA => PA.10
GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; //FAZ SDA COMO SA´IDA
GPIO_InitStruct.Pull = GPIO_PULLUP;
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
return x; //se 0 ok, se 1 erro
}

//--------------------------------------------------- LE UM BYTE
int8_t le_byte(void){
	uint8_t x=0;																// muda a config do pino de saida para entrada para poder ler o ack depois muda para sada de novo
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = GPIO_PIN_10; // SDA => PA.10
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT; //FAZ SDA COMO ENTRADA
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	
	for (int i=0; i<8; i++){
	SCL1;
	HAL_Delay(1);//1 milisegundo
	x = x<<1;
	x |= HAL_GPIO_ReadPin(GPIOA,SDA); //L^E O PINO
	SCL0;
	HAL_Delay(1);//1 milisegundo
	}
	

	GPIO_InitStruct.Pin = GPIO_PIN_10; // SDA => PA.10
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; //FAZ SDA COMO SA´IDA
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	return x; 

}

//--------------------------------------------------- LEITURA DE UMIDADE
int le_umi(void){
	int erro;
	float umi;
	uint8_t byte1, byte2, byte3;
	uint16_t umidade;
	start_i2c();								
	envia_0_i2c();
	envia_0_i2c();
	envia_0_i2c();
	envia_0_i2c();
	envia_0_i2c();
	envia_1_i2c();
	envia_0_i2c();
	envia_1_i2c();
	erro = ack_i2c();
	HAL_Delay(100);
	byte1 = le_byte();
	envia_0_i2c ();      //ack do mestre ara o escravo
	byte2 = le_byte();
	envia_0_i2c ();      //ack do mestre ara o escravo
	byte3 = le_byte();
	envia_1_i2c ();      //ack do mestre ara o escravo
	
	umidade = ((byte1 & 0x0f) << 8) | byte2;
	
	umi = (-2.0468) + (0.0367)*(umidade) + (-0.0000015855)*(umidade*umidade);
	
	return (int)umi;
}
//--------------------------------------------------- LEITURA DE TEMPERATURA
int le_temp(void){
	int erro;
	float temp;
	uint8_t byte1, byte2, byte3;
	uint16_t temperatura;
	start_i2c();								
	envia_0_i2c();
	envia_0_i2c();
	envia_0_i2c();
	envia_0_i2c();
	envia_0_i2c();
	envia_0_i2c();
	envia_1_i2c();
	envia_1_i2c();
	erro = ack_i2c();
	HAL_Delay(360);
	byte1 = le_byte();
	envia_0_i2c ();      //ack do mestre ara o escravo
	byte2 = le_byte();
	envia_0_i2c ();      //ack do mestre ara o escravo
	byte3 = le_byte();
	envia_1_i2c ();      //ack do mestre ara o escravo
	
	temperatura = ((byte1 & 0x3f) << 8) | byte2;
	
	temp = (-39.6)+(0.01*temperatura);
	
	return (int)temp;
}

//--------------------------------------------------- LEITURA DE LUMINOSIDADE
uint16_t le_lumi(void){
	uint16_t leitura_AD;
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1,100);
	leitura_AD = HAL_ADC_GetValue(&hadc1); //LEITURA DO CANAL 5
	HAL_ADC_Stop(&hadc1);
	return leitura_AD;
}

//--------------------------------------------------- ESCREVE UMIDADE NA TELA
void escreve_umi(void){
	char vetor_umidade[30];
	int umidade =0;
	umidade = le_umi();
	sprintf(vetor_umidade,"U: %2d %%",umidade);
	lcd_GOTO(0,1);
	lcd_STRING(vetor_umidade);			
}

//--------------------------------------------------- ESCREVE TEMPERATURA NA TELA
void escreve_tmp(void){
	char vetor_temperatura[30];
	int temperatura =0;
	temperatura = le_temp();
	sprintf(vetor_temperatura,"T: %2doC",temperatura);
	lcd_GOTO(0,9);
	lcd_STRING(vetor_temperatura);	
}

//--------------------------------------------------- ESCREVE LUMINOSIDADE NA TELA
void escreve_lumi(void){
	uint16_t leitura_luminosidade;
	leitura_luminosidade = le_lumi();
	char vetor_luminosidade[30];
	sprintf(vetor_luminosidade,"L: %04d lx",leitura_luminosidade);
	lcd_GOTO(2,3);
	lcd_STRING(vetor_luminosidade);	
}

//--------------------------------------------------- CONFIGURA RELE1 CONFIG HORA
int set_parametros (int le_escreve, int par, long int valor){
	static long int parametros [6] = {80005,   // liga rele1
																		80010,		// desliga rele1
																		80010,		// liga rele2
																		80015,		// desliga rele2
																		25,				// sp temperatura rele3
																		1000};		// sp luminosidade rele4
	if (le_escreve == 1){   // 1 - leitura   2- escrita
		return parametros [par];
	}
		else if (le_escreve == 2){
		parametros[par] = valor;
		}
}

//--------------------------------------------------- ESCREVE PARAMETROS NA TELA
void menu_parametros (void){
	long int  liga_RL1 =0,var_OK=0,tecla=0, desliga_RL1 =0, liga_RL2 =0, desliga_RL2=0;
	int  temp_liga=0;
	uint16_t lumi_liga=0;
	char SP_par[16];
	
	liga_RL1 = set_parametros(1,0,0);
	desliga_RL1 = set_parametros(1,1,0);
	liga_RL2 = set_parametros(1,2,0);
	desliga_RL2 = set_parametros(1,3,0);
	temp_liga = set_parametros(1,4,0);
	lumi_liga = set_parametros(1,5,0);
	
	lcd_GOTO(0,0);
	lcd_STRING("RL1          RL2");
	sprintf(SP_par,"L:%ld  %ld",liga_RL1,liga_RL2);
	lcd_GOTO(1,0);
	lcd_STRING(SP_par);
	
	sprintf(SP_par,"D:%ld  %ld",desliga_RL1,desliga_RL2);
	lcd_GOTO(2,0);
	lcd_STRING(SP_par);
	
	sprintf(SP_par,"RL3:%d  RL4:%d",temp_liga,lumi_liga);
	lcd_GOTO(3,0);
	lcd_STRING(SP_par);
	
	while (var_OK != 66){
		tecla = le_teclado();
		if (tecla == 77){
			var_OK = 66;
		}
			else if (tecla == 99){
			var_OK = 66;
			}
	}

	lcd_clear();
	
	

}


//--------------------------------------------------- CONFIGURA RELE1 CONFIG HORA
void config_RL1 (){
	int tecla =0, i=0,j=0,var_OK=0, hora_vet[6] ={66, 66, 66, 66, 66, 66}, aux_hr_liga[3]={0,0,0}, aux_hr_desl[3]={0,0,0};
	long int set_liga=0, set_desliga=0;
	float ponto =1;
	char vetor_hora_liga[30],vetor_hora_desliga[30];
	set_liga = set_parametros(1,0,0);
	set_desliga = set_parametros(1,1,0);
	for(j=0; j<3;j++){
	aux_hr_liga[j] = set_liga%100;
	set_liga /= 100;
	}
	for(j=0; j<3;j++){
	aux_hr_desl[j] = set_desliga%100;
	set_desliga /= 100;
	}
	
	lcd_GOTO(0,2);
	lcd_STRING("Config Rele 1");
	lcd_GOTO(1,0);
	sprintf(vetor_hora_liga,"Liga: %02d:%02d:%02d",aux_hr_liga[2],aux_hr_liga[1],aux_hr_liga[0]);
	lcd_STRING(vetor_hora_liga);
	lcd_GOTO(2,0);
	sprintf(vetor_hora_desliga,"Desl: %02d:%02d:%02d",aux_hr_desl[2],aux_hr_desl[1],aux_hr_desl[0]);
	lcd_STRING(vetor_hora_desliga);
	lcd_GOTO(3,0);
	lcd_STRING("*: ESC     #: OK");
	
	for(j=1; j<3;j++){
		for (i=5; i>-1;i--){
			while (hora_vet[i] == 66){
			tecla = le_teclado();
			lcd_GOTO(j,13-i- 2*ponto);
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
		if (j==1){
			if(hora_vet[5]*10 + hora_vet[4] >23) 	set_liga = 230000;
			else 	set_liga = hora_vet[5]*100000 + hora_vet[4]*10000;
	 
			if(hora_vet[3]*10 + hora_vet[2] >60) 	set_liga = set_liga + 5900;
			else 	set_liga = set_liga + hora_vet[3]*1000 + hora_vet[2]*100;

			if(hora_vet[1]*10 + hora_vet[0] >60) 	set_liga = set_liga + 59;
			else 	set_liga = set_liga +  hora_vet[1]*10 + hora_vet[0];
			
			hora_vet [0]= 66;
			hora_vet [1]= 66;
			hora_vet [2]= 66;
			hora_vet [3]= 66;
			hora_vet [4]= 66;
			hora_vet [5]= 66;
			ponto = 1;
		}
		else if (j==2){
			if(hora_vet[5]*10 + hora_vet[4] >23) 	set_desliga = 230000;
			else 	set_desliga = hora_vet[5]*100000 + hora_vet[4]*10000;
	 
			if(hora_vet[3]*10 + hora_vet[2] >60) 	set_desliga = set_desliga + 5900;
			else 	set_desliga = set_desliga + hora_vet[3]*1000 + hora_vet[2]*100;

			if(hora_vet[1]*10 + hora_vet[0] >60) 	set_desliga = set_desliga + 59;
			else 	set_desliga = set_desliga +  hora_vet[1]*10 + hora_vet[0];
		}
	}
	
			while (var_OK != 66){
		tecla = le_teclado();
		if (tecla == 77){
			lcd_GOTO(3,0);
			lcd_STRING("   CANCELADA!   ");
			var_OK = 66;
		}
			else if (tecla == 99){
			set_parametros(2,0,set_liga);
			set_parametros(2,1,set_desliga);
			lcd_GOTO(3,0);
			lcd_STRING("  CONFIG SALVA! ");
			var_OK = 66;
			}
	}
		

	lcd_clear();

	
}


//--------------------------------------------------- CONFIGURA RELE2 CONFIG HORA
void config_RL2 (){
	int tecla =0, i=0,j=0,var_OK=0, hora_vet[6] ={66, 66, 66, 66, 66, 66}, aux_hr_liga[3]={0,0,0}, aux_hr_desl[3]={0,0,0};
	long int set_liga=0, set_desliga=0;
	float ponto =1;
	char vetor_hora_liga[30],vetor_hora_desliga[30];
	set_liga = set_parametros(1,2,0);
	set_desliga = set_parametros(1,3,0);
	for(j=0; j<3;j++){
	aux_hr_liga[j] = set_liga%100;
	set_liga /= 100;
	}
	for(j=0; j<3;j++){
	aux_hr_desl[j] = set_desliga%100;
	set_desliga /= 100;
	}
	
	lcd_GOTO(0,2);
	lcd_STRING("Config Rele 2");
	lcd_GOTO(1,0);
	sprintf(vetor_hora_liga,"Liga: %02d:%02d:%02d",aux_hr_liga[2],aux_hr_liga[1],aux_hr_liga[0]);
	lcd_STRING(vetor_hora_liga);
	lcd_GOTO(2,0);
	sprintf(vetor_hora_desliga,"Desl: %02d:%02d:%02d",aux_hr_desl[2],aux_hr_desl[1],aux_hr_desl[0]);
	lcd_STRING(vetor_hora_desliga);
	lcd_GOTO(3,0);
	lcd_STRING("*: ESC     #: OK");
	
	for(j=1; j<3;j++){
		for (i=5; i>-1;i--){
			while (hora_vet[i] == 66){
			tecla = le_teclado();
			lcd_GOTO(j,13-i- 2*ponto);
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
		if (j==1){
			if(hora_vet[5]*10 + hora_vet[4] >23) 	set_liga = 230000;
			else 	set_liga = hora_vet[5]*100000 + hora_vet[4]*10000;
	 
			if(hora_vet[3]*10 + hora_vet[2] >60) 	set_liga = set_liga + 5900;
			else 	set_liga = set_liga + hora_vet[3]*1000 + hora_vet[2]*100;

			if(hora_vet[1]*10 + hora_vet[0] >60) 	set_liga = set_liga + 59;
			else 	set_liga = set_liga +  hora_vet[1]*10 + hora_vet[0];
			
			hora_vet [0]= 66;
			hora_vet [1]= 66;
			hora_vet [2]= 66;
			hora_vet [3]= 66;
			hora_vet [4]= 66;
			hora_vet [5]= 66;
			ponto = 1;
		}
		else if (j==2){
			if(hora_vet[5]*10 + hora_vet[4] >23) 	set_desliga = 230000;
			else 	set_desliga = hora_vet[5]*100000 + hora_vet[4]*10000;
	 
			if(hora_vet[3]*10 + hora_vet[2] >60) 	set_desliga = set_desliga + 5900;
			else 	set_desliga = set_desliga + hora_vet[3]*1000 + hora_vet[2]*100;

			if(hora_vet[1]*10 + hora_vet[0] >60) 	set_desliga = set_desliga + 59;
			else 	set_desliga = set_desliga +  hora_vet[1]*10 + hora_vet[0];
		}
	}
	
		while (var_OK != 66){
		tecla = le_teclado();
		if (tecla == 77){
			lcd_GOTO(2,2);
			lcd_STRING("CANCELADA!");
			var_OK = 66;
		}
			else if (tecla == 99){
			set_parametros(2,2,set_liga);
			set_parametros(2,3,set_desliga);
			lcd_GOTO(3,0);
			lcd_STRING("  CONFIG SALVA! ");
			var_OK = 66;
			}
	}
		

	lcd_clear();
	
}


//--------------------------------------------------- CONFIGURA RELE3 ACIONA COM TEMP
void config_RL3 (){
	int tecla =0, i=0, sp_temp =0, var_OK=0, temp_vet[2] ={66, 66};
	char vetor_temp[10];
	lcd_GOTO(0,2);
	lcd_STRING("Config Rele 3");
	sp_temp = set_parametros(1,4,0);
	sprintf(vetor_temp,"SP de temp: %doC",sp_temp);
	lcd_GOTO(1,0);
	lcd_STRING(vetor_temp);
	lcd_GOTO(3,0);
	lcd_STRING("*: ESC     #: OK");
	
	for (i=1; i>-1;i--){
		while (temp_vet[i] == 66){
		tecla = le_teclado();
		lcd_GOTO(1,13-i);
			
		switch (tecla){
			case 10:
			lcd_STRING("0");
			temp_vet[i] = 0;
			break;
			case 1:
			lcd_STRING("1");
			temp_vet[i]= 1;
			break;
			case 2:
			lcd_STRING("2");
			temp_vet[i]= 2;
			break;
			case 3:
			lcd_STRING("3");
			temp_vet[i]= 3;
			break;
			case 4:
			lcd_STRING("4");
			temp_vet[i]= 4;
			break;
			case 5:
			lcd_STRING("5");
			temp_vet[i] = 5;
			break;
			case 6:
			lcd_STRING("6");
			temp_vet[i]= 6;
			break;
			case 7:
			lcd_STRING("7");
			temp_vet[i]= 7;
			break;
			case 8:
			lcd_STRING("8");
			temp_vet[i]= 8;
			break;
			case 9:
			lcd_STRING("9");
			temp_vet[i]= 9;
			break;	
			}
		}

	}
	
	while (var_OK != 66){
		tecla = le_teclado();
		if (tecla == 77){
			lcd_GOTO(2,2);
			lcd_STRING("CANCELADA!");
			var_OK = 66;
		}
			else if (tecla == 99){
			sp_temp =temp_vet[1]*10 + temp_vet[0];
			set_parametros(2,4,sp_temp);
			lcd_GOTO(2,1);
			lcd_STRING("CONFIG SALVA!");
			var_OK = 66;
			}
	}

	lcd_clear();

}


//--------------------------------------------------- CONFIGURA RELE4 ACIONA COM LUMI
void config_RL4 (){
	int tecla =0, i=0, sp_lumi =0, var_OK=0, lumi_vet[4] ={66, 66, 66, 66};
	char vetor_lumi[10];
	lcd_GOTO(0,2);
	lcd_STRING("Config Rele 4");
	sp_lumi = set_parametros(1,5,0);
	sprintf(vetor_lumi,"SP de lumi: %d",sp_lumi);
	lcd_GOTO(1,0);
	lcd_STRING(vetor_lumi);
	lcd_GOTO(3,0);
	lcd_STRING("*: ESC     #: OK");
	
	for (i=3; i>-1;i--){
		while (lumi_vet[i] == 66){
		tecla = le_teclado();
		lcd_GOTO(1,15-i);
			
		switch (tecla){
			case 10:
			lcd_STRING("0");
			lumi_vet[i] = 0;
			break;
			case 1:
			lcd_STRING("1");
			lumi_vet[i]= 1;
			break;
			case 2:
			lcd_STRING("2");
			lumi_vet[i]= 2;
			break;
			case 3:
			lcd_STRING("3");
			lumi_vet[i]= 3;
			break;
			case 4:
			lcd_STRING("4");
			lumi_vet[i]= 4;
			break;
			case 5:
			lcd_STRING("5");
			lumi_vet[i] = 5;
			break;
			case 6:
			lcd_STRING("6");
			lumi_vet[i]= 6;
			break;
			case 7:
			lcd_STRING("7");
			lumi_vet[i]= 7;
			break;
			case 8:
			lcd_STRING("8");
			lumi_vet[i]= 8;
			break;
			case 9:
			lcd_STRING("9");
			lumi_vet[i]= 9;
			break;	
			}
		}

	}
	
		while (var_OK != 66){
		tecla = le_teclado();
		if (tecla == 77){
			lcd_GOTO(2,2);
			lcd_STRING("CANCELADA!");
			var_OK = 66;
		}
			else if (tecla == 99){
			sp_lumi =lumi_vet[3]*1000 +lumi_vet[2]*100 + lumi_vet[1]*10 + lumi_vet[0];
			set_parametros(2,5,sp_lumi);
//			sprintf(vetor_lumi,"SP de lumi: %d",sp_lumi);
			lcd_GOTO(2,1);
			lcd_STRING("CONFIG SALVA!");
			var_OK = 66;
			}
	}

	lcd_clear();
}


//--------------------------------------------------- MENU RELES
void menu_rele(void){
int tecla =0, var_escolha = 0, var_OK = 0, var_sair = 0;
	char vetor_escolha[2];
	lcd_GOTO(0,2);
	lcd_STRING("MENU RELES");	
	lcd_GOTO(1,0);
	lcd_STRING("1- RL1   2- RL2");	
	lcd_GOTO(2,0);
	lcd_STRING("3- RL3   4- RL4");		
	lcd_GOTO(3,4);
	lcd_STRING("*:ESC  #:OK");	
	
	while (var_OK != 66){
	tecla = le_teclado();
		if (tecla == 77){
			var_OK = 66;
		}
			else if (tecla != 99){
				if (tecla != 0)	var_escolha = tecla;
				sprintf(vetor_escolha,"%d",var_escolha);
				lcd_GOTO(3,0);
				lcd_STRING(vetor_escolha);
			}
				else	var_OK = 66;
	}
	
	lcd_clear();
	if (var_escolha == 1) config_RL1();
	else if (var_escolha == 2) config_RL2();
	else if (var_escolha == 3) config_RL3();
	else if (var_escolha == 4) config_RL4();
	
}

//--------------------------------------------------- MENU ESCOLHAS
void menu_1(void){
	int tecla =0, var_escolha = 0, var_OK = 0, var_sair = 0;
	char vetor_escolha[2];
	lcd_GOTO(0,5);
	lcd_STRING("MENU");	
	lcd_GOTO(1,0);
	lcd_STRING("1 - Ajusta Hora");	
	lcd_GOTO(2,0);
	lcd_STRING("2 - Config Reles");		
	lcd_GOTO(3,4);
	lcd_STRING("*:ESC  #:OK");	
	
	while (var_OK != 66){
	tecla = le_teclado();
		if (tecla == 77){
			var_OK = 66;
		}
			else if (tecla != 99){
				if (tecla != 0)	var_escolha = tecla;
				sprintf(vetor_escolha,"%d",var_escolha);
				lcd_GOTO(3,0);
				lcd_STRING(vetor_escolha);
			}
				else	var_OK = 66;
	}
	
	lcd_clear();
	if (var_escolha == 1) set_hora();
	else if (var_escolha == 2) menu_rele();
	else if (var_escolha == 9) menu_parametros();
	
}

//--------------------------------------------------- MENU PRINCIPAL
void menu_principal(void){
	int tecla =0;
	lcd_GOTO(3,1);
	lcd_STRING("#: Menu");	
	tecla = le_teclado();
	if (tecla == 99){
	lcd_clear();
	menu_1();
	}
}

//--------------------------------------------------- ACIONAMENTOS 
void acionamentos(void){
	long int hora_atual=0, liga_RL1 =0, desliga_RL1 =0, liga_RL2 =0, desliga_RL2=0;
	int temp_atual =0, temp_liga=0;
	uint16_t lumi_atual=0, lumi_liga=0;
//	char horas [30];
	
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
	
	hora_atual = sTime.Hours*10000 + sTime.Minutes*100 + sTime.Seconds;
//  sprintf(horas,"%ld",hora_atual);
//	lcd_GOTO(0,2);
//	lcd_STRING(horas);
	temp_atual = le_temp();
	lumi_atual = le_lumi();
	
	liga_RL1 = set_parametros(1,0,0);
	desliga_RL1 = set_parametros(1,1,0);
	liga_RL2 = set_parametros(1,2,0);
	desliga_RL2 = set_parametros(1,3,0);
	temp_liga = set_parametros(1,4,0);
	lumi_liga = set_parametros(1,5,0);
	
	if ((liga_RL1 > hora_atual) && (liga_RL1 < hora_atual+3)){
		HAL_GPIO_WritePin(GPIOB, RELE1_Pin,1);			
		HAL_Delay(1);
	}
	if ((liga_RL2 > hora_atual) && (liga_RL2 < hora_atual+3)){
		HAL_GPIO_WritePin(GPIOB, RELE2_Pin,1);		
		HAL_Delay(1);
	}
	if ((desliga_RL1 > hora_atual) && (desliga_RL1 < hora_atual+3)){
		HAL_GPIO_WritePin(GPIOB, RELE1_Pin,0);			
		HAL_Delay(1);
	}
	if ((desliga_RL2 > hora_atual) && (desliga_RL2 < hora_atual+3)){
		HAL_GPIO_WritePin(GPIOB, RELE2_Pin,0);			
		HAL_Delay(1);
	}


	if(temp_atual > temp_liga){
		HAL_GPIO_WritePin(GPIOB, RELE3_Pin,1);			
		HAL_Delay(1);
	}
	else {
		HAL_GPIO_WritePin(GPIOB, RELE3_Pin,0);			
		HAL_Delay(1);
	}
	
	
	if(lumi_atual > lumi_liga){
		HAL_GPIO_WritePin(GPIOB, RELE4_Pin,1);			
		HAL_Delay(1);
	}
	else {
		HAL_GPIO_WritePin(GPIOB, RELE4_Pin,0);			
		HAL_Delay(1);
	}
	
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
	int tecla =0, i=1;
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
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
	

		lcd_init();
//  set_hora();
	
	
	sTime.Hours = 0;
	sTime.Minutes = 0;
	sTime.Seconds = 0;
	HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
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
		escreve_umi();
		escreve_tmp();
		escreve_hora();
		escreve_lumi();
		menu_principal();
		acionamentos();
				

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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_ADC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV2;
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
