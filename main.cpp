#include <png++/png.hpp>
#include <random>
#include <cmath>
#include <cstring>
#include <thread>

std::random_device randDev;
std::uniform_int_distribution<int> dist(0, 255);
std::mt19937 eng(randDev());

const unsigned CLUSTERS = 400;
const unsigned WIDTH = 1920;
const unsigned HEIGHT = 1080;
const unsigned THREADS = 4;

bool euclidian = true;
bool showProgress;

struct Cluster {
	unsigned x, y, c;
} clusters[CLUSTERS];

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

void renderRow(unsigned* data, unsigned row)
{
	for(unsigned x = 0;x < WIDTH;++ x)
	{
		unsigned cs = closestCluster(x, row);
		data[x] = clusters[cs].c;
	}
}

int main(int argc, char** argv)
{
	png::image< png::rgb_pixel > image(WIDTH, HEIGHT);

	for(unsigned i = 0;i < argc;++ i)
	{
		if(strcmp(argv[i], "-no-euclid") == 0)
			euclidian = false;
		if(strcmp(argv[i], "-progress") == 0)
			showProgress = true;
	}

	for(auto &c : clusters)
	{
		c.x = rnd(WIDTH);
		c.y = rnd(HEIGHT);
		c.c = rnd(255);
	}

	unsigned rows[THREADS][WIDTH];
	std::thread threads[THREADS];

	unsigned lastProgress = 0;
	for(png::uint_32 y = 0;y < HEIGHT/THREADS;++ y)
	{
		for(unsigned i = 0;i < THREADS;++ i)
			threads[i] = std::thread(renderRow, rows[i], y*THREADS+i);

		for(unsigned i = 0;i < THREADS;++ i)
		{
			threads[i].join();
			for(unsigned x = 0;x < WIDTH;++ x)
				image[y*THREADS+i][x] = png::rgb_pixel(rows[i][x], rows[i][x], rows[i][x]);
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
	image.write("output.png");
	std::cout << "Done!" << std::endl;

	return 0;
}
