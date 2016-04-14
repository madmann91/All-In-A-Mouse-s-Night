#include "e_common.h"

int e_checkfiletype(const e_filetype_t* a,const e_filetype_t* b)
{
	return (a->magic==b->magic)&&(a->version==b->version);
}
