/**
 * @file trace_io.h
 * @brief 
 * @date Thu 29 Jan 2015 02:04:55 PM CST
 * @author liqiang
 *
 * @addtogroup 
 * @ingroup 
 * @details 
 *
 * @{
 */

#ifndef __TRC_IO_H__
#define __TRC_IO_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

#include "features.h"


/*********************************************************************
 * MACROS
 */

#ifdef CONFIG_TRACE_IO
#define TRC_IO(event, state) if(trc_io_event_callback){trc_io_event_callback(event, state);}
#else
#define TRC_IO(event, state)
#endif

/*********************************************************************
 * TYPEDEFS
 */

typedef enum
{
    // EVT
    TRC_IO_EVT_EXEC_CB,

    // EVT
    TRC_IO_EVT_TIMER_EXEC_CB,

    // Sleep
    TRC_IO_PM_IDLE,
    TRC_IO_PM_SLEEP,
    TRC_IO_PM_DEEP_SLEEP,
    TRC_IO_PM_ULTRA_SLEEP,
    TRC_IO_PM_CHECKER,

    // Number
    TRC_IO_EVENT_NUM,
}trc_io_event_t;

typedef void (* trc_io_event_callback_t)(trc_io_event_t event, int is_on);

/*********************************************************************
 * EXTERN VARIABLES
 */
extern trc_io_event_callback_t trc_io_event_callback;

/*********************************************************************
 * EXTERN FUNCTIONS
 */
/**
 *******************************************************************************
 * @brief  trc io set
 *
 * @param[in] event  event
 * @param[in] pin  pin
 *******************************************************************************
 */
void trc_io_set(trc_io_event_t event, uint8_t pin);

#ifdef __cplusplus
}
#endif

#endif

/** @} */

