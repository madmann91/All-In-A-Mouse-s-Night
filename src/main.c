#include "config.h"
#include "g_common.h"

int main(int argc,char** argv)
{
	if(!g_init())
		return 1;
	g_run();
	g_quit();
	return 0;
}
