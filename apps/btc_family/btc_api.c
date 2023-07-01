/**
 * @file    btc_api.c
 * @author  Cypherock X1 Team
 * @brief   Defines helpers apis for bitcoin app.
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

#include "btc_api.h"

#include <pb_decode.h>
#include <pb_encode.h>

#include "common_error.h"
#include "events.h"

/*****************************************************************************
 * EXTERN VARIABLES
 *****************************************************************************/

/*****************************************************************************
 * PRIVATE MACROS AND DEFINES
 *****************************************************************************/

/*****************************************************************************
 * PRIVATE TYPEDEFS
 *****************************************************************************/

/*****************************************************************************
 * STATIC VARIABLES
 *****************************************************************************/

/*****************************************************************************
 * GLOBAL VARIABLES
 *****************************************************************************/

/*****************************************************************************
 * STATIC FUNCTION PROTOTYPES
 *****************************************************************************/

/*****************************************************************************
 * STATIC FUNCTIONS
 *****************************************************************************/

/*****************************************************************************
 * GLOBAL FUNCTIONS
 *****************************************************************************/
bool decode_btc_query(const uint8_t *data,
                      uint16_t data_size,
                      btc_query_t *query_out) {
  if (NULL == data || NULL == query_out || 0 == data_size) {
    btc_send_error(ERROR_COMMON_ERROR_CORRUPT_DATA_TAG,
                   ERROR_DATA_FLOW_DECODING_FAILED);
    return false;
  }

  /* Initialize bitcoin query */
  btc_query_t query = BTC_QUERY_INIT_ZERO;

  /* Create a stream that reads from the buffer. */
  pb_istream_t stream = pb_istream_from_buffer(data, data_size);

  /* Now we are ready to decode the message. */
  bool status = pb_decode(&stream, BTC_QUERY_FIELDS, &query);

  /* Copy query obj if status is true*/
  if (true == status) {
    memcpy(query_out, &query, sizeof(query));
  } else {
    btc_send_error(ERROR_COMMON_ERROR_CORRUPT_DATA_TAG,
                   ERROR_DATA_FLOW_DECODING_FAILED);
  }

  return status;
}

bool encode_btc_result(btc_result_t *result,
                       uint8_t *buffer,
                       uint16_t max_buffer_len,
                       size_t *bytes_written_out) {
  if (NULL == result || NULL == buffer || NULL == bytes_written_out)
    return false;

  /* Create a stream that will write to our buffer. */
  pb_ostream_t stream = pb_ostream_from_buffer(buffer, max_buffer_len);

  /* Now we are ready to encode the message! */
  bool status = pb_encode(&stream, BTC_RESULT_FIELDS, result);

  if (true == status) {
    *bytes_written_out = stream.bytes_written;
  }

  return status;
}

bool check_btc_query(const btc_query_t *query, pb_size_t exp_query_tag) {
  if ((NULL == query) || (exp_query_tag != query->which_request)) {
    btc_send_error(ERROR_COMMON_ERROR_CORRUPT_DATA_TAG,
                   ERROR_DATA_FLOW_INVALID_QUERY);
    return false;
  }
  return true;
}

btc_result_t init_btc_result(pb_size_t result_tag) {
  btc_result_t result = BTC_RESULT_INIT_ZERO;
  result.which_response = result_tag;
  return result;
}

void btc_send_error(pb_size_t which_error, uint32_t error_code) {
  btc_result_t result = init_btc_result(BTC_RESULT_COMMON_ERROR_TAG);
  result.common_error = init_common_error(which_error, error_code);
  btc_send_result(&result);
}

void btc_send_result(btc_result_t *result) {
  // TODO: Eventually 1700 will be replaced by BTC_RESULT_SIZE when all
  // option files for bitcoin app are complete
  uint8_t buffer[1700] = {0};
  size_t bytes_encoded = 0;
  ASSERT(encode_btc_result(result, buffer, sizeof(buffer), &bytes_encoded));
  usb_send_msg(&buffer[0], bytes_encoded);
}

bool btc_get_query(btc_query_t *query, pb_size_t exp_query_tag) {
  evt_status_t event = get_events(EVENT_CONFIG_USB, MAX_INACTIVITY_TIMEOUT);

  if (true == event.p0_event.flag) {
    return false;
  }

  if (!decode_btc_query(
          event.usb_event.p_msg, event.usb_event.msg_size, query)) {
    return false;
  }

  if (!check_btc_query(query, exp_query_tag)) {
    return false;
  }

  return true;
}
