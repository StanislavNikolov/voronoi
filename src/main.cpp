#include <random> 	// random_device...
#include <cmath> 	// distance(sqrt)
#include <thread> 	// threads
#include <png++/png.hpp>

#include "parseInput.hpp"
#include "globalVariables.hpp"

// Make a pseudo-random engine seeded by a "true" random number
std::random_device randomDevice;
std::mt19937 prandomEngine(randomDevice());

char DEFAULT_NAME[] 		= "output";

struct Cluster {
	int x, y, c;
	float velX, velY;
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

int main(int argc, char** argv)
{
	char* outputName = &DEFAULT_NAME[0];
	parseInput(argc, argv);

	std::cout << showProgress << " " << clusters << std::endl;

	clusters = new Cluster[clusterCount];

	std::uniform_int_distribution<unsigned> distColor(0, maxBrightness);
	std::uniform_int_distribution<unsigned> distWidth(0, imageWidth);
	std::uniform_int_distribution<unsigned> distHeght(0, imageHeight);
	std::uniform_int_distribution<unsigned> distMotion(0, 20);
	for(unsigned i = 0;i < clusterCount;++ i)
	{
		clusters[i].x = distWidth(prandomEngine);
		clusters[i].y = distHeght(prandomEngine);
		clusters[i].c = distColor(prandomEngine);
		if(frameCount > 1)
		{
			clusters[i].velX = ((float)distMotion(prandomEngine) / 10);
								//-(float)distMotion(prandomEngine) / 10);
			clusters[i].velY = ((float)distMotion(prandomEngine) / 10);
								//-(float)distMotion(prandomEngine) / 10);
		}

	}

	png::gray_pixel** rows = new png::gray_pixel*[threadCount];
	for(unsigned i = 0;i < threadCount;++ i)
		rows[i] = new png::gray_pixel[imageWidth];

	std::thread* threads = new std::thread[threadCount];
	png::image<png::gray_pixel> image(imageWidth, imageHeight);

	for(unsigned currFrm = 0;currFrm < frameCount;++ currFrm)
	{
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
			char* tmpName = genFrameName(outputName, currFrm);
			image.write(tmpName);
			delete tmpName;
			std::cout << "Done with frame " << currFrm << std::endl;
		}

		for(unsigned c = 0;c < clusterCount and currFrm+1 < frameCount;++ c)
		{
			clusters[c].x += clusters[c].velX;
			if(clusters[c].x > imageWidth)
				clusters[c].x = 0;
			if(clusters[c].x < 0)
				clusters[c].x = imageWidth;

			clusters[c].y += clusters[c].velY;
			if(clusters[c].y > imageHeight)
				clusters[c].y = 0;
			if(clusters[c].y < 0)
				clusters[c].y = imageHeight;
		}
	}

	return 0;
}
