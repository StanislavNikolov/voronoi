# Introduction
This is a hackable Voronoi graphics generators. It supports manhattan and euclidian distance, mulithreading support and other stuff you should look into the source to understand. Look at some [examples here.](http://imgur.com/gallery/pdzu4)  

![example](http://i.imgur.com/tuBQLy0.png)

# Building it
## Dependencies
Compiler that supports c++11.  
The png++ library.  

## Compiling it
> c++ -std=c++11 -O2 -lpng -lpthread main.cpp globalVariables.cpp parseInput.cpp -o main

# Using it
Just call the generated binary. It supports those command-line options:  
 * **--no-euclid** - use the manhattan distance to determinate every point's cluster.
 * **--progress** - show stats while generating the image.
 * **--width/height** - set the output image width/height.
 * **--clusters** - the amount of clusters (zones) in the image.
 * **--threads** - the number of threads working for you pleasure!
 * **--dark** - the maximum brightness in the image (between 1 and 255).
 * **--frames** - generate different images with the clusters moved only a little, so that they can later be stiched together into a video with something like ffmpeg.

## Examples
> ./main --no-euclid --width 1920 --height 1080 --threads 8 --clusters 1000 --dark 150
