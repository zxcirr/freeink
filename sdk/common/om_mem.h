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
 * @file     om_mem.h
 * @brief    memory heap manager
 * @date     14 Mar. 2024
 * @author   OnMicro SW Team
 *
 * @defgroup DOC DOC
 * @ingroup  DOCUMENT
 * @brief    memory heap manager
 * @details  memory heap manager
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */

#ifndef __OM_MEM_H
#define __OM_MEM_H


/*******************************************************************************
 * INCLUDES
 */
#include <stdint.h>
#include <stddef.h>
#include "om_driver.h"
#include "features.h"


#ifdef __cplusplus
extern "C"
{
#endif


/*******************************************************************************
 * MACROS
 */
#ifndef CONFIG_MEM_NUM
    #define CONFIG_MEM_NUM      1U
#elif (CONFIG_MEM_NUM == 0)
    #error "CONFIG_MEM_NUM can not be zero"
#endif

// Memory Region Index
#define OM_MEM(n)               (n)


/*******************************************************************************
 * TYPEDEFS
 */
typedef uint8_t om_mem_type_t;


/*******************************************************************************
 * EXTERN FUNCTIONS
 */
/**
 *******************************************************************************
 * @brief  Initialize memory block
 *******************************************************************************
 */
extern void om_mem_init(void);

/**
 *******************************************************************************
 * @brief  Register memory block to the memory type
 *
 * @param[in] mem_type    Memory type
 * @param[in] pool        Memory pool, need align to 4 Byte
 * @param[in] size        Size of the memory block, in bytes, need to align to 4 Byte
 *******************************************************************************
 */
extern void om_mem_register(om_mem_type_t mem_type, __ALIGN4 void *pool, __ALIGN4 uint32_t size);

/**
 *******************************************************************************
 * @brief  Allocates a block of size bytes from the memory type
 *
 * @param[in] mem_type    Memory type
 * @param[in] size        Size of the memory block, in bytes
 *
 * @return                Return a pointer to the beginning of the block
 *******************************************************************************
 */
extern void *om_mem_malloc(om_mem_type_t mem_type, uint32_t size);

/**
 *******************************************************************************
 * @brief  Deallocate memory block
 *
 * @param[in] mem_type    Memory type
 * @param[in] mem         Pointer to a memory block previously allocated with malloc/calloc
 *******************************************************************************
 */
extern void om_mem_free(om_mem_type_t mem_type, void *mem);

/**
 *******************************************************************************
 * @brief  Callocate memory block
 *
 * @param[in] mem_type    Memory type
 * @param[in] num         Number of chunk
 * @param[in] size        size of chunk
 *
 * @return                Return a pointer to the beginning of the block
 *******************************************************************************
 */
extern void *om_mem_calloc(om_mem_type_t mem_type, uint8_t num, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif  /* __OM_MEM_H */


/** @} */

