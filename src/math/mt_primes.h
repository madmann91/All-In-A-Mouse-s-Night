/***********************************************************
*                                                          *
*  File : mt_prime.h                                       *
*  Date : 20/02/2011                                       *
*  Comments : Prime number list                            *
*                                                          *
************************************************************/

#ifndef __TB_PRIME_H__
#define __TB_PRIME_H__

#include "mt_common.h"

/*! \addtogroup mt */
/*! @{ */

/*! \name Prime */
/*! @{ */

#define MT_PRIMES_COUNT 29
/*! \brief Contains a list of primes, starting with 5 (each number is almost the double of the previous one) */
extern const unsigned int mt_primes[MT_PRIMES_COUNT];
/*! \brief Returns the prime number in the list above the given value */
unsigned int mt_nextprime(unsigned int);
/*! \brief Returns the prime number in the list under the given value */
unsigned int mt_previousprime(unsigned int);

/*! @} */

/*! @} */

#endif
