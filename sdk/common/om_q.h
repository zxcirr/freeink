/* ----------------------------------------------------------------------------
 * Copyright (c) 2020-2030 OnMicro Limited. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of OnMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * -------------------------------------------------------------------------- */

/**
 * @file     om_q.h
 * @brief
 * @date     06. Aug 2020
 * @author   OnMicro SW Team
 *
 * @defgroup OM_QUEUE Queue
 * @ingroup  COMMON
 * @brief    Queue
 * @details  Queue
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */

#ifndef __OM_Q_H__
#define __OM_Q_H__


/*******************************************************************************
 * INCLUDES
 */
#include "om_list.h"
#include "om_device.h"

#ifdef __cplusplus
extern "C"
{
#endif


/*******************************************************************************
 * MACROS
 */


/*******************************************************************************
 * TYPEDEFS
 */
/// q is list
typedef om_list_t om_q_t;
/// qnode is list_node
typedef om_list_node_t om_q_node_t;


/*******************************************************************************
 * EXTERN VARIABLES
 */


/*******************************************************************************
 * INLINE FUNCTIONS
 */
/**
 *******************************************************************************
 * @brief queue initialize
 *
 * @param[in] queue  queue
 *******************************************************************************
 */
__STATIC_INLINE void om_q_init(om_q_t *queue)
{
    om_list_init((om_list_t *)queue);
}

/**
 *******************************************************************************
 * @brief push a node to queue's front
 *
 * @param[in] new_node    new node
 * @param[in] queue       queue
 *******************************************************************************
 */
__STATIC_INLINE void om_q_push_front(om_q_node_t *new_node, om_q_t *queue)
{
    om_list_push_front((om_list_node_t *)new_node, (om_list_t *)queue);
}

/**
 *******************************************************************************
 * @brief push a node to queue's end
 *
 * @param[in] new_node    new node
 * @param[in] queue       queue
 *******************************************************************************
 */
__STATIC_INLINE void om_q_push(om_q_node_t *new_node, om_q_t *queue)
{
    om_list_push((om_list_node_t *)new_node, (om_list_t *)queue);
}

/**
 *******************************************************************************
 * @brief whether is empty
 *
 * @param[in] queue    queue
 *
 * @return
 *    - true:  queue is empty
 *    - false: queue is not empty
 *******************************************************************************
 */
__STATIC_INLINE bool om_q_is_empty(const om_q_t *queue)
{
    return om_list_is_empty((om_list_t *)queue);
}

/**
 *******************************************************************************
 * @brief peek node from front
 *
 * @param[in] queue    queue
 *
 * @return
 *    - NULL: no item
 *    - NOT NULL: the first item
 *******************************************************************************
 */
__STATIC_INLINE om_q_node_t *om_q_peek(om_q_t *queue)
{
    return (om_q_node_t *)om_list_get_first_node((om_list_t *)queue);
}

/**
 *******************************************************************************
 * @brief peek node from behind
 *
 * @param[in] queue    queue
 *
 * @return
 *    - NULL: no item
 *    - NOT NULL: the last item
 *******************************************************************************
 */
__STATIC_INLINE om_q_node_t *om_q_peek_behind(om_q_t *queue)
{
    return (om_q_node_t *)om_list_get_last_node((om_list_t *)queue);
}

/**
 *******************************************************************************
 * @brief pop item from the front
 *
 * @param[in] queue    queue
 *
 * @return
 *    - NULL: no item
 *    - NOT NULL: the item
 *******************************************************************************
 */
__STATIC_INLINE om_q_node_t *om_q_pop(om_q_t *queue)
{
    return (om_q_node_t *)om_list_pop((om_list_t *)queue);
}

/**
 *******************************************************************************
 * @brief pop item at the end
 *
 * @param[in] queue    queue
 *
 * @return
 *    - NULL: no item
 *    - NOT NULL: the item
 *******************************************************************************
 */
__STATIC_INLINE om_q_node_t *om_q_pop_behind(om_q_t *queue)
{
    return (om_q_node_t *)om_list_pop_behind((om_list_t *)queue);
}

/**
 *******************************************************************************
 * @brief move node from old_queue to new_queue
 *
 * @param[in] node         node
 * @param[in] old_queue    old queue
 * @param[in] new_queue    new queue
 *******************************************************************************
 */
__STATIC_INLINE void om_q_move(om_q_node_t *node, om_q_t *old_queue, om_q_t *new_queue)
{
    om_list_move((om_list_node_t *)node, (om_list_t *)old_queue, (om_list_t *)new_queue);
}


#ifdef __cplusplus
}
#endif

#endif  /* __CODE_TEMPLETE_H */


/** @} */
