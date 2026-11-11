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
 * @file     om_kfifo.h
 * @brief    kfifo, fast but size must be power of 2
 * @date     06. Aug 2020
 * @author   OnMicro SW Team
 *
 * @defgroup OM_KFIFO Kfifo
 * @ingroup  COMMON
 * @brief    Kfifo
 * @details  Kfifo
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */

#ifndef __OM_KFIFO_H
#define __OM_KFIFO_H

#ifdef __cplusplus
extern "C"
{
#endif

/// @cond

/*******************************************************************************
 * INCLUDES
 */
#include <stdint.h>
#include <string.h>
#include "om_device.h"
#include "om_utils.h"

/*******************************************************************************
 * MACROS
 */
/// kfifo's SIZE must be power of 2 for intercepting IN and OUT into range SIZE,
/// CANNOT substitude % for &, because IN,OUT can overflow!!!
#define __OM_KFIFO_IS_POWER_OF_2(x)    ((x) != 0 && (((x) & ((x) - 1)) == 0))


/*******************************************************************************
 * TYPEDEFS
 */
/// Kfifo struct
typedef struct
{
    /// buffer
    unsigned char *buffer;
    /// buffer total size
    unsigned int size;
    /// in position
    volatile unsigned int in;
    /// out position
    volatile unsigned int out;
}om_kfifo_t;

/*******************************************************************************
 * INSIDE FUNCTIONS
 */

/**
 *******************************************************************************
 * @brief  om fifo min
 *
 * @param[in] a  a
 * @param[in] b  b
 *
 * @return min
 *******************************************************************************
 **/
__STATIC_FORCEINLINE unsigned int __om_kfifo_min(unsigned int a, unsigned int b)
{
    return (a < b) ? a : b;
}

/**
 *******************************************************************************
 * @brief  fifo offset
 *
 * @param[in] fifo  fifo
 * @param[in] off  offset
 *
 * @return real offset
 *******************************************************************************
 **/
__STATIC_FORCEINLINE unsigned int __om_kfifo_off(om_kfifo_t *fifo, unsigned int off)
{
    return off & (fifo->size - 1);
}

/**
 *******************************************************************************
 * @brief  fifo in data
 *
 * @param[in] fifo  fifo
 * @param[in] from  from
 * @param[in] len  len
 * @param[in] off  off
 *******************************************************************************
 **/
__STATIC_INLINE void __om_kfifo_in_data(om_kfifo_t *fifo, const unsigned char *from, unsigned int len, unsigned int off)
{
    unsigned int l;

    off = __om_kfifo_off(fifo, fifo->in + off);

    l = __om_kfifo_min(len, fifo->size - off);
    memcpy(fifo->buffer + off, from, l);

    memcpy(fifo->buffer, from + l, len - l);
}

/**
 *******************************************************************************
 * @brief  om fifo in data 1byte
 *
 * @param[in] fifo  fifo
 * @param[in] from  from
 *******************************************************************************
 **/
__STATIC_FORCEINLINE void __om_kfifo_in_data_1byte(om_kfifo_t *fifo, const unsigned char *from)
{
    unsigned int off = __om_kfifo_off(fifo, fifo->in);

    *(fifo->buffer + off) = *from;
}

/**
 *******************************************************************************
 * @brief  om fifo out data
 *
 * @param[in] fifo  fifo
 * @param[in] to  to
 * @param[in] len  len
 * @param[in] off  off
 *******************************************************************************
 **/
__STATIC_INLINE void __om_kfifo_out_data(om_kfifo_t *fifo, unsigned char *to, unsigned int len, unsigned int off)
{
    unsigned int l;

    off = __om_kfifo_off(fifo, fifo->out + off);

    l = __om_kfifo_min(len, fifo->size - off);
    memcpy(to, fifo->buffer + off, l);

    memcpy(to + l, fifo->buffer, len - l);
}

/**
 *******************************************************************************
 * @brief  co fifo out data 1byte
 *
 * @param[in] fifo  fifo
 * @param[in] to  to data
 *******************************************************************************
 **/
__STATIC_FORCEINLINE void __om_kfifo_out_data_1byte(om_kfifo_t *fifo, unsigned char *to)
{
    unsigned int off = __om_kfifo_off(fifo, fifo->out);

    *to = *(fifo->buffer + off);
}

/**
 *******************************************************************************
 * @brief  om fifo add out
 *
 * @param[in] fifo  fifo
 * @param[in] off  offset
 *******************************************************************************
 **/
__STATIC_FORCEINLINE void __om_kfifo_add_out(om_kfifo_t *fifo, unsigned int off)
{
    fifo->out += off;
}

/**
 *******************************************************************************
 * @brief  om fifo add in
 *
 * @param[in] fifo  fifo
 * @param[in] off  offset
 *******************************************************************************
 **/
__STATIC_FORCEINLINE void __om_kfifo_add_in(om_kfifo_t *fifo, unsigned int off)
{
    fifo->in += off;
}

/// @endcond

/*********************************************************************
 * EXTERN FUNCTIONS
 */

/**
 *******************************************************************************
 * @brief fifo reset
 *
 * @param[in] fifo  fifo object
 *******************************************************************************
 **/
__STATIC_FORCEINLINE void om_kfifo_reset(om_kfifo_t *fifo)
{
    fifo->in = fifo->out = 0;
}

/**
 *******************************************************************************
 * @brief get fifo size
 *
 * @param[in] fifo  fifo object
 *
 * @return size of fifo
 *******************************************************************************
 **/
__STATIC_FORCEINLINE unsigned int om_kfifo_size(om_kfifo_t *fifo)
{
    return fifo->size;
}

/**
 *******************************************************************************
 * @brief get fifo length
 *
 * @param[in] fifo  fifo object
 *
 * @return length of fifo
 *******************************************************************************
 **/
__STATIC_FORCEINLINE unsigned int om_kfifo_len(om_kfifo_t *fifo)
{
    register unsigned int out;
    out = fifo->out;
    return fifo->in - out;
}

/**
 *******************************************************************************
 * @brief whether is fifo empty
 *
 * @param[in] fifo  fifo object
 *
 * @return empty?
 *******************************************************************************
 **/
__STATIC_FORCEINLINE int om_kfifo_is_empty(om_kfifo_t *fifo)
{
    return fifo->in == fifo->out;
}

/**
 *******************************************************************************
 * @brief whether is fifo full
 *
 * @param[in] fifo  fifo object
 *
 * @return full?
 *******************************************************************************
 **/
__STATIC_FORCEINLINE int om_kfifo_is_full(om_kfifo_t *fifo)
{
    return om_kfifo_len(fifo) == om_kfifo_size(fifo);
}

/**
 *******************************************************************************
 * @brief get fifo available length
 *
 * @param[in] fifo  fifo object
 *
 * @return available length
 *******************************************************************************
 **/
__STATIC_FORCEINLINE unsigned int om_kfifo_avail(om_kfifo_t *fifo)
{
    return om_kfifo_size(fifo) - om_kfifo_len(fifo);
}

/**
 *******************************************************************************
 * @brief fifo init
 *
 * @param[in] fifo  fifo object
 * @param[in] buffer  fifo buffer
 * @param[in] size  fifo size
 *******************************************************************************
 **/
__STATIC_INLINE void om_kfifo_init(om_kfifo_t *fifo, unsigned char *buffer, unsigned int size)
{
    OM_ASSERT(__OM_KFIFO_IS_POWER_OF_2(size));

    fifo->buffer = buffer;
    fifo->size = size;

    om_kfifo_reset(fifo);
}

/**
 *******************************************************************************
 * @brief put data to fifo
 *
 * @param[in] fifo  fifo object
 * @param[in] from  data buffer
 * @param[in] len  data buffer length
 *
 * @return putted length
 *******************************************************************************
 **/
__STATIC_INLINE unsigned int om_kfifo_in(om_kfifo_t *fifo, const unsigned char *from, unsigned int len)
{
    len = __om_kfifo_min(om_kfifo_avail(fifo), len);

    __om_kfifo_in_data(fifo, from, len, 0);
    __om_kfifo_add_in(fifo, len);

    return len;
}

/**
 *******************************************************************************
 * @brief  co fifo in 1byte
 *
 * @param[in] fifo  fifo
 * @param[in] from  from
 *
 * @return putted length
 *******************************************************************************
 **/
__STATIC_FORCEINLINE unsigned int om_kfifo_in_1byte(om_kfifo_t *fifo, const unsigned char *from)
{
    if (!om_kfifo_avail(fifo)) return 0;

    __om_kfifo_in_data_1byte(fifo, from);
    __om_kfifo_add_in(fifo, 1);

    return 1;
}

/**
 *******************************************************************************
 * @brief obtain data from fifo
 *
 * @param[in] fifo  fifo object
 * @param[in] to  data buffer
 * @param[in] len  data buffer length
 *
 * @return obtained length
 *******************************************************************************
 **/
__STATIC_INLINE unsigned int om_kfifo_out(om_kfifo_t *fifo, unsigned char *to, unsigned int len)
{
    len = __om_kfifo_min(om_kfifo_len(fifo), len);

    __om_kfifo_out_data(fifo, to, len, 0);
    __om_kfifo_add_out(fifo, len);

    return len;
}

/**
 *******************************************************************************
 * @brief  co fifo out 1byte
 *
 * @param[in] fifo  fifo
 * @param[in] to  to
 *
 * @return obtained length
 *******************************************************************************
 **/
__STATIC_FORCEINLINE unsigned int om_kfifo_out_1byte(om_kfifo_t *fifo, unsigned char *to)
{
    if (om_kfifo_is_empty(fifo)) return 0;

    __om_kfifo_out_data_1byte(fifo, to);
    __om_kfifo_add_out(fifo, 1);

    return 1;
}

/**
 *******************************************************************************
 * @brief peek data from fifo
 *
 * @param[in] fifo  fifo object
 * @param[in] to  data buffer
 * @param[in] len  data buffer length
 *
 * @return peeked length
 *******************************************************************************
 **/
__STATIC_INLINE unsigned int om_kfifo_peek(om_kfifo_t *fifo, unsigned char *to, unsigned int len)
{
    len = __om_kfifo_min(om_kfifo_len(fifo), len);

    __om_kfifo_out_data(fifo, to, len, 0);

    return len;
}

/**
 *******************************************************************************
 * @brief peek data from fifo with offset
 *
 * @param[in] fifo  fifo object
 * @param[in] to  data buffer
 * @param[in] len  data buffer length
 * @param[in] offset  buffer offset
 *
 * @return peeked length
 *******************************************************************************
 **/
__STATIC_INLINE unsigned int om_kfifo_peek_ex(om_kfifo_t *fifo, unsigned char *to, unsigned int len, unsigned int offset)
{
    len = __om_kfifo_min(om_kfifo_len(fifo), len + offset);

    __om_kfifo_out_data(fifo, to, len, offset);

    return len;
}

#ifdef __cplusplus
}
#endif

#endif

/** @} */


