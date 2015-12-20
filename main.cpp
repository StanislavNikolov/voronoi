#include <random> 	// random_device...
#include <cmath> 	// distance(sqrt)
#include <cstring> 	// strcmp
#include <thread> 	// threads
#include <cstdlib>	// atoi
#include <png++/png.hpp>

// Make a pseudo-random engine seeded by a "true" random number
std::random_device randomDevice;
std::mt19937 prandomEngine(randomDevice());

unsigned clusterCount 		= 400;
unsigned threadCount 		= 2;
unsigned imageWidth 		= 1920;
unsigned imageHeight 		= 1080;

bool euclidian 				= true;
bool showProgress 			= false;
bool saveImage 				= true;
unsigned maxBrightness 		= 255;

char DEFAULT_NAME[] 		= "output.png";

struct Cluster {
	unsigned x, y, c;
};

Cluster* clusters;

double distance(unsigned ax, unsigned ay, unsigned bx, unsigned by)
{
	if(euclidian)
		return sqrt((ax-bx)*(ax-bx) + (ay-by)*(ay-by));
	else
		return (std::max(ax, bx) - std::min(ax, bx)) + (std::max(ay, by) - std::min(ay, by));
}

unsigned closestCluster(unsigned x, unsigned y)
{
	unsigned output = 0;
	double min = distance(x, y, clusters[0].x, clusters[0].y);
	for(unsigned idx = 1;idx < clusterCount;++ idx)
	{
		unsigned d = distance(x, y, clusters[idx].x, clusters[idx].y);
		if(d < min)
		{
			min = d;
			output = idx;
		}
	}
	return output;
}

void renderRow(png::gray_pixel* data, unsigned row)
{
	for(unsigned x = 0;x < imageWidth;++ x)
	{
		unsigned cs = closestCluster(x, row);
		data[x] = png::gray_pixel(clusters[cs].c);
	}
}

int main(int argc, char** argv)
{
	char* outputName = &DEFAULT_NAME[0];
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
		if(strcmp(argv[i], "--name") == 0)
			outputName = &argv[++ i][0];
		if(strcmp(argv[i], "--no-save") == 0)
			saveImage = false;
	}

	clusters = new Cluster[clusterCount];

	std::uniform_int_distribution<unsigned> distColor(0, maxBrightness);
	std::uniform_int_distribution<unsigned> distWidth(0, imageWidth);
	std::uniform_int_distribution<unsigned> distHeght(0, imageHeight);
	for(unsigned i = 0;i < clusterCount;++ i)
	{
		clusters[i].x = distWidth(prandomEngine);
		clusters[i].y = distHeght(prandomEngine);
		clusters[i].c = distColor(prandomEngine);
	}

	png::gray_pixel** rows = new png::gray_pixel*[threadCount];
	for(unsigned i = 0;i < threadCount;++ i)
		rows[i] = new png::gray_pixel[imageWidth];

	std::thread* threads = new std::thread[threadCount];
	png::image<png::gray_pixel> image(imageWidth, imageHeight);

	unsigned lastProgress = 0;
	for(png::uint_32 y = 0;y < imageHeight/threadCount;++ y)
	{
		for(unsigned i = 0;i < threadCount;++ i)
			threads[i] = std::thread(renderRow, rows[i], y*threadCount+i);

		for(unsigned i = 0;i < threadCount;++ i)
		{
			threads[i].join();
			for(unsigned x = 0;x < imageWidth;++ x)
				image[y*threadCount+i][x] = rows[i][x];
		}

		if(showProgress)
		{
			float p = ((float)(y*threadCount) / imageHeight) * 100;
			if(p > lastProgress + 1)
			{
				std::cout << (unsigned)p << "% complete." << std::endl;
				lastProgress = p;
			}
		}
	}

	if(saveImage)
	{
		std::cout << "Saving the image..." << std::endl;
		image.write(outputName);
		std::cout << "Done!" << std::endl;
	}

	return 0;
}
