/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "cJSON.h"
#include "string.h"
#include "stdio.h"
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
SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//chan LCD
#define RS_Pin GPIO_PIN_10
#define RS_GPIO_Port GPIOA
#define RW_Pin GPIO_PIN_11
#define RW_GPIO_Port GPIOA
#define EN_Pin GPIO_PIN_12
#define EN_GPIO_Port GPIOA
#define D0_Pin GPIO_PIN_15
#define D0_GPIO_Port GPIOA
#define D1_Pin GPIO_PIN_3
#define D1_GPIO_Port GPIOB
#define D2_Pin GPIO_PIN_4
#define D2_GPIO_Port GPIOB
#define D3_Pin GPIO_PIN_5
#define D3_GPIO_Port GPIOB
#define D4_Pin GPIO_PIN_6
#define D4_GPIO_Port GPIOB
#define D5_Pin GPIO_PIN_7
#define D5_GPIO_Port GPIOB
#define D6_Pin GPIO_PIN_8
#define D6_GPIO_Port GPIOB
#define D7_Pin GPIO_PIN_9
#define D7_GPIO_Port GPIOB
#define DHT11_PORT GPIOA
#define DHT11_PIN GPIO_PIN_8

//bien cua dht11
uint8_t Rh_byte1, Rh_byte2, Temp_byte1, Temp_byte2;
float RH, TEMP;
uint8_t Presence = 0;
uint16_t SUM;
//bien xu ly UART
char mang[100];
char mang3[1000];
char mang2[20];
char Rx_buff[100];
uint8_t Rx_data;
uint8_t Rx_index=0;
//bien trang thai led


cJSON *str_json;

// ham xu ly tin hieu DHT11
void delay (uint16_t time)
{
	/* change your code here for the delay in microseconds */
	__HAL_TIM_SetCounter(&htim3,0);
	while ((__HAL_TIM_GetCounter(&htim3))<time);
}
void Set_Pin_Output (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void Set_Pin_Input (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}


void DHT11_Start (void)
{
	Set_Pin_Output (DHT11_PORT, DHT11_PIN);  // set the pin as output
	HAL_GPIO_WritePin (DHT11_PORT, DHT11_PIN, 0);   // pull the pin low
	delay (18000);   // wait for 18ms
  HAL_GPIO_WritePin (DHT11_PORT, DHT11_PIN, 1);   // pull the pin high
	delay (25);   // wait for 20us
	Set_Pin_Input(DHT11_PORT, DHT11_PIN);    // set as input
}

uint8_t DHT11_Check_Response (void)
{
	uint8_t Response = 0;
	delay (40);
	if (!(HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)))
	{
		delay (80);
		if ((HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN))) Response = 1;
		else Response = -1; // 255
	}
	while ((HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)));   // wait for the pin to go low

	return Response;
}

uint8_t DHT11_Read (void)
{
	uint8_t i,j;
	for (j=0;j<8;j++)
	{
		while (!(HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)));   // wait for the pin to go high
		delay (40);   // wait for 40 us
		if (!(HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)))   // if the pin is low
		{
			i&= ~(1<<(7-j));   // write 0
		}
		else i|= (1<<(7-j));  // if the pin is high, write 1
		while ((HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)));  // wait for the pin to go low
	}
	return i;
}

// Ham xu ly hien thi LCD
void send_data(unsigned char data)
{
HAL_GPIO_WritePin(D0_GPIO_Port,D0_Pin, data&0x01);
HAL_GPIO_WritePin(D1_GPIO_Port,D1_Pin, (data>>1)&0x01);
HAL_GPIO_WritePin(D2_GPIO_Port,D2_Pin, (data>>2)&0x01);
HAL_GPIO_WritePin(D3_GPIO_Port,D3_Pin, (data>>3)&0x01);
HAL_GPIO_WritePin(D4_GPIO_Port,D4_Pin, (data>>4)&0x01);
HAL_GPIO_WritePin(D5_GPIO_Port,D5_Pin, (data>>5)&0x01);
HAL_GPIO_WritePin(D6_GPIO_Port,D6_Pin, (data>>6)&0x01);
HAL_GPIO_WritePin(D7_GPIO_Port,D7_Pin, (data>>7)&0x01);
}
void LCD_command (char lenh)
{
	//RS_LCD=0;
	HAL_GPIO_WritePin(RS_GPIO_Port,RS_Pin,GPIO_PIN_RESET);
	//RW_LCD=0;
	HAL_GPIO_WritePin(RW_GPIO_Port,RW_Pin,GPIO_PIN_RESET);
	//EN_LCD=1;
	HAL_GPIO_WritePin(EN_GPIO_Port,EN_Pin,GPIO_PIN_SET);
	send_data(lenh);
	//EN_LCD=0;
	HAL_GPIO_WritePin(EN_GPIO_Port,EN_Pin,GPIO_PIN_RESET);
	HAL_Delay(10);
}
void LCD_data (char dulieu)
{
	//RS_LCD=1;
	HAL_GPIO_WritePin(RS_GPIO_Port,RS_Pin,GPIO_PIN_SET);
	//RW_LCD=0;
	HAL_GPIO_WritePin(RW_GPIO_Port,RW_Pin,GPIO_PIN_RESET);
	//EN_LCD=1;
	HAL_GPIO_WritePin(EN_GPIO_Port,EN_Pin,GPIO_PIN_SET);
	send_data(dulieu);
	//EN_LCD=0;
	HAL_GPIO_WritePin(EN_GPIO_Port,EN_Pin,GPIO_PIN_RESET);
	HAL_Delay(10);
}
void LCD_init(void)
{
	LCD_command(0x03); // 0000 0011
	LCD_command(0x38); // 0011 1000
	LCD_command(0x06); // 0000 0110
	LCD_command(0x0c); // 0000 1111 
	LCD_command(0x14); // 0001 0100
	LCD_command(0x01); // 0000 0001 // xoa man hinh
}
void send_string (char *str)
{
	while(*str)
	{
		LCD_data(*str);
		str++;	
	}
}

// Ham xu ly UART
void clearBuff_send(void){
	for(int i=0;i<100;i++){
	mang[i]=0;
	}
}
void sendData(uint8_t tb1,uint8_t tb2,uint8_t tb3,uint8_t tb4,uint8_t nd,uint8_t da){
	
	sprintf(mang,"{\"tb1\":%d,\"tb2\":%d,\"tb3\":%d,\"tb4\":%d,\"nd\":%d,\"da\":%d}\r\n",tb1,tb2,tb3,tb4,nd,da);
	sprintf(mang3,"AT+CIPSEND=%d,%d\r\n",1,51);
	HAL_UART_Transmit(&huart2,(uint8_t *)mang3,17,1000);
	HAL_Delay(100);
	HAL_UART_Transmit(&huart2,(uint8_t *)mang,51,1000);
	clearBuff_send();
}

void clearBuff(void){
	for(int i=0;i<100;i++){
	Rx_buff[i]=0;
	}
}

void xuly(char *data){
	char t[64];
  strncpy( t, data+10, strlen(data)-9); 
	str_json=cJSON_Parse(t);
	if(!str_json){
		//HAL_UART_Transmit(&huart2,(uint8_t*)"ERROR JSON\r\n",16,100);
		return;
	}else{
		//HAL_UART_Transmit(&huart2,(uint8_t*)"SUCCESS JSON\r\n",16,100);
			if(cJSON_GetObjectItem(str_json,"tb1")){
				if(cJSON_GetObjectItem(str_json,"tb1")->valueint==0){
				//HAL_UART_Transmit(&huart2,(uint8_t*)"Led1 on\r\n",12,100);
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);
				}else if(cJSON_GetObjectItem(str_json,"tb1")->valueint==1){
				HAL_UART_Transmit(&huart2,(uint8_t*)"Led1 off\r\n",12,100);
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);
				}
			}
			if(cJSON_GetObjectItem(str_json,"tb2")){
				if(cJSON_GetObjectItem(str_json,"tb2")->valueint==0){
				//	HAL_UART_Transmit(&huart2,(uint8_t*)"Led2 on\r\n",12,100);
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
				}else if(cJSON_GetObjectItem(str_json,"tb2")->valueint==1){
				HAL_UART_Transmit(&huart2,(uint8_t*)"Led2 off\r\n",12,100);
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
				}
			}
			if(cJSON_GetObjectItem(str_json,"tb3")){
				if(cJSON_GetObjectItem(str_json,"tb3")->valueint==0){
				//	HAL_UART_Transmit(&huart2,(uint8_t*)"Led3 on\r\n",12,100);
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
				}else if(cJSON_GetObjectItem(str_json,"tb3")->valueint==1){
				HAL_UART_Transmit(&huart2,(uint8_t*)"Led3 off\r\n",12,100);
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
				}
			}
			if(cJSON_GetObjectItem(str_json,"tb4")){
				if(cJSON_GetObjectItem(str_json,"tb4")->valueint==0){
				//	HAL_UART_Transmit(&huart2,(uint8_t*)"Led4 on\r\n",12,100);
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
				}else if(cJSON_GetObjectItem(str_json,"tb4")->valueint==1){
				HAL_UART_Transmit(&huart2,(uint8_t*)"Led4 off\r\n",12,100);
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
				}
			}
		cJSON_Delete(str_json);
}
}
void Receive(void){
	if(Rx_data!='\n'){
		Rx_buff[Rx_index++]=Rx_data;
	}else{
		Rx_index=0;
		xuly(Rx_buff);
		//sprintf(mang2,"\nData da nhan: %s\n",Rx_buff);
		//HAL_UART_Transmit(&huart2,mang2,strlen(mang2),1000);
		clearBuff();
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
		Receive();
		HAL_UART_Receive_IT(&huart2,&Rx_data,1);
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_RxCpltCallback could be implemented in the user file
   */
}




char lenh1[]="AT+CIPMUX=1\r\n";
char lenh2[]="AT+CIPSTART=1,\"TCP\",\"3.84.62.72\",8080\r\n";
char lenh_1[]="AT+CWMODE=3\r\n";
char lenh_2[]="AT+CIPMUX=1\r\n";
char lenh3[]="AT+CWSAP=\"thang\",\"12345678\",5,3\r\n";
char lenh4[]="AT+CIPSERVER=1,80\r\n";
char lenh5[]="AT+CWJAP=\"NI\",\"yen2k0809\"\r\n";
void ESP1_init(){
	HAL_UART_Transmit(&huart2,(uint8_t *)"AT\r\n",strlen("AT\r\n"),1000);
	HAL_Delay(10);
	//chuyen che do cua esp1
	HAL_UART_Transmit(&huart2,(uint8_t *)lenh_1,strlen(lenh1),1000);
	HAL_Delay(10);
	//xet chay o nhieu kenh
	HAL_UART_Transmit(&huart2,(uint8_t *)lenh_2,strlen(lenh2),1000);
	HAL_Delay(10);
	//xet phat
	HAL_UART_Transmit(&huart2,(uint8_t *)lenh3,strlen(lenh3),1000);
	HAL_Delay(10);
	//chay server
	HAL_UART_Transmit(&huart2,(uint8_t *)lenh4,strlen(lenh4),1000);
	HAL_Delay(10);
	//ket not toi wifi
	HAL_UART_Transmit(&huart2,(uint8_t *)lenh5,strlen(lenh5),1000);
	HAL_Delay(5000);
}


uint8_t data_tx[30];
uint8_t data_rx[30];

void w25q16_sector_earse(uint32_t addr){
	uint8_t data_buff[4];
	data_buff[0]=0x06;
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1,data_buff,1,100);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
	data_buff[0]=0x20;
	data_buff[1]=addr>>16;
	data_buff[2]=addr>>8;
	data_buff[3]=addr;
	HAL_SPI_Transmit(&hspi1,data_buff,4,100);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
	HAL_Delay(400);
}
void w25q16_write(uint32_t u32addr,uint8_t *u8data,uint16_t u16size){
	uint8_t data_buff[4];
	data_buff[0]=0x06;
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1,data_buff,1,100);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
	HAL_Delay(1);
	data_buff[0]=0x02;
	data_buff[1]=u32addr>>16;
	data_buff[2]=u32addr>>8;
	data_buff[3]=u32addr;
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1,data_buff,4,1000);
	HAL_SPI_Transmit(&hspi1,u8data,u16size,100);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
	HAL_Delay(5);
}
void w25q16_read(uint32_t u32addr,uint8_t *u8data,uint16_t u16size){
	uint8_t data_buff[4];
	data_buff[0]=0x03;
	data_buff[1]=u32addr>>16;
	data_buff[2]=u32addr>>8;
	data_buff[3]=u32addr;
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
	
	HAL_SPI_Transmit(&hspi1,data_buff,4,1000);
	HAL_SPI_Receive(&hspi1,u8data,u16size,1000);
	
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
}
uint8_t data_save[4];
uint8_t th[4];
void send_save(){
	if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_15)){
	data_save[0]=0x01;
	}else
	data_save[0]=0x00;
	if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14)){
	data_save[1]=0x01;
	}else
	data_save[1]=0x00;
	if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_13)){
	data_save[2]=0x01;
	}else
	data_save[2]=0x00;
	if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_12)){
	data_save[3]=0x01;
	}else
	data_save[3]=0x00;
	w25q16_sector_earse(0x00000000);
	HAL_Delay(10);
	w25q16_write(0x00000000,data_save,4);
	HAL_Delay(100);
	sendData(data_save[0],data_save[1],data_save[2],data_save[3],Temp_byte1,Rh_byte1);
	
	HAL_Delay(100);
	
	w25q16_read(0x00000000,th,4);
}
void setTB(uint16_t GPIO_Pin,uint8_t i){
	if(i==0x01){
	HAL_GPIO_WritePin(GPIOB,GPIO_Pin,GPIO_PIN_SET);
	}
	else{
	HAL_GPIO_WritePin(GPIOB,GPIO_Pin,GPIO_PIN_RESET);
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
  MX_SPI1_Init();
  MX_USART2_UART_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
	LCD_init();
	HAL_TIM_Base_Start(&htim3);
	HAL_UART_Receive_IT(&huart2,&Rx_data,1);
	
	//check ket noi voi w25q32
	data_tx[0]=0x9f;
	data_tx[1]=0x00;
	data_tx[2]=0x00;
	data_tx[3]=0x00;

	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);

	HAL_SPI_TransmitReceive(&hspi1,data_tx,data_rx,4,1000);

	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
	
	
	if(data_rx[1]!=0xef && data_rx[2]!=0x40 && data_rx[3]!=0x15){
		while(1){}
	}
	//get trang thai cua tb
	HAL_Delay(100);
	w25q16_read(0x00000000,th,4);
	setTB(GPIO_PIN_15,th[0]);
	setTB(GPIO_PIN_14,th[1]);
	setTB(GPIO_PIN_13,th[2]);
	setTB(GPIO_PIN_12,th[3]);
	// khoi tao ket noi ESP
	ESP1_init();
	HAL_UART_Transmit(&huart2,(uint8_t *)lenh2,strlen(lenh2),1000);
	HAL_Delay(1000);
	HAL_UART_Transmit(&huart2,(uint8_t *)"AT+CIPSEND=1,5\r\n",strlen("AT+CIPSEND=1,5\r\n"),1000);
	HAL_Delay(100);
	HAL_UART_Transmit(&huart2,(uint8_t *)"equip\r\n",strlen("equip\r\n"),1000);
	HAL_Delay(100);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		DHT11_Start();
	  Presence = DHT11_Check_Response();
	  Rh_byte1 = DHT11_Read ();
	  Rh_byte2 = DHT11_Read ();
	  Temp_byte1 = DHT11_Read ();
	  Temp_byte2 = DHT11_Read ();
	  SUM = DHT11_Read();
		HAL_Delay(1000);
		HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13);
		LCD_command(0x80);
		sprintf(mang,"NHIET DO: %d C ", Temp_byte1);
		send_string(mang);
		HAL_Delay(100);
		LCD_command(0xC0);
		sprintf(mang,"DO AM: %d  ",Rh_byte1);
		send_string(mang);
		HAL_Delay(100);
		send_save();
		HAL_Delay(2000);
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
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
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 72-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

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

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12
                          |GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15
                          |GPIO_PIN_3|GPIO_PIN_5|GPIO_PIN_6
                          |GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);
	
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA4 PA10 PA11 PA12
                           PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12
                          |GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB10 PB11 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_10|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB12 PB13 PB14 PB15
                           PB3 PB4 PB5 PB6
                           PB7 PB8 PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15
                          |GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6
                          |GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI1_IRQn, 4, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

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

