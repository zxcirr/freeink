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
 * @file     om_utils.h
 * @brief
 * @date     06. Aug 2020
 * @author   OnMicro SW Team
 *
 * @defgroup OM_UTILS Utils
 * @ingroup  COMMON
 * @brief    Helper functions
 * @details  Helper functions
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */

#ifndef __OM_UTIL_H__
#define __OM_UTIL_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "om_device.h"
#include "om_log.h"

/*********************************************************************
 * MACROS
 */
/// MAX
#define OM_MAX(x,y)                            (((x) > (y)) ? (x) : (y))
/// MIN
#define OM_MIN(x,y)                            (((x) < (y)) ? (x) : (y))

/// count of
#define OM_COUNT_OF(s)                         (sizeof(s)/sizeof((s)[0]))
/// offset of
#define OM_OFFSET_OF(type, member)             ((unsigned int) &((type *)0)->member)
/// container of
#define OM_CONTAINER_OF(ptr, type, member)     ((type *)((char *)(ptr) - OM_OFFSET_OF(type, member)))

/**
 *******************************************************************************
 * @brief  The OM_ASSERT macro is used for function's parameters check.
 * @param  expr: If expr is false, it calls drv_assert_failed function
 *         which reports the name of the source file and the source
 *         line number of the call that failed.
 *         If expr is true, it returns no value.
 * @return None
 *******************************************************************************
 */
/* Assert ------------------------------------------------------------------- */
#ifdef CONFIG_ASSERT
#define OM_ASSERT(expr)  do {                                                  \
        if(!(unsigned)(expr)) {                                                \
            while(1);                                                          \
        }                                                                      \
    } while(0)

/// assert with while
#define OM_ASSERT_WHILE(cond, expr)     do {                                   \
        if((unsigned)(cond)) {                                                 \
            if(!(unsigned)(expr)) {                                            \
                OM_LOG(OM_LOG_ERROR, "Error:%s:%d\n", __FILE__, __LINE__);     \
                while(1);                                                      \
            }                                                                  \
        }                                                                      \
    } while(0)

/// assert with show warning
#define OM_ASSERT_WARNING(expr)  do {                                          \
        if(!(unsigned)(expr)) {                                                \
            OM_LOG(OM_LOG_WARN, "WARNING:%s:%d\n", __FILE__, __LINE__);        \
        }                                                                      \
    } while(0)

#else
#define OM_ASSERT(expr)                 ((void)0U)
#define OM_ASSERT_WHILE(cond, expr)     ((void)0U)
#define OM_ASSERT_WARNING(expr)         ((void)0U)
#endif  /* CONFIG_ASSERT */

/// Compile Time Asserts
#define OM_STATIC_ASSERT(exp)           extern char om_static_assert_failed[(exp)?1:-1]

/**
 *******************************************************************************
 *  @brief Align val on the multiple of align equal or nearest higher. align shall in [2, 4, 8, ... 2**n]
 *
 *  @param[in]   val          Value to align.
 *  @param[in]   align        Align
 *
 *  @return:                  Aligned Value
 *******************************************************************************
 */
#define OM_ALIGN_HI(val, align)    (((unsigned)((val) + ((align) - 1))) & (~((align) - 1)))

/**
 *******************************************************************************
 *  @brief Align val on the multiple of align equal or nearest lower. align shall in [2, 4, 8, ... 2**n]
 *
 *  @param[in]   val          Value to align.
 *  @param[in]   align        Align
 *
 *  @return                   Value aligned.
 *******************************************************************************
 */
#define OM_ALIGN_LO(val, align)    ((unsigned)(val) & (~((align) - 1)))

/**
 *******************************************************************************
 *  @brief Align val on the multiple of 32 equal or nearest higher.
 *
 *  @param[in]   val    Value to align.
 *
 *  @return             Value aligned.
 *******************************************************************************
 */
#define OM_ALIGN32_HI(val) (((unsigned)(val)+31)&(~31))

/**
 *******************************************************************************
 *  @brief Align val on the multiple of 32 equal or nearest lower.
 *
 *  @param[in]   val    Value to align.
 *
 *  @return             Value aligned.
 *******************************************************************************
 */
#define OM_ALIGN32_LO(val) ((unsigned)(val)&(~31))

/**
 *******************************************************************************
 *  @brief Align val on the multiple of 8 equal or nearest higher.
 *
 *  @param[in]   val    Value to align.
 *
 *  @return             Value aligned.
 *******************************************************************************
 */
#define OM_ALIGN8_HI(val) (((unsigned)(val)+7)&(~7))

/**
 *******************************************************************************
 *  @brief Align val on the multiple of 8 equal or nearest lower.
 *
 *  @param[in]   val    Value to align.
 *
 *  @return             Value aligned.
 *******************************************************************************
 */
#define OM_ALIGN8_LO(val) ((unsigned)(val)&(~7))

/**
 *******************************************************************************
 *  @brief Align val on the multiple of 4 equal or nearest higher.
 *
 *  @param[in]   val    Value to align.
 *
 *  @return             Value aligned.
 *******************************************************************************
 */
#define OM_ALIGN4_HI(val) (((unsigned)(val)+3)&(~3))

/**
 *******************************************************************************
 *  @brief Align val on the multiple of 4 equal or nearest lower.
 *
 *  @param[in]   val    Value to align.
 *
 *  @return             Value aligned.
 *******************************************************************************
 */
#define OM_ALIGN4_LO(val) ((unsigned)(val)&(~3))


/**
 *******************************************************************************
 *  @brief Align val on the multiple of 2 equal or nearest higher.
 *
 *  @param[in]   val    Value to align.
 *
 *  @return             Value aligned.
 *******************************************************************************
 */
#define OM_ALIGN2_HI(val) (((unsigned)(val)+1)&(~1))

/**
 *******************************************************************************
 *  @brief Align val on the multiple of 2 equal or nearest lower.
 *
 *  @param[in]   val    Value to align.
 *
 *  @return             Value aligned.
 *******************************************************************************
 */
#define OM_ALIGN2_LO(val) ((unsigned)(val)&(~1))


/**
 *******************************************************************************
 *  @brief Check val is align to the power of 2
 *
 *  @param[in]   val     Value to align.
 *  @param[in]   align   Align value, align range in [2, 4, 8, ...2**31, 0]
 *
 *  @return              is aligned.
 *******************************************************************************
 */
#define OM_IS_ALIGN(val, align)   (!((unsigned)(val) & ((unsigned)(align) - 1)))

/// Check val is align to 16 Bytes.
#define OM_IS_ALIGN16(val)   OM_IS_ALIGN(val, 16)

/// Check val is align to 8 Bytes.
#define OM_IS_ALIGN8(val)    OM_IS_ALIGN(val, 8)

/// Check val is align to 4 Bytes.
#define OM_IS_ALIGN4(val)    OM_IS_ALIGN(val, 4)

/// Check val is align to 2 Bytes.
#define OM_IS_ALIGN2(val)    OM_IS_ALIGN(val, 2)

/* attribute description in function/marcro define -------------------------- */
/* used for specify function/macro parameter as an IN parameter */
#ifdef __IN
#error "redefine __IN"
#else
#define __IN
#endif

/* used for specify function/macro parameter as an OUT parameter */
#ifdef __OUT
#error "redefine __OUT"
#else
#define __OUT
#endif

/* used for specify function/macro parameter as an INOUT parameter */
#ifdef __INOUT
#error "redefine __INOUT"
#else
#define __INOUT
#endif

/* used for specify function/macro parameter need ALIGN to 2 Bytes */
#ifdef __ALIGN2
#error "redefine __ALIGN2"
#else
#define __ALIGN2
#endif

/* used for specify function/macro parameter need ALIGN to 4 Bytes */
#ifdef __ALIGN4
#error "redefine __ALIGN4"
#else
#define __ALIGN4
#endif


/* Assert ------------------------------------------------------------------- */



/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * EXTERN VARIABLES
 */


/*********************************************************************
 * EXTERN FUNCTIONS
 */
/**
 *******************************************************************************
 * @brief  om crc16 ccitt
 *
 * @param[in] crcinit  crcinit
 * @param[in] buf  buf
 * @param[in] len  len
 *
 * @return crc16
 *******************************************************************************
 **/
extern uint16_t om_crc16_ccitt(uint16_t crcinit, const void *buf, unsigned len);

/**
 *******************************************************************************
 * @brief  om array reversal
 *
 * @param[in] array  array
 * @param[in] len  len
 *******************************************************************************
 */
extern void om_array_reversal(void *array, unsigned len);

#ifdef __cplusplus
}
#endif

#endif

/** @} */

