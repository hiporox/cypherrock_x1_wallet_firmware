/**
 * @file    polygon_app.c
 * @author  Cypherock X1 Team
 * @brief   Polygon application configuration and helpers
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

#include "polygon_app.h"

#include <stddef.h>

#include "evm_main.h"

/*****************************************************************************
 * EXTERN VARIABLES
 *****************************************************************************/

/**
 * @brief Whitelisted contracts with respective token symbol
 * @details A map of Ethereum contract addresses with their token symbols. These
 * will enable the device to verify the ERC20 token transaction in a
 * user-friendly manner.
 *
 * @see erc20_contracts_t
 */
extern const erc20_contracts_t polygon_contracts[];

/*****************************************************************************
 * PRIVATE MACROS AND DEFINES
 *****************************************************************************/

/*****************************************************************************
 * PRIVATE TYPEDEFS
 *****************************************************************************/

/*****************************************************************************
 * STATIC FUNCTION PROTOTYPES
 *****************************************************************************/

/**
 * @brief Checks if the provided token address is whitelisted and return the
 * matching contract instance.
 *
 * @param address Reference to the buffer containing the token address
 * @param contract Pointer to store the matched contract address instance
 *
 * @return bool Indicating if the provided token address is whitelisted
 * @return true If the address matches to an entry in the whitelist
 * @return false If the address does not match to an entry in the whitelist
 */
static bool is_token_whitelisted(const uint8_t *address,
                                 const erc20_contracts_t **contract);

/*****************************************************************************
 * STATIC VARIABLES
 *****************************************************************************/

static const evm_config_t polygon_app_config = {
    .lunit_name = "MATIC",
    .name = "Polygon",
    .chain_id = 137,

    // whitelisted contracts
    .is_token_whitelisted = is_token_whitelisted,
};

static const cy_app_desc_t polygon_app_desc = {
    .id = 9,
    .version =
        {
            .major = 1,
            .minor = 1,
            .patch = 0,
        },
    .app = evm_main,
    .app_config = &polygon_app_config};

/*****************************************************************************
 * GLOBAL VARIABLES
 *****************************************************************************/

/*****************************************************************************
 * STATIC FUNCTIONS
 *****************************************************************************/

static bool is_token_whitelisted(const uint8_t *address,
                                 const erc20_contracts_t **contract) {
  if (NULL != contract) {
    *contract = NULL;
  }
  return false;
}

/*****************************************************************************
 * GLOBAL FUNCTIONS
 *****************************************************************************/

const cy_app_desc_t *get_polygon_app_desc() {
  return &polygon_app_desc;
}