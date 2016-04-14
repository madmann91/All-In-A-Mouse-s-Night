#include "profile.h"

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
counter_t frequency(void)
{
	LARGE_INTEGER i;
	QueryPerformanceFrequency(&i);
	return (counter_t)i.QuadPart;
}
counter_t timer(void)
{
	LARGE_INTEGER i;
	QueryPerformanceCounter(&i);
	return (counter_t)i.QuadPart;
}
#else
#include <time.h>
counter_t frequency(void)
{
	return CLOCKS_PER_SEC;
}
counter_t timer(void)
{
	return clock();
}
#endif

void reset_profile(profile_t* profile)
{
	profile->ticks=0;
	profile->elapsed=0;
}

void begin_profile(profile_t* profile)
{
	profile->ticks=timer();
}

void end_profile(profile_t* profile)
{
	profile->elapsed+=(timer()-profile->ticks);
}
