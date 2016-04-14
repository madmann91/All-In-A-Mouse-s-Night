/***********************************************************
*                                                          *
*  File : mt_common.h                                      *
*  Date : 13/11/2009                                       *
*  Comments : Common mathematics defines                   *
*                                                          *
************************************************************/

#ifndef __MT_COMMON_H__
#define __MT_COMMON_H__

/*! \addtogroup mt Mathematics module */
/*! @{ */

#ifndef MT_DOUBLE
typedef float mt_scalar_t;
#define mt_sqrt sqrtf
#define mt_cos cosf
#define mt_sin sinf
#define mt_fabs fabsf
#define mt_log logf
#define mt_tan tanf
#define mt_atan atanf
#define mt_acos acosf
#define mt_fmod fmodf
#define mt_floor floorf
#define mt_ceil ceilf
#define MT_PI 3.14159265358979f
#define MT_LN2 0.69314718055994f
#define MT_LN8 2.07944154167983f
#define MT_EPSILON 1.192092896e-07f
#define MT_MINIMUM 1.175494351e-38f
#define MT_MAXIMUM 3.402823466e+38f
#else
typedef double mt_scalar_t;
#define mt_sqrt sqrt
#define mt_cos cos
#define mt_sin sin
#define mt_fabs fabs
#define mt_log log
#define mt_tan tan
#define mt_atan atan
#define mt_acos acos
#define mt_fmod fmod
#define mt_floor floor
#define mt_ceil ceil
#define MT_PI 3.14159265358979323846
#define MT_LN2 0.693147180559945309417
#define MT_LN8 2.079441541679835928251
#define MT_EPSILON 2.2204460492503131e-016
#define MT_MINIMUM 2.2250738585072014e-308
#define MT_MAXIMUM 1.7976931348623158e+308
#endif

#define mt_swap(a,b) a^=b;\
					b^=a;\
					a^=b;

/*!
  \brief Solves a trinom : a*x^2+b*x+c=0
  \param[in] a See description
  \param[in] b See description
  \param[in] c See description
  \param[in] max The maximum value for x (minimum is 0)
  \param[out] x A solution of the trinom (with 0<x<max)
  \return 1 is there's at least one valid solution, otherwise 0
*/
int mt_trinom(mt_scalar_t a,mt_scalar_t b,mt_scalar_t c,mt_scalar_t max,mt_scalar_t* x);

/*! @} */

#endif
