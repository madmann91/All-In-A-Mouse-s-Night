#include "mt_primes.h"

const unsigned int mt_primes[MT_PRIMES_COUNT]={5,11,23,47,97,197,397,797,1597,3203,6421,12853,25717,51437,
102877,205759,411527,823117,1646237,3292489,6584983,13169977,26339969,52679969,105359939,210719881,
421439783,842879579,1685759167};

unsigned int mt_nextprime(unsigned int n)
{
	unsigned int bounds[2]={0,MT_PRIMES_COUNT-1};
	unsigned int m;

	//Binary search
	while(bounds[1]-bounds[0]>1)
	{
		m=(bounds[0]+bounds[1])/2;
		if(mt_primes[m]>n)
			bounds[1]=m;
		else
			bounds[0]=m;
	}

	return mt_primes[bounds[1]];
}

unsigned int mt_previousprime(unsigned int n)
{
	unsigned int bounds[2]={0,MT_PRIMES_COUNT-1};
	unsigned int m;

	while(bounds[1]-bounds[0]>1)
	{
		m=(bounds[0]+bounds[1])/2;
		if(mt_primes[m]>n)
			bounds[1]=m;
		else
			bounds[0]=m;
	}

	//Return the lower bound
	return mt_primes[bounds[0]];
}
