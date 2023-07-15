/**
 * @file    wallet_menu.c
 * @author  Cypherock X1 Team
 * @brief   Populate and handle old wallet menu options
 * @copyright Copyright (c) 2023 HODL TECH PTE LTD
 * <br/> You may obtain a copy of license at <a href="https://mitcc.org/"
 *target=_blank>https://mitcc.org/</a>
 *
 ******************************************************************************
 * @attention
 *
 * (c) Copyright 2023 by HODL TECH PTE LTD
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

/*****************************************************************************
 * INCLUDES
 *****************************************************************************/
#include "wallet_menu.h"

#include "constant_texts.h"
#include "core_error_priv.h"
#include "menu_priv.h"
#include "ui_screens.h"
#include "view_seed_flow.h"
#include "wallet_list.h"

/*****************************************************************************
 * EXTERN VARIABLES
 *****************************************************************************/

/*****************************************************************************
 * PRIVATE MACROS AND DEFINES
 *****************************************************************************/

/*****************************************************************************
 * PRIVATE TYPEDEFS
 *****************************************************************************/
typedef enum {
  VIEW_SEED = 1,
  DELETE_WALLET,

} create_wallet_options_e;

/*****************************************************************************
 * STATIC FUNCTION PROTOTYPES
 *****************************************************************************/
/**
 * @brief This is the initializer callback for the wallet menu.
 *
 * @param ctx The engine context* from which the flow is invoked
 * @param data_ptr Currently unused pointer set by the engine
 */
static void wallet_menu_initialize(engine_ctx_t *ctx, const void *data_ptr);

/**
 * @brief This is the UI event handler for the wallet menu.
 * @details The function decodes the UI event and calls the wallet flow
 * based on selection. It deletes the wallet menu step if the back button
 * is pressed on the menu
 *
 * @param ctx The engine context* from which the flow is invoked
 * @param ui_event The ui event object which triggered the callback
 * @param data_ptr Currently unused pointer set by the engine
 */
static void wallet_menu_handler(engine_ctx_t *ctx,
                                ui_event_t ui_event,
                                const void *data_ptr);

/**
 * @brief This p0 event callback function handles clearing p0 events occured
 * while engine is waiting for other events.
 *
 * @details After main menu initalization, we don't expect p0 events as no
 * operation or flow has been started yet.
 *
 * @param ctx The engine context* from which the flow is invoked
 * @param p0_evt The p0 event object which triggered the callback
 * @param data_ptr Currently unused pointer set by the engine
 */
static void ignore_p0_handler(engine_ctx_t *ctx,
                              p0_evt_t p0_evt,
                              const void *data_ptr);
/*****************************************************************************
 * STATIC VARIABLES
 *****************************************************************************/
static uint8_t wallet_index = 0xFF;

static const flow_step_t wallet_step = {.step_init_cb = wallet_menu_initialize,
                                        .p0_cb = ignore_p0_handler,
                                        .ui_cb = wallet_menu_handler,
                                        .usb_cb = NULL,
                                        .nfc_cb = NULL,
                                        .evt_cfg_ptr = &device_nav_evt_config,
                                        .flow_data_ptr = &wallet_index};

/*****************************************************************************
 * GLOBAL VARIABLES
 *****************************************************************************/

/*****************************************************************************
 * STATIC FUNCTIONS
 *****************************************************************************/
static void ignore_p0_handler(engine_ctx_t *ctx,
                              p0_evt_t p0_evt,
                              const void *data_ptr) {
  ignore_p0_event();
}

static void wallet_menu_initialize(engine_ctx_t *ctx, const void *data_ptr) {
  char *wallet_list[MAX_WALLETS_ALLOWED] = {0};
  get_wallet_list((const char **)wallet_list);

  menu_init((const char **)ui_text_options_old_wallet,
            NUMBER_OF_OPTIONS_OLD_WALLET,
            (const char *)wallet_list[wallet_index],
            true);
  return;
}

static void wallet_menu_handler(engine_ctx_t *ctx,
                                ui_event_t ui_event,
                                const void *data_ptr) {
  if (UI_EVENT_LIST_CHOICE == ui_event.event_type) {
    switch (ui_event.list_selection) {
      case VIEW_SEED: {
        view_seed_flow(wallet_index);
        break;
      }
      case DELETE_WALLET: {
        // TODO: Handle delete wallet flow
        break;
      }
      default: {
        break;
      }
    }
  } else {
    // UI_EVENT_LIST_REJECTION handled below already
  }

  /* Return to the previous menu irrespective if UI_EVENT_REJECTION was
   * detected, or a wallet flow was executed */
  engine_delete_current_flow_step(ctx);

  return;
}

/*****************************************************************************
 * GLOBAL FUNCTIONS
 *****************************************************************************/
const flow_step_t *wallet_menu_get_step(uint8_t selection_index) {
  ASSERT(0 != selection_index && MAX_WALLETS_ALLOWED >= selection_index);

  wallet_index = selection_index - 1;
  return &wallet_step;
}
