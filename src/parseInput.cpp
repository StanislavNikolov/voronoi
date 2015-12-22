#include <cstring> 	// strcmp
#include <cstdlib>	// atoi

#include "parseInput.hpp"
#include "globalVariables.hpp"

void parseInput(int argc, char** argv)
{
	// Parsing the command-line arguments
	for(unsigned i = 0;i < argc;++ i)
	{
		if(strcmp(argv[i], "--no-euclid") == 0)
			euclidian = false;
		if(strcmp(argv[i], "--progress") == 0)
			showProgress = true;
		if(strcmp(argv[i], "--dark") == 0)
			maxBrightness = atoi(argv[++ i]);
		if(strcmp(argv[i], "--clusters") == 0)
			clusterCount = atoi(argv[++ i]);
		if(strcmp(argv[i], "--width") == 0)
			imageWidth = atoi(argv[++ i]);
		if(strcmp(argv[i], "--height") == 0)
			imageHeight = atoi(argv[++ i]);
		if(strcmp(argv[i], "--threads") == 0)
			threadCount = atoi(argv[++ i]);
		if(strcmp(argv[i], "--frames") == 0)
			frameCount = atoi(argv[++ i]);
		//if(strcmp(argv[i], "--name") == 0)
			//outputName = &argv[++ i][0];
		if(strcmp(argv[i], "--no-save") == 0)
			saveImage = false;
	}
}
