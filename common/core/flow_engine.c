/**
 * @file    flow_engine.c
 * @author  Cypherock X1 Team
 * @brief
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
#include "flow_engine.h"

#include "cQueue.h"

/*****************************************************************************
 * EXTERN VARIABLES
 *****************************************************************************/

/*****************************************************************************
 * PRIVATE MACROS AND DEFINES
 *****************************************************************************/
#define ENGINE_NUM_STACKS 2

#define ENGINE_RUN_INITIALIZE_CB(cb, data_ptr)                                 \
  {                                                                            \
    if (cb) {                                                                  \
      cb(data_ptr);                                                            \
    }                                                                          \
  }

#define ENGINE_RUN_EVENT_CB(cb, data_ptr, event)                               \
  {                                                                            \
    if (cb) {                                                                  \
      cb(event, data_ptr);                                                     \
    }                                                                          \
  }

/*****************************************************************************
 * PRIVATE TYPEDEFS
 *****************************************************************************/
typedef struct {
  flow_step_t **buffer;
  queue_t *queue_config;
} engine_ctx_t;

/*****************************************************************************
 * STATIC VARIABLES
 *****************************************************************************/
static flow_step_t *engine_stack_a_buffer[ENGINE_STACK_DEPTH] = {0};
static queue_t engine_stack_a;

static flow_step_t *engine_queue_a_buffer[ENGINE_STACK_DEPTH] = {0};
static queue_t engine_queue_a;

static engine_ctx_t engine_ctx[ENGINE_NUM_STACKS] = {
    {&engine_stack_a_buffer[0], &engine_stack_a},
    {&engine_queue_a_buffer[0], &engine_queue_a}};

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
void engine_initialize(void) {
  /* Initialize all queues support by the engine */
  for (uint8_t indx = 0; indx < ENGINE_NUM_STACKS; indx++) {
    (void)engine_reset_flow(indx);
  }
}

bool engine_reset_flow(uint8_t engine_stack) {
  /* Ensure that the stack requested to be reset is implemented. It is important
   * to handle this case as this API is exposed to the external world and
   * therefore we must ensure that the argument is valid */
  if (ENGINE_NUM_STACKS <= engine_stack) {
    return false;
  }

  /* Zero-ise the content of the queue and the queue struct */
  memzero(engine_ctx[engine_stack].buffer,
          ENGINE_STACK_DEPTH * (sizeof(flow_step_t *)));
  memzero(engine_ctx[engine_stack].queue_config, sizeof(queue_t));

  /* We need to re-initialize the queue - LIFO or FIFO based on the engine_stack
   * index */
  queue_type_e buffer_type = LIFO;
  if (engine_stack >= ENGINE_FIFO_A) {
    buffer_type = FIFO;
  }

  /* Initialize the queue with the appropriate parameters. The return is ignored
   * here intentionally as this API call is not expected to fail as all
   * allocations are static */
  (void)q_init_static(engine_ctx[engine_stack].queue_config,
                      sizeof(flow_step_t *),
                      ENGINE_STACK_DEPTH,
                      buffer_type,
                      engine_ctx[engine_stack].buffer,
                      ENGINE_STACK_DEPTH * (sizeof(flow_step_t *)));

  return true;
}

bool engine_next_flow_step(uint8_t engine_stack,
                           const flow_step_t *flow_step_ptr) {
  bool result = false;

  /* Ensure that the stack requested to be reset is implemented. It is important
   * to handle this case as this API is exposed to the external world and
   * therefore we must ensure that the argument is valid */
  if (ENGINE_NUM_STACKS <= engine_stack) {
    return result;
  }

  /* We want to store the pointer to the flow step inputted from the argument
   * and NOT the actual content, therefore we need to pass the double pointer to
   * the API */
  const flow_step_t **flow_step_dptr = &flow_step_ptr;
  result = q_push(engine_ctx[engine_stack].queue_config, flow_step_dptr);
  return result;
}

bool engine_prev_flow_step(uint8_t engine_stack) {
  bool result = false;

  /* Ensure that the stack requested to be reset is implemented. It is important
   * to handle this case as this API is exposed to the external world and
   * therefore we must ensure that the argument is valid */
  if (ENGINE_NUM_STACKS <= engine_stack) {
    return result;
  }

  /* The q_pop API will copy the content on the pointer given as the argument.
   * Since we have only pushed the pointer to the flow_step_t in
   * engine_next_flow_step, therefore we only need to allocate space for a
   * pointer, which can be a void *. Also, this API is expected to do internal
   * pop, so the popped element is not returned in any way to the caller */
  void *flow_step_ptr;
  result = q_pop(engine_ctx[engine_stack].queue_config, &flow_step_ptr);
  return result;
}

bool engine_current_flow_step(uint8_t engine_stack,
                              flow_step_t **flow_step_dptr) {
  bool result = false;

  /* Ensure that the stack requested to be reset is implemented. It is important
   * to handle this case as this API is exposed to the external world and
   * therefore we must ensure that the argument is valid */
  if (ENGINE_NUM_STACKS <= engine_stack) {
    return result;
  }

  /* q_peek fills the content from the queue into the (double) pointer */
  result = q_peek(engine_ctx[engine_stack].queue_config, flow_step_dptr);
  return result;
}

void engine_run(uint8_t engine_stack) {
  /* Ensure that the stack requested to be reset is implemented. It is important
   * to handle this case as this API is exposed to the external world and
   * therefore we must ensure that the argument is valid */
  if (ENGINE_NUM_STACKS <= engine_stack) {
    return;
  }

  while (1) {
    flow_step_t *current_flow = NULL;

    /* Break if the stack or queue reached an empty state */
    if ((false == engine_current_flow_step(engine_stack, &current_flow)) ||
        (NULL == current_flow)) {
      break;
    }

    const evt_config_t *evt_config_ptr = current_flow->evt_cfg_ptr;
    /* We must ensure that the `evt_config_ptr` is not NULL */
    ASSERT(NULL != evt_config_ptr);

    const void *flow_data_ptr = current_flow->flow_data_ptr;

    /* If code flow reaches this point, it means that the UX flow is still not
     * complete. */
    ENGINE_RUN_INITIALIZE_CB(current_flow->step_init_cb, flow_data_ptr);

    evt_status_t evt_status = {0};
    evt_config_t evt_config = *evt_config_ptr;

    get_events(evt_config, &evt_status);

    /* Callbacks are called based on the event status returned by the get_events
     * API. It is expected that the evt_config and the callbacks match, meaning
     * that if any flow has requested to listen about x events, it must register
     * a callback for that event. */
    if (true == evt_status.p0_event.flag) {
      ENGINE_RUN_EVENT_CB(
          current_flow->p0_cb, flow_data_ptr, evt_status.p0_event);
    } else if (true == evt_status.ui_event.event_occured) {
      ENGINE_RUN_EVENT_CB(
          current_flow->ui_cb, flow_data_ptr, evt_status.ui_event);
    } else if (true == evt_status.usb_event.flag) {
      ENGINE_RUN_EVENT_CB(
          current_flow->usb_cb, flow_data_ptr, evt_status.usb_event);
    } else if (true == evt_status.nfc_event.event_occured) {
      ENGINE_RUN_EVENT_CB(
          current_flow->nfc_cb, flow_data_ptr, evt_status.nfc_event);
    } else {
      /* This case should never arise */
    }
  }

  return;
}