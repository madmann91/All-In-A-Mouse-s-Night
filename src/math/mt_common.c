#include "config.h"
#include "mt_common.h"

int mt_trinom(mt_scalar_t a,mt_scalar_t b,mt_scalar_t c,mt_scalar_t max,mt_scalar_t* t)
{
	mt_scalar_t d=b*b-4.0f*a*c;

	//This is just a simple trinom resolution
	if(d>(mt_scalar_t)0.0)
	{
		const mt_scalar_t root=mt_sqrt(d);
		const mt_scalar_t inv=(mt_scalar_t)1.0/((mt_scalar_t)2.0*a);
		const mt_scalar_t r1=(-b-root)*inv;
		const mt_scalar_t r2=(-b+root)*inv;
		mt_scalar_t min=(r1>0&&r1<r2)?r1:r2;

		if(min>0&&min<max)
		{
			*t=min;
			return 1;
		}
	}
	return 0;
}
