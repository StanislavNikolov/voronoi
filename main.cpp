#include <random> 	// random_device...
#include <cmath> 	// distance(sqrt)
#include <cstring> 	// strcmp
#include <thread> 	// threads
#include <cstdlib>	// atoi
#include <png++/png.hpp>

std::random_device randDev;
std::uniform_int_distribution<int> dist(0, 255);
std::mt19937 eng(randDev());

unsigned CLUSTERS = 400;
unsigned WIDTH = 1920;
unsigned HEIGHT = 1080;
unsigned THREADS = 2;

bool euclidian 				= true;
bool showProgress 			= false;
unsigned maxBrightness 		= 255;

char DEFAULT_NAME[] 	= "output.png";

struct Cluster {
	unsigned x, y, c;
};

Cluster* clusters;

unsigned rnd(unsigned n)
{
	return eng() % n;
}

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
	for(unsigned idx = 1;idx < CLUSTERS;++ idx)
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
	for(unsigned x = 0;x < WIDTH;++ x)
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
			CLUSTERS = atoi(argv[++ i]);
		if(strcmp(argv[i], "--width") == 0)
			WIDTH = atoi(argv[++ i]);
		if(strcmp(argv[i], "--height") == 0)
			HEIGHT = atoi(argv[++ i]);
		if(strcmp(argv[i], "--threads") == 0)
			THREADS = atoi(argv[++ i]);
		if(strcmp(argv[i], "--name") == 0)
			outputName = &argv[++ i][0];
	}

	clusters = new Cluster[CLUSTERS];

	for(unsigned i = 0;i < CLUSTERS;++ i)
	{
		clusters[i].x = rnd(WIDTH);
		clusters[i].y = rnd(HEIGHT);
		clusters[i].c = rnd(maxBrightness);
	}

	png::gray_pixel** rows = new png::gray_pixel*[THREADS];
	for(unsigned i = 0;i < THREADS;++ i)
		rows[i] = new png::gray_pixel[WIDTH];

	std::thread* threads = new std::thread[THREADS];
	png::image<png::gray_pixel> image(WIDTH, HEIGHT);

	unsigned lastProgress = 0;
	for(png::uint_32 y = 0;y < HEIGHT/THREADS;++ y)
	{
		for(unsigned i = 0;i < THREADS;++ i)
			threads[i] = std::thread(renderRow, rows[i], y*THREADS+i);

		for(unsigned i = 0;i < THREADS;++ i)
		{
			threads[i].join();
			for(unsigned x = 0;x < WIDTH;++ x)
				image[y*THREADS+i][x] = rows[i][x];
		}

		if(showProgress)
		{
			float p = ((float)(y*THREADS) / HEIGHT) * 100;
			if(p > lastProgress + 1)
			{
				std::cout << (unsigned)p << "% complete." << std::endl;
				lastProgress = p;
			}
		}
	}

	std::cout << "Saving the image..." << std::endl;
	image.write(outputName);
	std::cout << "Done!" << std::endl;

	return 0;
}
