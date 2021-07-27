/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_STAT_Pin GPIO_PIN_13
#define LED_STAT_GPIO_Port GPIOC
#define AUX_SW_Pin GPIO_PIN_2
#define AUX_SW_GPIO_Port GPIOC
#define MENU_SW_Pin GPIO_PIN_2
#define MENU_SW_GPIO_Port GPIOB
#define LED_SERVO_Pin GPIO_PIN_10
#define LED_SERVO_GPIO_Port GPIOB
#define OLED_CS_Pin GPIO_PIN_11
#define OLED_CS_GPIO_Port GPIOB
#define OLED_DC_Pin GPIO_PIN_12
#define OLED_DC_GPIO_Port GPIOB
#define OLED_RST_Pin GPIO_PIN_14
#define OLED_RST_GPIO_Port GPIOB
#define ZOOM_SW_Pin GPIO_PIN_8
#define ZOOM_SW_GPIO_Port GPIOC
#define SERVO_SW_Pin GPIO_PIN_9
#define SERVO_SW_GPIO_Port GPIOC
#define ROLL_A_Pin GPIO_PIN_10
#define ROLL_A_GPIO_Port GPIOC
#define ROLL_B_Pin GPIO_PIN_11
#define ROLL_B_GPIO_Port GPIOC
#define ROLL_SW_Pin GPIO_PIN_12
#define ROLL_SW_GPIO_Port GPIOC
#define TILT_SW_Pin GPIO_PIN_6
#define TILT_SW_GPIO_Port GPIOB
#define PAN_A_Pin GPIO_PIN_7
#define PAN_A_GPIO_Port GPIOB
#define PAN_B_Pin GPIO_PIN_8
#define PAN_B_GPIO_Port GPIOB
#define PAN_SW_Pin GPIO_PIN_9
#define PAN_SW_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/