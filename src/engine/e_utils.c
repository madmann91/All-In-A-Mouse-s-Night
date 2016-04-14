#include "config.h"
#include "e_utils.h"

void e_wait(unsigned int delay)
{
	SDL_Delay(delay);
}

unsigned long e_getticks(void)
{
	return SDL_GetTicks();
}

void e_start(e_timer_t* timer)
{
	timer->stopped=0;
	timer->ticks=e_getticks();
}

void e_stop(e_timer_t* timer)
{
	if(!timer->stopped)
	{
		timer->stopped=1;
		timer->elapsed+=e_getticks()-timer->ticks;
	}
}

void e_restart(e_timer_t* timer)
{
	const unsigned long ticks=e_getticks();

	if(!timer->stopped)
		timer->elapsed+=ticks-timer->ticks;

	timer->stopped=0;
	timer->ticks=ticks;
}

unsigned int e_pow2(unsigned int a)
{
	int b;
	if(frexpf((float)a,&b)!=0.5f)
		b=1<<b;
	else
		b=a;
	return (unsigned int)b;
}

unsigned int e_upow2(unsigned int a)
{
	int b;
	if(frexpf((float)a,&b)!=0.5f)
		b=1<<(b-1);
	else
		b=a;
	return (unsigned int)b;
}
