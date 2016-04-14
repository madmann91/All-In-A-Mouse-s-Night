/***********************************************************
*                                                          *
*  File : e_utils.h                                        *
*  Date : 13/11/2009                                       *
*  Comments : Utility functions                            *
*                                                          *
************************************************************/

#ifndef __E_UTILS_H__
#define __E_UTILS_H__

#include "e_common.h"

/*! \addtogroup e */
/*! @{ */

struct e_timer_s
{
	int stopped:1;
	unsigned long ticks;
	unsigned long elapsed;
};

typedef struct e_timer_s e_timer_t;

/* \brief Computes the closest power of two */
unsigned int e_pow2(unsigned int);
/* \brief Computes the closest power of two under the given value */
unsigned int e_upow2(unsigned int);
/* \brief Waits during an amout of time in milliseconds */
void e_wait(unsigned int);
/* \brief Gets the number of milliseconds elapsed since the system was started */
unsigned long e_getticks(void);
/* \brief Starts a timer */
void e_start(e_timer_t*);
/* \brief Stops a timer */
void e_stop(e_timer_t*);
/* \brief Restarts a timer */
void e_restart(e_timer_t*);

/*! @} */

#endif
