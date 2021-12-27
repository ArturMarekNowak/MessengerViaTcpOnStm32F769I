/**
  ******************************************************************************
  * @file    PWR/PWR_CurrentConsumption/stm32f7xx_lp_modes.h
  * @author  MCD Application Team
  * @brief   Header for stm32f7xx_lp_modes.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F7xx_LP_MODES_H
#define __STM32F7xx_LP_MODES_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#if !defined (SLEEP_MODE) && !defined (STOP_MODE) && !defined (STANDBY_MODE)\
 && !defined (STANDBY_RTC_MODE) && !defined (STANDBY_BKPSRAM_MODE)
/* Uncomment the corresponding line to select the STM32F7xx Low Power mode */
/*#define SLEEP_MODE*/
/*#define STOP_MODE*/
/*#define STANDBY_MODE*/
/*#define STANDBY_RTC_MODE*/
#define STANDBY_BKPSRAM_MODE
#endif

#if !defined (SLEEP_MODE) && !defined (STOP_MODE) && !defined (STANDBY_MODE)\
 && !defined (STANDBY_RTC_MODE) && !defined (STANDBY_BKPSRAM_MODE)
 #error "Please select first the target STM32F7xx Low Power mode to be measured (in stm32f7xx_lp_modes.h file)"
#endif

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void SleepMode_Measure(void);
void StopMode_Measure(void);
void StandbyMode_Measure(void);
void StandbyRTCMode_Measure(void);
void StandbyBKPSRAMMode_Measure(void);

uint32_t USB_ULPI_Read(uint32_t Addr);
uint32_t USB_ULPI_Write(uint32_t Addr, uint32_t Data);   
void USB_ULPI_MspInit(void);
void USB_PhyEnterLowPowerMode(void);
void USB_PhyExitFromLowPowerMode(void);
void ETH_PhyEnterPowerDownMode(void);
void ETH_PhyExitFromPowerDownMode(void);


#endif /* __STM32F7xx_LP_MODES_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
