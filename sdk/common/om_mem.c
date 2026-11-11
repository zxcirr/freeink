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
 * @file     om_mem.c
 * @brief    memory heap manager
 * @date     14 Mar. 2023
 * @author   OnMicro SW Team
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */


/*******************************************************************************
 * INCLUDES
 */
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "om_mem.h"


/*******************************************************************************
 * TYPEDEFS
 */
typedef struct om_mem_heap {            /* << Memory Pool management struct >>     */
    struct om_mem_heap   *next;         /* Next Memory Block in the list           */
    uint32_t              len;          /* Length of data block                    */
} om_mem_heap_t;

typedef struct {
    uint32_t base[CONFIG_MEM_NUM];
    uint32_t end[CONFIG_MEM_NUM];
} om_mem_env_t;


/*******************************************************************************
 * CONST & VARIABLES
 */
static om_mem_env_t om_mem_env;


/*******************************************************************************
 * PUBLIC FUNCTIONS
 */
void om_mem_init(void)
{
    memset((void *)(&om_mem_env), 0, sizeof(om_mem_env));
}

void om_mem_register(om_mem_type_t mem_type, __ALIGN4 void *pool, __ALIGN4 uint32_t size)
{
    om_mem_heap_t *mem;

    OM_ASSERT(mem_type < CONFIG_MEM_NUM);
    OM_ASSERT(OM_IS_ALIGN4(pool) && (pool != NULL));
    OM_ASSERT(OM_IS_ALIGN4(size) && (size != 0U));

    if ((uint32_t)pool) {
        OM_ASSERT (size > sizeof(om_mem_heap_t));

        mem = (om_mem_heap_t *)pool;
        mem->next       = (om_mem_heap_t *)((uint32_t)pool + size - sizeof(om_mem_heap_t *));
        mem->len        = 0U;
        mem->next->next = NULL;
    }

    om_mem_env.base[mem_type] = (uint32_t)pool;
    om_mem_env.end[mem_type]  = (uint32_t)pool + size;
}

void *om_mem_malloc(om_mem_type_t mem_type, uint32_t size)
{
    om_mem_heap_t *p, *p_search, *p_new;
    uint32_t hole_size;
    uint32_t pool;

    OM_ASSERT(mem_type < CONFIG_MEM_NUM);
    pool = om_mem_env.base[mem_type];
    if(!pool) {
        return NULL;
    }

    p_search = (om_mem_heap_t *)pool;
    /* Add header offset to 'size' */
    size += sizeof(om_mem_heap_t);
    /* Make sure that block is 4-byte aligned  */
    size = OM_ALIGN4_HI(size);

    OM_CRITICAL_BEGIN();
    while (1) {
        hole_size  = (uint32_t)p_search->next - (uint32_t)p_search;
        hole_size -= p_search->len;
        /* Check if hole size is big enough */
        if (hole_size >= size) {
            break;
        }
        p_search = p_search->next;
        if (p_search->next == NULL) {
            /* Failed, we are at the end of the list */
            p = NULL;
            goto _exit;
        }
    }

    if (p_search->len == 0U) {
        /* No block is allocated, set the Length of the first element */
        p_search->len = size;
        p = (om_mem_heap_t *)(((uint32_t)p_search) + sizeof(om_mem_heap_t));
    } else {
        /* Insert new list element into the memory list */
        p_new       = (om_mem_heap_t *)((uint32_t)p_search + p_search->len);
        p_new->next = p_search->next;
        p_new->len  = size;
        p_search->next = p_new;
        p = (om_mem_heap_t *)(((uint32_t)p_new) + sizeof(om_mem_heap_t));
    }

_exit:
    OM_CRITICAL_END();

    return (p);
}

void om_mem_free(om_mem_type_t mem_type, void *mem)
{
    om_mem_heap_t *p_search, *p_prev, *p_return;
    uint32_t pool;

    OM_ASSERT(mem_type < CONFIG_MEM_NUM);

    if (((uint32_t)mem <= om_mem_env.base[mem_type]) || ((uint32_t)mem >= om_mem_env.end[mem_type])) {
        return;
    }
    pool = om_mem_env.base[mem_type];
    OM_ASSERT(pool);

    OM_CRITICAL_BEGIN();
    p_return = (om_mem_heap_t *)((uint32_t)mem - sizeof(om_mem_heap_t));

    /* Set list header */
    p_prev = NULL;
    p_search = (om_mem_heap_t *)pool;
    while (p_search != p_return) {
        p_prev   = p_search;
        p_search = p_search->next;
        if (p_search == NULL) {
            /* Valid Memory block not found */
            goto _exit;
        }
    }

    if (p_prev == NULL) {
        /* First block to be released, only set length to 0 */
        p_search->len = 0U;
    } else {
        /* Discard block from chain list */
        p_prev->next = p_search->next;
    }

_exit:
    OM_CRITICAL_END();
}

void *om_mem_calloc(om_mem_type_t mem_type, uint8_t num, uint32_t size)
{
    void *mem;
    mem = om_mem_malloc(mem_type, num*size);
    if (mem != NULL) {
        memset(mem, 0, num*size);
    }

    return mem;
}

/** @} */

