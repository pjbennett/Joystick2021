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

/*
 * NOTES
 *
 * 26-FEB-21 (v2.74)
 * Increased ADC sampling time from 1.5 cycles (min) to 7.5 cycles
 * in order to eliminate channel crosstalk (worked)
 *
 *
 *
*/

/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ssd1306.h"
#include "ssd1306_tests.h"
#include <string.h>
#include <stdio.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define VER "v2.76"
//v2.76 - on site at NBA Draft.  Increase zoom drift range from 20 to 40

#define TRUE 1
#define FALSE 0
#define ON 1
#define OFF 0
#define HIGH 1
#define LOW 0

#define ADC_BUF_LEN 1000 //make sure ADC_BUF_LEN is a multiple of 5 (channels)
#define FOCUS 0
#define ZOOM 1
#define PAN 2
#define TILT 3
#define ROLL 4

#define AVG_PAN 100
#define AVG_TILT 100
#define AVG_ROLL 100

#define AVG_FOCUS 100
#define AVG_ZOOM 100

#define MENU_OFF 0
#define MENU_MAIN 0 //another name for the main page
#define MENU_PAN 1
#define MENU_TILT 2
#define MENU_ROLL 3
#define MENU_ZOOM 4
#define MENU_FOCUS 5
#define MENU_AUX 6



#define TILT_A 0
#define TILT_B 1

#define CAL_OFF 0
#define CAL_START 1
#define CAL_FINISH 2

//BOX1 - Deadbands
//#define X1H 1975
//#define X1L 1950
//#define Y1H 1977
//#define Y1L 1957
//#define X2H 1975
//#define X2L 1956
//#define Y2H 1977
//#define Y2L 1958

//BOX2 - Deadbands
#define X1H 1999
#define X1L 1970
#define Y1H 1985
#define Y1L 1968
#define X2H 1972
#define X2L 1960
#define Y2H 1980
#define Y2L 1965

#define NUM_TICK 25

#define PAN_DEADBAND_SIZE 5
#define PAN_DEADBAND_CENTER 1950

#define ITEM_PAN_DRIFT 0
#define ITEM_PAN_DB_SIZE 1
#define ITEM_PAN_DB_CENTER 2
#define ITEM_PAN_SENSE 3
#define ITEM_PAN_EXIT 4

#define SET_PAN_INACTIVE 0
#define SET_PAN_DRIFT 1
#define SET_PAN_DB_SIZE 2
#define SET_PAN_DB_CENTER 3
#define SET_PAN_SENSE 4
#define SET_PAN_EXIT 5

#define ITEM_TILT_DRIFT 0
#define ITEM_TILT_DB_SIZE 1
#define ITEM_TILT_DB_CENTER 2
#define ITEM_TILT_SENSE 3
#define ITEM_TILT_EXIT 4

#define SET_TILT_INACTIVE 0
#define SET_TILT_DRIFT 1
#define SET_TILT_DB_SIZE 2
#define SET_TILT_DB_CENTER 3
#define SET_TILT_SENSE 4
#define SET_TILT_EXIT 5

#define ITEM_ROLL_DRIFT 0
#define ITEM_ROLL_DB_SIZE 1
#define ITEM_ROLL_DB_CENTER 2
#define ITEM_ROLL_SENSE 3
#define ITEM_ROLL_EXIT 4

#define SET_ROLL_INACTIVE 0
#define SET_ROLL_DRIFT 1
#define SET_ROLL_DB_SIZE 2
#define SET_ROLL_DB_CENTER 3
#define SET_ROLL_SENSE 4
#define SET_ROLL_EXIT 5

#define ITEM_ZOOM_DRIFT 0
#define ITEM_ZOOM_DB_SIZE 1
#define ITEM_ZOOM_DB_CENTER 2
#define ITEM_ZOOM_SENSE 3
#define ITEM_ZOOM_EXIT 4

#define SET_ZOOM_INACTIVE 0
#define SET_ZOOM_DRIFT 1
#define SET_ZOOM_DB_SIZE 2
#define SET_ZOOM_DB_CENTER 3
#define SET_ZOOM_SENSE 4
#define SET_ZOOM_EXIT 5

#define FLASH_PAGE_ADDR 0x0807f800

#define NORMAL 0
#define REVERSE 1

#define ZOOM_DRIFT_LIMIT 40

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc2;
DMA_HandleTypeDef hdma_adc2;

SPI_HandleTypeDef hspi2;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim8;

UART_HandleTypeDef huart2;

PCD_HandleTypeDef hpcd_USB_FS;

/* USER CODE BEGIN PV */
char displayBuff[40];
uint16_t adc_buf[ADC_BUF_LEN];
uint16_t adcValue[10];
uint8_t dataTX[15];

uint16_t headData[10];

//uint32_t avgPan;
uint16_t avgPanData[AVG_PAN];
uint16_t count_pan;
uint16_t prevPanValue;

//uint32_t avgTilt;
uint16_t avgTiltData[AVG_TILT];
uint16_t count_tilt;
uint16_t prevTiltValue;

//uint32_t avgRoll;
uint16_t avgRollData[AVG_ROLL];
uint16_t count_roll;
uint16_t prevRollValue;

uint16_t avgZoomData[AVG_ZOOM];
uint16_t count_zoom;
uint16_t prevZoomValue;

uint16_t avgFocusData[AVG_FOCUS];
uint16_t count_focus;
uint16_t prevFocusValue;

uint16_t panRawValue;
uint16_t panSamplingValue;
uint32_t panAvgValue;
int16_t panCalValue;
int16_t panAvgDisplay;

uint16_t tiltRawValue;
uint16_t tiltSamplingValue;
uint32_t tiltAvgValue;
int16_t tiltCalValue;
int16_t tiltAvgDisplay;

uint16_t rollRawValue;
uint16_t rollSamplingValue;
uint32_t rollAvgValue;
int16_t rollCalValue;
int16_t rollAvgDisplay;

uint16_t zoomRawValue;
uint16_t zoomSamplingValue;
uint32_t zoomAvgValue;
int16_t zoomCalValue;
int16_t zoomAvgDisplay;

uint16_t focusRawValue;
uint16_t focusSamplingValue;
uint32_t focusAvgValue;
int16_t focusCalValue;
int16_t focusAvgDisplay;

int16_t zoomValue;
int16_t panValue;
int16_t tiltValue;
int16_t rollValue;


uint8_t rollStatus;
uint8_t focusBarValue;
uint8_t zoomBarValue;
int8_t zoomOffset;
uint8_t zoomEncoderReset;

int8_t panOffset;
int8_t panDBsize;
int16_t panDBcenter;

int8_t panEncoderPinAPrev;

int8_t tiltOffset;

int16_t zoomValue;
int16_t panValue;
int16_t tiltValue;
int16_t rollValue;

int16_t panDisplayValue;
int16_t tiltDisplayValue;
int16_t rollDisplayValue;
int16_t zoomDisplayValue;
int16_t focusDisplayValue;

int8_t menuPage;
int8_t menuItem;
int8_t menuSetting;


uint8_t tiltSense;

int8_t PTSetting;
float PTRgain;


uint16_t state=0,counter=0;

uint8_t prevNextCode = 0;
uint16_t store=0;
uint8_t prevNextCodeRoll = 0;
uint16_t storeRoll=0;

int8_t c,val;

uint8_t auxPressed;

uint8_t menuPressed;
uint8_t panPressed;
uint8_t tiltPressed;
uint8_t rollPressed;
uint8_t zoomPressed;
uint8_t focusPressed;


uint16_t calibrationStart;
uint16_t calCount;
uint8_t calStatus;

uint16_t joystickMin;
uint16_t joystickMax;
uint16_t joystickCenter;

int8_t gainSetting;

float zoomCalFactor;

uint16_t testBuffer[100];
uint32_t testValue;

uint16_t rawTick[100];
uint16_t avgTick[100];
uint8_t tickCount;

uint32_t channel1 = 0;
uint32_t channel2 = 0;
uint32_t channel3 = 0;
uint32_t channel4 = 0;
uint32_t channel5 = 0;
uint32_t channel6 = 0;

uint16_t samplingCount = 0;

struct structConfig {
	int32_t panOffset;
	int32_t panDBsize;
	int32_t panDBcenter;
	int32_t panSense;
	int32_t tiltOffset;
	int32_t tiltDBsize;
	int32_t tiltDBcenter;
	int32_t tiltSense;
	int32_t rollOffset;
	int32_t rollDBsize;
	int32_t rollDBcenter;
	int32_t rollSense;
	int32_t zoomOffset;
	int32_t zoomDBsize;
	int32_t zoomDBcenter;
	int32_t zoomSense;
	int32_t focusOffset;
	int32_t focusDBsize;
	int32_t focusDBcenter;
	int32_t focusSense;
	int32_t ptrGain;
};

struct structConfig configData;

uint32_t flashAddr = FLASH_PAGE_ADDR;

int32_t lastZoomEncoderValue;
int32_t lastTiltEncoderValue;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_SPI2_Init(void);
static void MX_ADC2_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM8_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM1_Init(void);
static void MX_USB_PCD_Init(void);
/* USER CODE BEGIN PFP */
void updateZoomOffset (void);
void updateDisplay (void);
void sendCameraData(void);
void updateZoomEncoder (void);
void updateRollEncoder (void);
void updateTiltEncoder (void);
void updatePanEncoder (void);
int8_t readPanEncoder(void);
int8_t readRollEncoder(void);
void menuOff (void);
void menuZoom (void);
void menuRoll (void);
void menuTilt (void);
void menuPan (void);
void menuAux (void);
void saveConfigData (void);

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
  MX_SPI2_Init();
  MX_ADC2_Init();
  MX_USART2_UART_Init();
  MX_TIM2_Init();
  MX_TIM8_Init();
  MX_TIM3_Init();
  MX_TIM1_Init();
  MX_USB_PCD_Init();
  /* USER CODE BEGIN 2 */
  HAL_Delay(100);
  ssd1306_Init();
  //ssd1306_TestAll();

  ssd1306_Fill(Black);
  ssd1306_SetCursor(2, 0);
  ssd1306_WriteString("SUPRACAM - CAMERA OP", Font_6x8, White);
  ssd1306_SetCursor(2, 10);
  ssd1306_WriteString("CONTROL PANEL", Font_6x8, White);
  ssd1306_SetCursor(2, 20);
  ssd1306_WriteString("VERSION 1.14", Font_6x8, White);

  ssd1306_DrawRectangle(8,40,112,50,White);
  ssd1306_DrawRectangle(8,53,112,63,White);

  ssd1306_UpdateScreen();

  HAL_ADC_Start_DMA(&hadc2, (uint32_t*)adc_buf, ADC_BUF_LEN);
  HAL_TIM_Base_Start_IT(&htim2);

  HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL);
  HAL_TIM_Encoder_Start(&htim8, TIM_CHANNEL_ALL);
  HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);

  headData[FOCUS] = 2500;
  headData[ZOOM] = 2048;
  headData[PAN] = 1024;
  headData[TILT] = 1024;
  headData[ROLL] = 1024;

  gainSetting = 10;
  TIM1->CNT = 40;

  //read calibration from EEPROM
  configData.panOffset = (int32_t) *(uint32_t*)flashAddr;
  configData.panDBsize = (int32_t) *(uint32_t*)(flashAddr+=4);
  configData.panDBcenter = (int32_t) *(uint32_t*)(flashAddr+=4);
  configData.panSense = (int32_t) *(uint32_t*)(flashAddr+=4);
  configData.tiltOffset = (int32_t) *(uint32_t*)(flashAddr+=4);
  configData.tiltDBsize = (int32_t) *(uint32_t*)(flashAddr+=4);
  configData.tiltDBcenter = (int32_t) *(uint32_t*)(flashAddr+=4);
  configData.tiltSense = (int32_t) *(uint32_t*)(flashAddr+=4);
  configData.rollOffset = (int32_t) *(uint32_t*)(flashAddr+=4);
  configData.rollDBsize = (int32_t) *(uint32_t*)(flashAddr+=4);
  configData.rollDBcenter = (int32_t) *(uint32_t*)(flashAddr+=4);
  configData.rollSense = (int32_t) *(uint32_t*)(flashAddr+=4);
  configData.zoomOffset = (int32_t) *(uint32_t*)(flashAddr+=4);
  configData.zoomDBsize = (int32_t) *(uint32_t*)(flashAddr+=4);
  configData.zoomDBcenter = (int32_t) *(uint32_t*)(flashAddr+=4);
  configData.zoomSense = (int32_t) *(uint32_t*)(flashAddr+=4);

  if (configData.tiltDBcenter < 1800)
	  configData.tiltDBcenter = 1980;
  if (configData.tiltDBsize < 0)
	  configData.tiltDBsize = 5;
  if (configData.tiltOffset < 0)
	  configData.tiltOffset = 0;




  //uint32_t flashdata;
  //flashdata = *(uint32_t*)0x0807F800;
  //panOffset = (int8_t) flashdata;
  //panDBsize = 5;
  //panDBcenter = 1950;

  //PTRgain = 2.0; //range is 0.2 to 2.0

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	updatePanEncoder();
	updateTiltEncoder();
	updateRollEncoder();
	updateZoomEncoder();

	//ADC oversampling
	channel1 = 0;
	channel2 = 0;
	channel3 = 0;
	channel4 = 0;
	channel5 = 0;
	channel6 = 0;

	samplingCount = ADC_BUF_LEN/5; //make sure ADC_BUF_LEN is a multiple of 5 (channels)
	uint16_t j,k;
	k = 0;
	for (j=0; j<samplingCount; j++)
	{
		channel1 += adc_buf[k];
		channel2 += adc_buf[k+1];
		channel3 += adc_buf[k+2];
		channel4 += adc_buf[k+3];
		channel5 += adc_buf[k+4];
		//channel6 += adc_buf[k+5];
		k+=5;
	}

	channel1 /= samplingCount;
	channel2 /= samplingCount;
	channel3 /= samplingCount;
	channel4 /= samplingCount;
	channel5 /= samplingCount;
	//channel6 /= samplingCount;


	//--- FOCUS ---
	focusRawValue = adcValue[4];
	//focusSamplingValue = (uint16_t)(channel5>>1); //divide by 2 for a range of 0 to 2000ish
	focusSamplingValue = (uint16_t)((float)channel5/1.9); //get to a range of 0 to 2100ish

	avgFocusData[count_focus] = focusSamplingValue;
	count_focus++;
	if (count_focus >= AVG_FOCUS)
		count_focus = 0;

	focusAvgValue = 0;
	for (int i = 0; i<AVG_FOCUS; i++)
	{
		focusAvgValue = focusAvgValue + avgFocusData[i];
	}
	focusAvgValue = focusAvgValue/AVG_FOCUS;
	focusAvgDisplay = (int16_t)focusAvgValue;

	if (focusAvgValue > 2100) //limit top end of the range
		focusAvgValue = 2100;

	focusCalValue = focusAvgValue; //calibrated range is 0 to 2100
	focusDisplayValue = focusCalValue;

	//test display support
	rawTick[tickCount] = focusRawValue;
	avgTick[tickCount] = focusAvgValue;
	tickCount++;
	if (tickCount > NUM_TICK)
		tickCount = 0;

	headData[FOCUS] = focusAvgValue + 1024; //range is 1024 to 3124 (extra 100 is to ensure full range)

	//--- ZOOM ---
	zoomRawValue = adcValue[1];
	zoomSamplingValue = (uint16_t)(channel2);
	avgZoomData[count_zoom] = zoomSamplingValue;
	count_zoom++;
	if (count_zoom >= AVG_ZOOM)
		count_zoom = 0;
	zoomAvgValue = 0;
	for (int i = 0; i<AVG_ZOOM; i++)
	{
		zoomAvgValue = zoomAvgValue + avgZoomData[i];
	}
	zoomAvgValue = zoomAvgValue/AVG_ZOOM;
	zoomAvgDisplay = (int16_t)zoomAvgValue;

	//ZOOM CALIBRATION - DEADBAND
	zoomCalValue = 0;
	if (zoomAvgValue > (configData.zoomDBcenter + configData.zoomDBsize))
	{
		zoomCalValue = zoomAvgValue - (configData.zoomDBcenter + configData.zoomDBsize);
	}
	if (zoomAvgValue < (configData.zoomDBcenter - configData.zoomDBsize))
	{
		zoomCalValue = zoomAvgValue - (configData.zoomDBcenter - configData.zoomDBsize);
	}
	if (zoomCalValue > 500)
		zoomCalValue = 500;
	if (zoomCalValue < -500)
		zoomCalValue = -500;

	if (configData.zoomSense == REVERSE)
	{
		zoomCalValue = zoomCalValue *-1; //airplane - stick up is tilt down
	}

	headData[ZOOM] = (zoomCalValue*2)+2048+configData.zoomOffset;
	zoomDisplayValue = zoomCalValue;


	//--- PAN ---
	panRawValue = adcValue[2];
	panSamplingValue = (uint16_t)(channel3);
	avgPanData[count_pan] = panSamplingValue;
	count_pan++;
	if (count_pan >= AVG_PAN)
		count_pan = 0;
	panAvgValue = 0;
	for (int i = 0; i<AVG_PAN; i++)
	{
		panAvgValue = panAvgValue + avgPanData[i];
	}
	panAvgValue = panAvgValue/AVG_PAN;
	panAvgDisplay = (int16_t)panAvgValue;

	//PAN CALIBRATION - DEADBAND
	panCalValue = 0;
	if (panAvgValue > (configData.panDBcenter + configData.panDBsize))
	{
		panCalValue = panAvgValue - (configData.panDBcenter + configData.panDBsize);
	}
	if (panAvgValue < (configData.panDBcenter - configData.panDBsize))
	{
		panCalValue = panAvgValue - (configData.panDBcenter - configData.panDBsize);
	}
	if (panCalValue > 500)
		panCalValue = 500;
	if (panCalValue < -500)
		panCalValue = -500;

	if (configData.panSense == REVERSE)
	{
		panCalValue = panCalValue *-1; //airplane - stick up is tilt down
	}

	headData[PAN] = (int16_t)((float)panCalValue*PTRgain)+1024+configData.panOffset;
	panDisplayValue = panCalValue;


	//--- TILT ---
	tiltRawValue = adcValue[3];
	tiltSamplingValue = (uint16_t)(channel4);
	avgTiltData[count_tilt] = tiltSamplingValue;
	count_tilt++;
	if (count_tilt >= AVG_TILT)
		count_tilt = 0;
	tiltAvgValue = 0;
	for (int i = 0; i<AVG_TILT; i++)
	{
		tiltAvgValue = tiltAvgValue + avgTiltData[i];
	}
	tiltAvgValue = tiltAvgValue/AVG_TILT;
	tiltAvgDisplay = (int16_t)tiltAvgValue;

	//TILT CALIBRATION - DEADBAND
	tiltCalValue = 0;
	if (tiltAvgValue > (configData.tiltDBcenter + configData.tiltDBsize))
	{
		tiltCalValue = tiltAvgValue - (configData.tiltDBcenter + configData.tiltDBsize);
	}
	if (tiltAvgValue < (configData.tiltDBcenter - configData.tiltDBsize))
	{
		tiltCalValue = tiltAvgValue - (configData.tiltDBcenter - configData.tiltDBsize);
	}
	if (tiltCalValue > 500)
		tiltCalValue = 500;
	if (tiltCalValue < -500)
		tiltCalValue = -500;

	if (configData.tiltSense == REVERSE)
	{
		tiltCalValue = tiltCalValue *-1; //airplane - stick up is tilt down
	}

	headData[TILT] = (int16_t)((float)tiltCalValue*PTRgain)+1024+configData.tiltOffset;
	tiltDisplayValue = tiltCalValue;


	//--- ROLL ---
	rollRawValue = adcValue[0];
	rollSamplingValue = (uint16_t)(channel1);
	avgRollData[count_roll] = rollSamplingValue;
	count_roll++;
	if (count_roll >= AVG_ROLL)
		count_roll = 0;
	rollAvgValue = 0;
	for (int i = 0; i<AVG_ROLL; i++)
	{
		rollAvgValue = rollAvgValue + avgRollData[i];
	}
	rollAvgValue = rollAvgValue/AVG_ROLL;
	rollAvgDisplay = (int16_t)rollAvgValue;

	//ROLL CALIBRATION - DEADBAND
	rollCalValue = 0;
	if (rollAvgValue > (configData.rollDBcenter + configData.rollDBsize))
	{
		rollCalValue = rollAvgValue - (configData.rollDBcenter + configData.rollDBsize);
	}
	if (rollAvgValue < (configData.rollDBcenter - configData.rollDBsize))
	{
		rollCalValue = rollAvgValue - (configData.rollDBcenter - configData.rollDBsize);
	}
	if (rollCalValue > 500)
		rollCalValue = 500;
	if (rollCalValue < -500)
		rollCalValue = -500;

	if (configData.rollSense == REVERSE)
	{
		rollCalValue = rollCalValue *-1; //airplane - stick up is tilt down
	}

	// check roll switch
	if (HAL_GPIO_ReadPin(SERVO_SW_GPIO_Port, SERVO_SW_Pin))
	{
		rollStatus = 0;
		rollCalValue = 0;
	}
	else
	{
		rollStatus = 1;
	}

	headData[ROLL] = (int16_t)((float)rollCalValue*PTRgain)+1024+configData.rollOffset;
	rollDisplayValue = rollCalValue;

	//============================================


		/*
	  //ZOOM OFFSET ADJUSTMENT
	  zoomOffset = ((TIM8->CNT)>>2);
	  if (zoomOffset > 20)
	  {
		  zoomOffset = 20;
		  TIM8->CNT = 80;
	  }
	  if (zoomOffset < -20)
	  {
		  zoomOffset = -20;
	  	  TIM8->CNT = -80;
	  }
	  */


	  /*
	  //TILT OFFSET ADJUSTMENT
	  tiltOffset = ((TIM3->CNT)>>2);
	  if (tiltOffset > 20)
	  {
		  tiltOffset = 20;
		  TIM3->CNT = 80;
	  }
	  if (tiltOffset < -20)
	  {
		  tiltOffset = -20;
	  	  TIM3->CNT = -80;
	  }
	  */

	  //PTR GAIN ADJUSTMENT
	  gainSetting = ((TIM1->CNT)>>2);
	  if (gainSetting > 10)
	  {
		  gainSetting = 10;
		  TIM1->CNT = 40;
	  }
	  if (gainSetting < -9)
	  {
		  gainSetting = -9;
	  	  TIM1->CNT = -36;
	  }

	  gainSetting += 10; //gain from 1 to 20
	  PTRgain = (float)(gainSetting)/20.0; //sets actual gain range (twenty steps on encoder)



	//MENU SCREEN - RAW JOYSTICK DATA
	if ((HAL_GPIO_ReadPin(MENU_SW_GPIO_Port, MENU_SW_Pin) == 0) && (menuPressed == FALSE))
	{
		menuPressed = TRUE;
		if (menuPage == MENU_AUX)
			menuPage = MENU_OFF;
		else
		{
		 	 menuPage = MENU_AUX;
		}
	}
	if (HAL_GPIO_ReadPin(MENU_SW_GPIO_Port, MENU_SW_Pin))
	{
		menuPressed = FALSE;
	}


	//PAN SCREEN
	if ((HAL_GPIO_ReadPin(PAN_SW_GPIO_Port, PAN_SW_Pin) == 0) && (panPressed == FALSE))
	{
		panPressed = TRUE;

	 	if (menuPage == MENU_PAN)
	 	{
	 		if (menuItem == ITEM_PAN_DRIFT)
	 		{
	 			if (menuSetting == SET_PAN_INACTIVE)
	 				menuSetting = SET_PAN_DRIFT;
	 			else
	 				menuSetting = SET_PAN_INACTIVE;
	 		}
	 		if (menuItem == ITEM_PAN_DB_SIZE)
	 		{
	 			if (menuSetting == SET_PAN_INACTIVE)
	 				menuSetting = SET_PAN_DB_SIZE;
	 			else
	 				menuSetting = SET_PAN_INACTIVE;
	 		}
	 		if (menuItem == ITEM_PAN_DB_CENTER)
	 		{
	 			if (menuSetting == SET_PAN_INACTIVE)
	 				menuSetting = SET_PAN_DB_CENTER;
	 			else
	 				menuSetting = SET_PAN_INACTIVE;
	 		}
	 		if (menuItem == ITEM_PAN_SENSE)
	 		{
	 			if (menuSetting == SET_PAN_INACTIVE)
	 				menuSetting = SET_PAN_SENSE;
	 			else
	 				menuSetting = SET_PAN_INACTIVE;
	 		}
	 		if (menuItem == ITEM_PAN_EXIT)
	 		{
	 			saveConfigData();
	 			menuPage = MENU_MAIN;
	 		}
	 	}
	 	else
	 	{
	 		menuPage = MENU_PAN;
	 		menuItem = ITEM_PAN_DRIFT;
	 		menuSetting = SET_PAN_INACTIVE;
		}
	}

	if (HAL_GPIO_ReadPin(PAN_SW_GPIO_Port, PAN_SW_Pin))
	{
	 	panPressed = FALSE;
	}
	// END PAN SCREEN

	//TILT SCREEN
	if ((HAL_GPIO_ReadPin(TILT_SW_GPIO_Port, TILT_SW_Pin) == 0) && (tiltPressed == FALSE))
	{
		tiltPressed = TRUE;

	 	if (menuPage == MENU_TILT)
	 	{
	 		if (menuItem == ITEM_TILT_DRIFT)
	 		{
	 			if (menuSetting == SET_TILT_INACTIVE)
	 				menuSetting = SET_TILT_DRIFT;
	 			else
	 				menuSetting = SET_TILT_INACTIVE;
	 		}
	 		if (menuItem == ITEM_TILT_DB_SIZE)
	 		{
	 			if (menuSetting == SET_TILT_INACTIVE)
	 				menuSetting = SET_TILT_DB_SIZE;
	 			else
	 				menuSetting = SET_TILT_INACTIVE;
	 		}
	 		if (menuItem == ITEM_TILT_DB_CENTER)
	 		{
	 			if (menuSetting == SET_TILT_INACTIVE)
	 				menuSetting = SET_TILT_DB_CENTER;
	 			else
	 				menuSetting = SET_TILT_INACTIVE;
	 		}
	 		if (menuItem == ITEM_TILT_SENSE)
	 		{
	 			if (menuSetting == SET_TILT_INACTIVE)
	 				menuSetting = SET_TILT_SENSE;
	 			else
	 				menuSetting = SET_TILT_INACTIVE;
	 		}
	 		if (menuItem == ITEM_TILT_EXIT)
	 		{
	 			saveConfigData();
	 			menuPage = MENU_MAIN;
	 		}
	 	}
	 	else
	 	{
	 		menuPage = MENU_TILT;
	 		menuItem = ITEM_TILT_DRIFT;
	 		menuSetting = SET_TILT_INACTIVE;
		}
	}

	if (HAL_GPIO_ReadPin(TILT_SW_GPIO_Port, TILT_SW_Pin))
	{
	 	tiltPressed = FALSE;
	}
	// END TILT SCREEN


	//ROLL SCREEN
	if ((HAL_GPIO_ReadPin(ROLL_SW_GPIO_Port, ROLL_SW_Pin) == 0) && (rollPressed == FALSE))
	{
		rollPressed = TRUE;

	 	if (menuPage == MENU_ROLL)
	 	{
	 		if (menuItem == ITEM_ROLL_DRIFT)
	 		{
	 			if (menuSetting == SET_ROLL_INACTIVE)
	 				menuSetting = SET_ROLL_DRIFT;
	 			else
	 				menuSetting = SET_ROLL_INACTIVE;
	 		}
	 		if (menuItem == ITEM_ROLL_DB_SIZE)
	 		{
	 			if (menuSetting == SET_ROLL_INACTIVE)
	 				menuSetting = SET_ROLL_DB_SIZE;
	 			else
	 				menuSetting = SET_ROLL_INACTIVE;
	 		}
	 		if (menuItem == ITEM_ROLL_DB_CENTER)
	 		{
	 			if (menuSetting == SET_ROLL_INACTIVE)
	 				menuSetting = SET_ROLL_DB_CENTER;
	 			else
	 				menuSetting = SET_ROLL_INACTIVE;
	 		}
	 		if (menuItem == ITEM_ROLL_SENSE)
	 		{
	 			if (menuSetting == SET_ROLL_INACTIVE)
	 				menuSetting = SET_ROLL_SENSE;
	 			else
	 				menuSetting = SET_ROLL_INACTIVE;
	 		}
	 		if (menuItem == ITEM_ROLL_EXIT)
	 		{
	 			saveConfigData();
	 			menuPage = MENU_MAIN;
	 		}
	 	}
	 	else
	 	{
	 		menuPage = MENU_ROLL;
	 		menuItem = ITEM_ROLL_DRIFT;
	 		menuSetting = SET_ROLL_INACTIVE;
		}
	}

	if (HAL_GPIO_ReadPin(ROLL_SW_GPIO_Port, ROLL_SW_Pin))
	{
	 	rollPressed = FALSE;
	}
	// END ROLL SCREEN




	//ZOOM SCREEN
	if ((HAL_GPIO_ReadPin(ZOOM_SW_GPIO_Port, ZOOM_SW_Pin) == 0) && (zoomPressed == FALSE))
	{
		zoomPressed = TRUE;

	 	if (menuPage == MENU_ZOOM)
	 	{
	 		if (menuItem == ITEM_ZOOM_DRIFT)
	 		{
	 			if (menuSetting == SET_ZOOM_INACTIVE)
	 				menuSetting = SET_ZOOM_DRIFT;
	 			else
	 				menuSetting = SET_ZOOM_INACTIVE;
	 		}
	 		if (menuItem == ITEM_ZOOM_DB_SIZE)
	 		{
	 			if (menuSetting == SET_ZOOM_INACTIVE)
	 				menuSetting = SET_ZOOM_DB_SIZE;
	 			else
	 				menuSetting = SET_ZOOM_INACTIVE;
	 		}
	 		if (menuItem == ITEM_ZOOM_DB_CENTER)
	 		{
	 			if (menuSetting == SET_ZOOM_INACTIVE)
	 				menuSetting = SET_ZOOM_DB_CENTER;
	 			else
	 				menuSetting = SET_ZOOM_INACTIVE;
	 		}
	 		if (menuItem == ITEM_ZOOM_SENSE)
	 		{
	 			if (menuSetting == SET_ZOOM_INACTIVE)
	 				menuSetting = SET_ZOOM_SENSE;
	 			else
	 				menuSetting = SET_ZOOM_INACTIVE;
	 		}
	 		if (menuItem == ITEM_ZOOM_EXIT)
	 		{
	 			saveConfigData();
	 			menuPage = MENU_MAIN;
	 		}
	 	}
	 	else
	 	{
	 		menuPage = MENU_ZOOM;
	 		menuItem = ITEM_ZOOM_DRIFT;
	 		menuSetting = SET_ZOOM_INACTIVE;
		}
	}

	if (HAL_GPIO_ReadPin(ZOOM_SW_GPIO_Port, ZOOM_SW_Pin))
	{
	 	zoomPressed = FALSE;
	}
	// END ZOOM SCREEN



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
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB|RCC_PERIPHCLK_USART2
                              |RCC_PERIPHCLK_TIM1|RCC_PERIPHCLK_TIM8
                              |RCC_PERIPHCLK_ADC12|RCC_PERIPHCLK_TIM2
                              |RCC_PERIPHCLK_TIM34;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.Adc12ClockSelection = RCC_ADC12PLLCLK_DIV4;
  PeriphClkInit.USBClockSelection = RCC_USBCLKSOURCE_PLL;
  PeriphClkInit.Tim1ClockSelection = RCC_TIM1CLK_HCLK;
  PeriphClkInit.Tim8ClockSelection = RCC_TIM8CLK_HCLK;
  PeriphClkInit.Tim2ClockSelection = RCC_TIM2CLK_HCLK;
  PeriphClkInit.Tim34ClockSelection = RCC_TIM34CLK_HCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC2_Init(void)
{

  /* USER CODE BEGIN ADC2_Init 0 */

  /* USER CODE END ADC2_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC2_Init 1 */

  /* USER CODE END ADC2_Init 1 */
  /** Common config
  */
  hadc2.Instance = ADC2;
  hadc2.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc2.Init.Resolution = ADC_RESOLUTION_12B;
  hadc2.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc2.Init.ContinuousConvMode = ENABLE;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc2.Init.NbrOfConversion = 5;
  hadc2.Init.DMAContinuousRequests = ENABLE;
  hadc2.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc2.Init.LowPowerAutoWait = DISABLE;
  hadc2.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  if (HAL_ADC_Init(&hadc2) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.SamplingTime = ADC_SAMPLETIME_7CYCLES_5;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_2;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = ADC_REGULAR_RANK_3;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.Rank = ADC_REGULAR_RANK_4;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = ADC_REGULAR_RANK_5;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC2_Init 2 */

  /* USER CODE END ADC2_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 7;
  hspi2.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi2.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 10;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 10;
  if (HAL_TIM_Encoder_Init(&htim1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

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

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 47999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 33;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

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

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 10;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 10;
  if (HAL_TIM_Encoder_Init(&htim3, &sConfig) != HAL_OK)
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
  * @brief TIM8 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM8_Init(void)
{

  /* USER CODE BEGIN TIM8_Init 0 */

  /* USER CODE END TIM8_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM8_Init 1 */

  /* USER CODE END TIM8_Init 1 */
  htim8.Instance = TIM8;
  htim8.Init.Prescaler = 0;
  htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim8.Init.Period = 65535;
  htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim8.Init.RepetitionCounter = 0;
  htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 10;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 10;
  if (HAL_TIM_Encoder_Init(&htim8, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM8_Init 2 */

  /* USER CODE END TIM8_Init 2 */

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
  * @brief USB Initialization Function
  * @param None
  * @retval None
  */
static void MX_USB_PCD_Init(void)
{

  /* USER CODE BEGIN USB_Init 0 */

  /* USER CODE END USB_Init 0 */

  /* USER CODE BEGIN USB_Init 1 */

  /* USER CODE END USB_Init 1 */
  hpcd_USB_FS.Instance = USB;
  hpcd_USB_FS.Init.dev_endpoints = 8;
  hpcd_USB_FS.Init.speed = PCD_SPEED_FULL;
  hpcd_USB_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
  hpcd_USB_FS.Init.low_power_enable = DISABLE;
  hpcd_USB_FS.Init.lpm_enable = DISABLE;
  hpcd_USB_FS.Init.battery_charging_enable = DISABLE;
  if (HAL_PCD_Init(&hpcd_USB_FS) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_Init 2 */

  /* USER CODE END USB_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Channel1_IRQn);

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
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_STAT_GPIO_Port, LED_STAT_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LED_SERVO_Pin|OLED_CS_Pin|OLED_DC_Pin|OLED_RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_STAT_Pin */
  GPIO_InitStruct.Pin = LED_STAT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_STAT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : AUX_SW_Pin ZOOM_SW_Pin SERVO_SW_Pin ROLL_A_Pin
                           ROLL_B_Pin ROLL_SW_Pin */
  GPIO_InitStruct.Pin = AUX_SW_Pin|ZOOM_SW_Pin|SERVO_SW_Pin|ROLL_A_Pin
                          |ROLL_B_Pin|ROLL_SW_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : MENU_SW_Pin TILT_SW_Pin PAN_A_Pin PAN_B_Pin
                           PAN_SW_Pin */
  GPIO_InitStruct.Pin = MENU_SW_Pin|TILT_SW_Pin|PAN_A_Pin|PAN_B_Pin
                          |PAN_SW_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_SERVO_Pin OLED_CS_Pin OLED_DC_Pin OLED_RST_Pin */
  GPIO_InitStruct.Pin = LED_SERVO_Pin|OLED_CS_Pin|OLED_DC_Pin|OLED_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
// Called when buffer is completely filled
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
	adcValue[0] = adc_buf[0];
	adcValue[1] = adc_buf[1];
	adcValue[2] = adc_buf[2];
	adcValue[3] = adc_buf[3];
	adcValue[4] = adc_buf[4];
	//adcValue[5] = adc_buf[5];

}

void updateTiltEncoder (void)
{
	int32_t encoderValue;
	encoderValue = ((TIM3->CNT)>>2);

	val = 0;
	if (encoderValue > lastTiltEncoderValue)
		val = 1;
	if (encoderValue < lastTiltEncoderValue)
		val = -1;

	lastTiltEncoderValue = encoderValue;



	if (menuPage == MENU_TILT)
	{
		if (menuSetting == SET_TILT_INACTIVE)
		{
			menuItem += val;
			if (menuItem < 0)
				menuItem = 0;
			if (menuItem > 4)
				menuItem = 4;
		}

		if (menuSetting == SET_TILT_DRIFT)
		{
			configData.tiltOffset += val;
			if (configData.tiltOffset > 20)
				configData.tiltOffset = 20;
			if (configData.tiltOffset < -20)
				configData.tiltOffset = -20;
		}

		if (menuSetting == SET_TILT_DB_SIZE)
		{
			configData.tiltDBsize += val;
			if (configData.tiltDBsize > 25)
				configData.tiltDBsize = 25;
			if (configData.tiltDBsize < 1)
				configData.tiltDBsize = 1;
		}

		if (menuSetting == SET_TILT_DB_CENTER)
		{
			configData.tiltDBcenter += val;
			if (configData.tiltDBcenter > 2200)
				configData.tiltDBcenter = 2200;
			if (configData.tiltDBcenter < 1800)
				configData.tiltDBcenter = 1800;
		}

		if (menuSetting == SET_TILT_SENSE)
		{
			configData.tiltSense += val;
			if (configData.tiltSense > 1)
				configData.tiltSense = 1;
			if (configData.tiltSense < 0)
				configData.tiltSense = 0;
		}
	}

}

void updateZoomEncoder (void)
{
	int32_t encoderValue;
	encoderValue = ((TIM8->CNT)>>2);

	val = 0;
	if (encoderValue > lastZoomEncoderValue)
		val = 1;
	if (encoderValue < lastZoomEncoderValue)
		val = -1;

	lastZoomEncoderValue = encoderValue;



	if (menuPage == MENU_ZOOM)
	{
		if (menuSetting == SET_ZOOM_INACTIVE)
		{
			menuItem += val;
			if (menuItem < 0)
				menuItem = 0;
			if (menuItem > 4)
				menuItem = 4;
		}

		if (menuSetting == SET_ZOOM_DRIFT)
		{
			configData.zoomOffset += val;
			if (configData.zoomOffset > ZOOM_DRIFT_LIMIT)
				configData.zoomOffset = ZOOM_DRIFT_LIMIT;
			if (configData.zoomOffset < -ZOOM_DRIFT_LIMIT)
				configData.zoomOffset = -ZOOM_DRIFT_LIMIT;
		}

		if (menuSetting == SET_ZOOM_DB_SIZE)
		{
			configData.zoomDBsize += val;
			if (configData.zoomDBsize > 25)
				configData.zoomDBsize = 25;
			if (configData.zoomDBsize < 1)
				configData.zoomDBsize = 1;
		}

		if (menuSetting == SET_ZOOM_DB_CENTER)
		{
			configData.zoomDBcenter += val;
			if (configData.zoomDBcenter > 2200)
				configData.zoomDBcenter = 2200;
			if (configData.zoomDBcenter < 1800)
				configData.zoomDBcenter = 1800;
		}

		if (menuSetting == SET_ZOOM_SENSE)
		{
			configData.zoomSense += val;
			if (configData.zoomSense > 1)
				configData.zoomSense = 1;
			if (configData.zoomSense < 0)
				configData.zoomSense = 0;
		}
	}

}

int8_t readPanEncoder(void)
{

  static int8_t rot_enc_table[] = {0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0};


  prevNextCode <<= 2;

	uint8_t pinClock = HAL_GPIO_ReadPin(PAN_A_GPIO_Port, PAN_A_Pin);
	uint8_t pinData = HAL_GPIO_ReadPin(PAN_B_GPIO_Port, PAN_B_Pin);


  if (pinData) prevNextCode |= 0x02;
  if (pinClock) prevNextCode |= 0x01;
  prevNextCode &= 0x0f;

   // If valid then store as 16 bit data.
   if  (rot_enc_table[prevNextCode] ) {
      store <<= 4;
      store |= prevNextCode;
      //if (store==0xd42b) return 1;
      //if (store==0xe817) return -1;
      if ((store&0xff)==0x2b) return -1;
      if ((store&0xff)==0x17) return 1;
   }
   return 0;
}

void updatePanEncoder (void)
{
	val = readPanEncoder();

	if (menuPage == MENU_PAN)
	{
		if (menuSetting == SET_PAN_INACTIVE)
		{
			menuItem += val;
			if (menuItem < 0)
				menuItem = 0;
			if (menuItem > 4)
				menuItem = 4;
		}

		if (menuSetting == SET_PAN_DRIFT)
		{
			configData.panOffset += val;
			if (configData.panOffset > 20)
				configData.panOffset = 20;
			if (configData.panOffset < -20)
				configData.panOffset = -20;
		}

		if (menuSetting == SET_PAN_DB_SIZE)
		{
			configData.panDBsize += val;
			if (configData.panDBsize > 25)
				configData.panDBsize = 25;
			if (configData.panDBsize < 1)
				configData.panDBsize = 1;
		}

		if (menuSetting == SET_PAN_DB_CENTER)
		{
			configData.panDBcenter += val;
			if (configData.panDBcenter > 2200)
				configData.panDBcenter = 2200;
			if (configData.panDBcenter < 1800)
				configData.panDBcenter = 1800;
		}
		if (menuSetting == SET_PAN_SENSE)
		{
			configData.panSense += val;
			if (configData.panSense > 1)
				configData.panSense = 1;
			if (configData.panSense < 0)
				configData.panSense = 0;
		}
	}
}



int8_t readRollEncoder(void)
{

  static int8_t rot_enc_table[] = {0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0};


  prevNextCodeRoll <<= 2;

	uint8_t pinClock = HAL_GPIO_ReadPin(ROLL_A_GPIO_Port, ROLL_A_Pin);
	uint8_t pinData = HAL_GPIO_ReadPin(ROLL_B_GPIO_Port, ROLL_B_Pin);


  if (pinData) prevNextCodeRoll |= 0x02;
  if (pinClock) prevNextCodeRoll |= 0x01;
  prevNextCodeRoll &= 0x0f;

   // If valid then store as 16 bit data.
   if  (rot_enc_table[prevNextCodeRoll] ) {
      storeRoll <<= 4;
      storeRoll |= prevNextCodeRoll;
      //if (store==0xd42b) return 1;
      //if (store==0xe817) return -1;
      if ((storeRoll&0xff)==0x2b) return -1;
      if ((storeRoll&0xff)==0x17) return 1;
   }
   return 0;
}

void updateRollEncoder (void)
{
	val = readRollEncoder();

	if (menuPage == MENU_ROLL)
	{
		if (menuSetting == SET_ROLL_INACTIVE)
		{
			menuItem += val;
			if (menuItem < 0)
				menuItem = 0;
			if (menuItem > 4)
				menuItem = 4;
		}

		if (menuSetting == SET_ROLL_DRIFT)
		{
			configData.rollOffset += val;
			if (configData.rollOffset > 20)
				configData.rollOffset = 20;
			if (configData.rollOffset < -20)
				configData.rollOffset = -20;
		}

		if (menuSetting == SET_ROLL_DB_SIZE)
		{
			configData.rollDBsize += val;
			if (configData.rollDBsize > 25)
				configData.rollDBsize = 25;
			if (configData.rollDBsize < 1)
				configData.rollDBsize = 1;
		}

		if (menuSetting == SET_ROLL_DB_CENTER)
		{
			configData.rollDBcenter += val;
			if (configData.rollDBcenter > 2200)
				configData.rollDBcenter = 2200;
			if (configData.rollDBcenter < 1800)
				configData.rollDBcenter = 1800;
		}
		if (menuSetting == SET_ROLL_SENSE)
		{
			configData.rollSense += val;
			if (configData.rollSense > 1)
				configData.rollSense = 1;
			if (configData.rollSense < 0)
				configData.rollSense = 0;
		}
	}
}


void updateZoomOffset (void)
{
	zoomOffset++;
	if (zoomOffset > 50)
		zoomOffset = 50;
}


void updateDisplay (void)
{
	//display header
	ssd1306_Fill(Black);
	ssd1306_SetCursor(2, 0);
	sprintf(displayBuff,"SUPRA   CAM OP  %s", VER);
	ssd1306_WriteString(displayBuff, Font_6x8, White);
	//ssd1306_WriteString("SUPRA   CAM OP  %s", Font_6x8, White);
	ssd1306_Line(1,8,127,8,White);


	switch (menuPage)
	{
	case MENU_OFF:
		menuOff();
		break;

	case MENU_PAN:
		menuPan();
		break;

	case MENU_TILT:
		menuTilt();
		break;

	case MENU_ROLL:
		menuRoll();
		break;

	case MENU_ZOOM:
		menuZoom();
		break;

	case MENU_AUX:
		menuAux();
		break;

	default:
		menuOff();
	}



	ssd1306_UpdateScreen();

}

void menuOff (void)
{
	uint16_t p,r,c;

	//PAN MAIN DISPLAY
	p = 60;
	r = 12;
	ssd1306_SetCursor(2, r);
	sprintf(displayBuff,"PAN:            %04d", panDisplayValue);
	ssd1306_WriteString(displayBuff, Font_6x8, White);
	ssd1306_Line(p-25,r+5,p+25,r+5,White);
	ssd1306_Line(p-25,r+4,p-25,r+6,White);
	ssd1306_Line(p,r+4,p,r+6,White);
	ssd1306_Line(p+25,r+4,p+25,r+6,White);
	c = (uint16_t)(panDisplayValue / 20)+p;
	ssd1306_Line(c-3,r-1,c+3,r-1,White);
	ssd1306_Line(c-2,r,c+2,r,White);
	ssd1306_Line(c-1,r+1,c+1,r+1,White);
	ssd1306_Line(c,r+2,c,r+2,White);

	//TILT MAIN DISPLAY
	p = 60;
	r = 22;
	ssd1306_SetCursor(2, r);
	sprintf(displayBuff,"TILT:           %04d", tiltDisplayValue);
	ssd1306_WriteString(displayBuff, Font_6x8, White);
	ssd1306_Line(p-25,r+5,p+25,r+5,White);
	ssd1306_Line(p-25,r+4,p-25,r+6,White);
	ssd1306_Line(p,r+4,p,r+6,White);
	ssd1306_Line(p+25,r+4,p+25,r+6,White);
	c = (uint16_t)(tiltDisplayValue / 20)+p;
	ssd1306_Line(c-3,r-1,c+3,r-1,White);
	ssd1306_Line(c-2,r,c+2,r,White);
	ssd1306_Line(c-1,r+1,c+1,r+1,White);
	ssd1306_Line(c,r+2,c,r+2,White);

	//ROLL MAIN DISPLAY
	p = 60;
	r = 32;
	ssd1306_SetCursor(2, r);

	if (rollStatus)
		sprintf(displayBuff,"ROLL:           %04d", rollDisplayValue);
	else
		sprintf(displayBuff,"ROLL:           OFF!");

	ssd1306_WriteString(displayBuff, Font_6x8, White);
	ssd1306_Line(p-25,r+5,p+25,r+5,White);
	ssd1306_Line(p-25,r+4,p-25,r+6,White);
	ssd1306_Line(p,r+4,p,r+6,White);
	ssd1306_Line(p+25,r+4,p+25,r+6,White);
	c = (uint16_t)(rollDisplayValue / 20)+p;
	ssd1306_Line(c-3,r-1,c+3,r-1,White);
	ssd1306_Line(c-2,r,c+2,r,White);
	ssd1306_Line(c-1,r+1,c+1,r+1,White);
	ssd1306_Line(c,r+2,c,r+2,White);

	//ZOOM MAIN DISPLAY
	p = 45;
	r = 42;
	ssd1306_SetCursor(2, r);
	sprintf(displayBuff,"ZOOM:");
	ssd1306_WriteString(displayBuff, Font_6x8, White);
	ssd1306_Line(p-10,r+5,p+10,r+5,White);
	ssd1306_Line(p-10,r+4,p-10,r+6,White);
	ssd1306_Line(p,r+4,p,r+6,White);
	ssd1306_Line(p+10,r+4,p+10,r+6,White);
	c = (uint16_t)(zoomDisplayValue / 50)+p;
	ssd1306_Line(c-3,r-1,c+3,r-1,White);
	ssd1306_Line(c-2,r,c+2,r,White);
	ssd1306_Line(c-1,r+1,c+1,r+1,White);
	ssd1306_Line(c,r+2,c,r+2,White);

	//FOCUS MAIN DISPLAY
	p = 110;
	r = 42;
	ssd1306_SetCursor(62, r);
	sprintf(displayBuff,"FOCUS:");
	ssd1306_WriteString(displayBuff, Font_6x8, White);
	ssd1306_Line(p-10,r+5,p+10,r+5,White);
	ssd1306_Line(p-10,r+4,p-10,r+6,White);
	ssd1306_Line(p,r+4,p,r+6,White);
	ssd1306_Line(p+10,r+4,p+10,r+6,White);
	c = (uint16_t)(focusDisplayValue / 105)+p-10;
	ssd1306_Line(c-3,r-1,c+3,r-1,White);
	ssd1306_Line(c-2,r,c+2,r,White);
	ssd1306_Line(c-1,r+1,c+1,r+1,White);
	ssd1306_Line(c,r+2,c,r+2,White);


	//PTR GAIN DISPLAY
	ssd1306_SetCursor(2, 55);
	sprintf(displayBuff,"PTR GAIN");
	ssd1306_WriteString(displayBuff, Font_6x8, White);

	ssd1306_SetCursor(90, 55);
	sprintf(displayBuff,"%3d%%", (uint8_t)(gainSetting*5));
	ssd1306_WriteString(displayBuff, Font_6x8, White);

	ssd1306_Line(60,60,80,60,White);
	ssd1306_Line(60,59,60,61,White);
	ssd1306_Line(70,59,70,61,White);
	ssd1306_Line(80,59,80,61,White);

	uint16_t PTRgainDisplay = (uint16_t)(PTRgain * 20);

	p = PTRgainDisplay+60;

	ssd1306_Line(p-3,54,p+3,54,White);
	ssd1306_Line(p-2,55,p+2,55,White);
	ssd1306_Line(p-1,56,p+1,56,White);
	ssd1306_Line(p,57, p,57,White);

	//update screen command is in update display function

}


void menuPan (void)
{
	ssd1306_SetCursor(2, 0);
	sprintf(displayBuff,"PAN RAW:%04d CAL:%04d", panAvgDisplay, panCalValue);
	ssd1306_WriteString(displayBuff, Font_6x8, White);

	ssd1306_SetCursor(2, 15);
	sprintf(displayBuff,"   DRIFT:       %4d", (int16_t)configData.panOffset);
	ssd1306_WriteString(displayBuff, Font_6x8, White);

	ssd1306_SetCursor(2, 25);
	sprintf(displayBuff,"   DB SIZE:     %4d", (int16_t)configData.panDBsize);
	ssd1306_WriteString(displayBuff, Font_6x8, White);

	ssd1306_SetCursor(2, 35);
	sprintf(displayBuff,"   DB CENTER:   %4d", (int16_t)configData.panDBcenter);
	ssd1306_WriteString(displayBuff, Font_6x8, White);

	ssd1306_SetCursor(2, 45);
	if (configData.panSense == NORMAL)
		sprintf(displayBuff,"   SENSE:     NORMAL");
	else
		sprintf(displayBuff,"   SENSE:    REVERSE");
	ssd1306_WriteString(displayBuff, Font_6x8, White);

	ssd1306_SetCursor(2, 55);
	sprintf(displayBuff,"   EXIT");
	ssd1306_WriteString(displayBuff, Font_6x8, White);

	uint8_t posRow, posCol;

	posCol = 1;
	switch (menuItem)
	{
	case ITEM_PAN_DRIFT:
		posRow = 15;
		break;
	case ITEM_PAN_DB_SIZE:
		posRow = 25;
		break;
	case ITEM_PAN_DB_CENTER:
		posRow = 35;
		break;
	case ITEM_PAN_SENSE:
		posRow = 45;
		break;
	case ITEM_PAN_EXIT:
		posRow = 55;
		break;
	default:
		posRow = 15;
	}


	switch (menuSetting)
	{
	case SET_PAN_DRIFT:
		posRow = 15;
		posCol = 82;
		break;
	case SET_PAN_DB_SIZE:
		posRow = 25;
		posCol = 82;
		break;
	case SET_PAN_DB_CENTER:
		posRow = 35;
		posCol = 82;
		break;
	case SET_PAN_SENSE:
		posRow = 45;
		posCol = 65;
		break;
	}

	ssd1306_SetCursor(posCol, posRow);
	sprintf(displayBuff,"->");
	ssd1306_WriteString(displayBuff, Font_6x8, White);
	//update screen command is in update display function
}

void menuTilt (void)
{
	ssd1306_SetCursor(2, 0);
	sprintf(displayBuff,"TILT RW:%04d CAL:%04d", tiltAvgDisplay, tiltCalValue);
	ssd1306_WriteString(displayBuff, Font_6x8, White);

	ssd1306_SetCursor(2, 15);
	sprintf(displayBuff,"   DRIFT:       %4d", (int16_t)configData.tiltOffset);
	ssd1306_WriteString(displayBuff, Font_6x8, White);

	ssd1306_SetCursor(2, 25);
	sprintf(displayBuff,"   DB SIZE:     %4d", (int16_t)configData.tiltDBsize);
	ssd1306_WriteString(displayBuff, Font_6x8, White);

	ssd1306_SetCursor(2, 35);
	sprintf(displayBuff,"   DB CENTER:   %4d", (int16_t)configData.tiltDBcenter);
	ssd1306_WriteString(displayBuff, Font_6x8, White);

	ssd1306_SetCursor(2, 45);
	if (configData.tiltSense == NORMAL)
		sprintf(displayBuff,"   SENSE:     NORMAL");
	else
		sprintf(displayBuff,"   SENSE:    REVERSE");
	ssd1306_WriteString(displayBuff, Font_6x8, White);

	ssd1306_SetCursor(2, 55);
	sprintf(displayBuff,"   EXIT");
	ssd1306_WriteString(displayBuff, Font_6x8, White);

	uint8_t posRow, posCol;

	posCol = 1;
	switch (menuItem)
	{
	case ITEM_TILT_DRIFT:
		posRow = 15;
		break;
	case ITEM_TILT_DB_SIZE:
		posRow = 25;
		break;
	case ITEM_TILT_DB_CENTER:
		posRow = 35;
		break;
	case ITEM_TILT_SENSE:
		posRow = 45;
		break;
	case ITEM_TILT_EXIT:
		posRow = 55;
		break;
	default:
		posRow = 15;
	}


	switch (menuSetting)
	{
	case SET_TILT_DRIFT:
		posRow = 15;
		posCol = 82;
		break;
	case SET_TILT_DB_SIZE:
		posRow = 25;
		posCol = 82;
		break;
	case SET_TILT_DB_CENTER:
		posRow = 35;
		posCol = 82;
		break;
	case SET_TILT_SENSE:
		posRow = 45;
		posCol = 65;
		break;
	}

	ssd1306_SetCursor(posCol, posRow);
	sprintf(displayBuff,"->");
	ssd1306_WriteString(displayBuff, Font_6x8, White);
	//update screen command is in update display function
}

void menuRoll (void)
{
	ssd1306_SetCursor(2, 0);
	sprintf(displayBuff,"ROLL RW:%04d CAL:%04d", rollAvgDisplay, rollCalValue);
	ssd1306_WriteString(displayBuff, Font_6x8, White);

	ssd1306_SetCursor(2, 15);
	sprintf(displayBuff,"   DRIFT:       %4d", (int16_t)configData.rollOffset);
	ssd1306_WriteString(displayBuff, Font_6x8, White);

	ssd1306_SetCursor(2, 25);
	sprintf(displayBuff,"   DB SIZE:     %4d", (int16_t)configData.rollDBsize);
	ssd1306_WriteString(displayBuff, Font_6x8, White);

	ssd1306_SetCursor(2, 35);
	sprintf(displayBuff,"   DB CENTER:   %4d", (int16_t)configData.rollDBcenter);
	ssd1306_WriteString(displayBuff, Font_6x8, White);

	ssd1306_SetCursor(2, 45);
	if (configData.rollSense == NORMAL)
		sprintf(displayBuff,"   SENSE:     NORMAL");
	else
		sprintf(displayBuff,"   SENSE:    REVERSE");
	ssd1306_WriteString(displayBuff, Font_6x8, White);

	ssd1306_SetCursor(2, 55);
	sprintf(displayBuff,"   EXIT");
	ssd1306_WriteString(displayBuff, Font_6x8, White);

	uint8_t posRow, posCol;

	posCol = 1;
	switch (menuItem)
	{
	case ITEM_ROLL_DRIFT:
		posRow = 15;
		break;
	case ITEM_ROLL_DB_SIZE:
		posRow = 25;
		break;
	case ITEM_ROLL_DB_CENTER:
		posRow = 35;
		break;
	case ITEM_ROLL_SENSE:
		posRow = 45;
		break;
	case ITEM_ROLL_EXIT:
		posRow = 55;
		break;
	default:
		posRow = 15;
	}


	switch (menuSetting)
	{
	case SET_ROLL_DRIFT:
		posRow = 15;
		posCol = 82;
		break;
	case SET_ROLL_DB_SIZE:
		posRow = 25;
		posCol = 82;
		break;
	case SET_ROLL_DB_CENTER:
		posRow = 35;
		posCol = 82;
		break;
	case SET_ROLL_SENSE:
		posRow = 45;
		posCol = 65;
		break;
	}

	ssd1306_SetCursor(posCol, posRow);
	sprintf(displayBuff,"->");
	ssd1306_WriteString(displayBuff, Font_6x8, White);
	//update screen command is in update display function
}



void menuZoom (void)
{
	ssd1306_SetCursor(2, 0);
	sprintf(displayBuff,"ZOOM RW:%04d CAL:%04d", zoomAvgDisplay, zoomCalValue);
	ssd1306_WriteString(displayBuff, Font_6x8, White);

	ssd1306_SetCursor(2, 15);
	sprintf(displayBuff,"   DRIFT:       %4d", (int16_t)configData.zoomOffset);
	ssd1306_WriteString(displayBuff, Font_6x8, White);

	ssd1306_SetCursor(2, 25);
	sprintf(displayBuff,"   DB SIZE:     %4d", (int16_t)configData.zoomDBsize);
	ssd1306_WriteString(displayBuff, Font_6x8, White);

	ssd1306_SetCursor(2, 35);
	sprintf(displayBuff,"   DB CENTER:   %4d", (int16_t)configData.zoomDBcenter);
	ssd1306_WriteString(displayBuff, Font_6x8, White);

	ssd1306_SetCursor(2, 45);
	if (configData.zoomSense == NORMAL)
		sprintf(displayBuff,"   SENSE:     NORMAL");
	else
		sprintf(displayBuff,"   SENSE:    REVERSE");
	ssd1306_WriteString(displayBuff, Font_6x8, White);

	ssd1306_SetCursor(2, 55);
	sprintf(displayBuff,"   EXIT");
	ssd1306_WriteString(displayBuff, Font_6x8, White);

	uint8_t posRow, posCol;

	posCol = 1;
	switch (menuItem)
	{
	case ITEM_ZOOM_DRIFT:
		posRow = 15;
		break;
	case ITEM_ZOOM_DB_SIZE:
		posRow = 25;
		break;
	case ITEM_ZOOM_DB_CENTER:
		posRow = 35;
		break;
	case ITEM_ZOOM_SENSE:
		posRow = 45;
		break;
	case ITEM_ZOOM_EXIT:
		posRow = 55;
		break;
	default:
		posRow = 15;
	}


	switch (menuSetting)
	{
	case SET_ZOOM_DRIFT:
		posRow = 15;
		posCol = 82;
		break;
	case SET_ZOOM_DB_SIZE:
		posRow = 25;
		posCol = 82;
		break;
	case SET_ZOOM_DB_CENTER:
		posRow = 35;
		posCol = 82;
		break;
	case SET_ZOOM_SENSE:
		posRow = 45;
		posCol = 65;
		break;
	}

	ssd1306_SetCursor(posCol, posRow);
	sprintf(displayBuff,"->");
	ssd1306_WriteString(displayBuff, Font_6x8, White);
	//update screen command is in update display function
}



void menuAux (void)
{
	ssd1306_SetCursor(1, 15);
	sprintf(displayBuff,"F  RAW:%04d CAL:%04d", focusAvgDisplay, focusCalValue);
	ssd1306_WriteString(displayBuff, Font_6x8, White);

	//ssd1306_SetCursor(40,15);
	//sprintf(displayBuff,"AUX MENU");
	//ssd1306_WriteString(displayBuff, Font_6x8, White);

	ssd1306_SetCursor(1, 25);
	if (rollStatus)
		sprintf(displayBuff,"X1 RAW:%04d CAL:%04d", rollAvgDisplay, rollCalValue);
	else
		sprintf(displayBuff,"X1 RAW:%04d CAL: OFF", rollAvgDisplay);
	ssd1306_WriteString(displayBuff, Font_6x8, White);

	ssd1306_SetCursor(1, 35);
	sprintf(displayBuff,"Y1 RAW:%04d CAL:%04d", zoomAvgDisplay, zoomCalValue);
	ssd1306_WriteString(displayBuff, Font_6x8, White);

	ssd1306_SetCursor(1, 45);
	sprintf(displayBuff,"X2 RAW:%04d CAL:%04d", panAvgDisplay, panCalValue);
	ssd1306_WriteString(displayBuff, Font_6x8, White);

	ssd1306_SetCursor(1, 55);
	sprintf(displayBuff,"Y2 RAW:%04d CAL:%04d", tiltAvgDisplay, tiltCalValue);
	ssd1306_WriteString(displayBuff, Font_6x8, White);

	//for (uint8_t i=0;i<NUM_TICK;i++)
	//{
	//	ssd1306_Line((rawTick[i]/2)+1,9,(rawTick[i]/2)+1,10,White);
	//	ssd1306_Line((avgTick[i]/2)+1,12,(avgTick[i]/2)+1,13,White);
	//}

	//update screen command is in update display function
}


void sendCameraData (void)
{
	dataTX[0] = 0xAA;
	dataTX[1] = (uint8_t)(headData[FOCUS] >> 8);
	dataTX[2] = (uint8_t)headData[FOCUS];
	dataTX[3] = (uint8_t)(headData[ZOOM] >> 8);
	dataTX[4] = (uint8_t)headData[ZOOM];
	dataTX[5] = (uint8_t)(headData[PAN] >> 8);
	dataTX[6] = (uint8_t)headData[PAN];
	dataTX[7] = (uint8_t)(headData[TILT] >> 8);
	dataTX[8] = (uint8_t)headData[TILT];
	dataTX[9] = (uint8_t)(headData[ROLL] >> 8);
	dataTX[10] = (uint8_t)headData[ROLL];
	dataTX[11] = 0x55;

	HAL_UART_Transmit_IT(&huart2, dataTX , 12);
}

void saveConfigData (void)
{
	//save config data (structure) to flash
	HAL_FLASH_Unlock();

	flashAddr = FLASH_PAGE_ADDR;

	uint32_t page_error = 0;
	HAL_StatusTypeDef flashstatus;
	FLASH_EraseInitTypeDef s_eraseinit;

	s_eraseinit.TypeErase = FLASH_TYPEERASE_PAGES;
	s_eraseinit.NbPages = 1;
	s_eraseinit.PageAddress = flashAddr;

	flashstatus = HAL_FLASHEx_Erase(&s_eraseinit, &page_error);
	flashstatus++; //hack to eliminate variable not used warning

	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, flashAddr, (uint32_t) configData.panOffset);
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, flashAddr+=4, (uint32_t) configData.panDBsize);
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, flashAddr+=4, (uint32_t) configData.panDBcenter);
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, flashAddr+=4, (uint32_t) configData.panSense);

	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, flashAddr+=4, (uint32_t) configData.tiltOffset);
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, flashAddr+=4, (uint32_t) configData.tiltDBsize);
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, flashAddr+=4, (uint32_t) configData.tiltDBcenter);
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, flashAddr+=4, (uint32_t) configData.tiltSense);

	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, flashAddr+=4, (uint32_t) configData.rollOffset);
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, flashAddr+=4, (uint32_t) configData.rollDBsize);
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, flashAddr+=4, (uint32_t) configData.rollDBcenter);
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, flashAddr+=4, (uint32_t) configData.rollSense);

	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, flashAddr+=4, (uint32_t) configData.zoomOffset);
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, flashAddr+=4, (uint32_t) configData.zoomDBsize);
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, flashAddr+=4, (uint32_t) configData.zoomDBcenter);
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, flashAddr+=4, (uint32_t) configData.zoomSense);

	HAL_FLASH_Lock();
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
