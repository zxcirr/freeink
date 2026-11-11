/**
 * @file features.h
 * @brief
 * @date Sat 14 Feb 2015 09:55:02 AM CST
 * @author ONMICRO SW TEAM
 *
 * @addtogroup
 * @ingroup
 * @details
 *
 * @{
 */

#ifndef __FEATURES_H__
#define __FEATURES_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

// Version defined
#ifdef CONFIG_AUTOCONF_PRESET
#include "autoconf_preset.h"

// default
#else
#include "autoconf.h"

#endif

/*********************************************************************
 * MACROS
 */

#if defined(CONFIG_OM6626A)
#define CONFIG_ROM_ID (0x6626A000 | (CONFIG_HARDWARE_VERSION << 8) | (0 << 0))
#else
#error Donnot supported device
#endif

/*********************************************************************
 * REDEFINE MACROS
 */


/*********************************************************************
 * TYPEDEFS
 */


/*********************************************************************
 * EXTERN VARIABLES
 */


/*********************************************************************
 * EXTERN FUNCTIONS
 */


#ifdef __cplusplus
}
#endif

#endif

/** @} */
