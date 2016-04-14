#ifndef __PROFILE_H__
#define __PROFILE_H__

#ifdef WIN32
typedef __int64 counter_t;
#else
typedef unsigned long counter_t;
#endif

typedef struct profile_s
{
	counter_t elapsed;
	counter_t ticks;
}profile_t;

counter_t frequency(void);
counter_t timer(void);
void reset_profile(profile_t*);
void begin_profile(profile_t*);
void end_profile(profile_t*);

#endif
