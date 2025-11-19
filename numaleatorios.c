#include <stdio.h>
#include "mtwister.h"

int main()
{
	MTRand r = seedRand(1234567);
	int i;
	for(i=0; i<1000; i++){
		printf("%c\n", (unsigned char) (genRandLong(&r) % (126 - 33) + 33));
	}
	return 0;
}