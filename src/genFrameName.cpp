#include <cstring>

#include "genFrameName.hpp"

char* genFrameName(char* baseName, unsigned id)
{
	unsigned baseNameSize = strlen(baseName);
	char* output = new char[baseNameSize + 10];
	output[baseNameSize + 9] = '\0';

	unsigned i;
	for(i = 0;baseName[i] != '\0';++ i)
		output[i] = baseName[i];

	for(i = 8;i < (unsigned)(-1);-- i)
	{
		output[baseNameSize + i] = (id % 10) + '0';
		id /= 10;
	}

	return output;
}
