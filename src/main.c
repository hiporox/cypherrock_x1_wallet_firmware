/**
 * @file    main.c
 * @author  Cypherock X1 Team
 * @brief   Main source file.
 *          Entry point to the application.
 * @copyright Copyright (c) 2022 HODL TECH PTE LTD
 * <br/> You may obtain a copy of license at <a href="https://mitcc.org/"
 *target=_blank>https://mitcc.org/</a>
 *
 ******************************************************************************
 * @attention
 *
 * (c) Copyright 2022 by HODL TECH PTE LTD
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *
 * "Commons Clause" License Condition v1.0
 *
 * The Software is provided to you by the Licensor under the License,
 * as defined below, subject to the following condition.
 *
 * Without limiting other conditions in the License, the grant of
 * rights under the License will not include, and the License does not
 * grant to you, the right to Sell the Software.
 *
 * For purposes of the foregoing, "Sell" means practicing any or all
 * of the rights granted to you under the License to provide to third
 * parties, for a fee or other consideration (including without
 * limitation fees for hosting or consulting/ support services related
 * to the Software), a product or service whose value derives, entirely
 * or substantially, from the functionality of the Software. Any license
 * notice or attribution required by the License must also include
 * this Commons Clause License Condition notice.
 *
 * Software: All X1Wallet associated files.
 * License: MIT
 * Licensor: HODL TECH PTE LTD
 *
 ******************************************************************************
 */
/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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
#define _DEFAULT_SOURCE /* needed for usleep() */
#include <stdlib.h>
#include <unistd.h>
#define SDL_MAIN_HANDLED /*To fix SDL's "undefined reference to WinMain"       \
                            issue*/
#include "application_startup.h"
#include "board.h"
#include "flow_engine.h"
#include "sys_state.h"
#include "ui_logo.h"

#if USE_SIMULATOR == 0
#include "cmsis_gcc.h"
#include "main.h"
#include "nfc.h"
#include "ui_message.h"
#include "usbd_core.h"

#endif    // USE_SIMULATOR

#if USE_SIMULATOR == 1
#ifdef _WIN32
#define main SDL_main
#endif
#include "sim_usb.h"
extern lv_indev_t *indev_keypad;
#endif
extern Counter counter;
extern bool main_app_ready;

#if USE_SIMULATOR == 1

/*On OSX SDL needs different handling*/
#if defined(__APPLE__) && defined(TARGET_OS_MAC)
#if __APPLE__ && TARGET_OS_MAC
#define SDL_APPLE
#endif
#endif

static int tick_thread(void *data);
static void memory_monitor(lv_task_t *param);

#endif
#ifdef DEV_BUILD
#include "dev_utils.h"
#endif
/**
 * @brief  The entry point to the application.
 * @retval int
 */

int main(void) {
#ifdef DEV_BUILD
  ekp_queue_init();
#endif

  /* Initialize the GPIO, clocks, LVGL library, i2c drivers, communication
   * layers for USB and NFC based data exchanges and everything related to the
   * hardware and low level software */
  application_init();

  /* Good place to display the Cypherock logo */
  logo_scr_init(2000);

  /* Initialize the flow engine which will be useful for navigation on the menu.
   * Starting point for the device could be different based on different state
   * of the device */
  engine_initialize();

  // TODO: Update with main menu flow for the first time, using dummy variable
  // as of now
  flow_step_t flow_step = {0};
  engine_next_flow_step(ENGINE_LIFO_A, &flow_step);

  while (1) {
    engine_run(ENGINE_LIFO_A);
  }

  return 0;
}

#if USE_SIMULATOR == 0

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1) {
  }
  /* USER CODE END Error_Handler_Debug */
}

/**
 * @brief Function to transmit data in real-time over SWV channel
 * @param file unused
 * @param *ptr string pointer for data to send
 * @param len  length of data to send
 *
 * @ret len of data transmitted
 */
int _write(int file, char *ptr, int len) {
#ifndef NDEBUG    // Disable printf in release mode
  int DataIdx;
  for (DataIdx = 0; DataIdx < len; DataIdx++) {
    ITM_SendChar(*ptr++);
  }
  return len;
#endif
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line) {
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line
     number, ex: printf("Wrong parameters value: file %s on line %d\r\n", file,
     line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

#endif    // USE_SIMULATOR
