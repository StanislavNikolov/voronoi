# Introduction
This is a hackable Voronoi graphics generators. It supports manhattan and euclidian distance, mulithreading support and other stuff you should look into the source to understand. Look at some [examples here.](http://imgur.com/gallery/pdzu4)  

# Building it
## Dependencies
Compiler that supports c++11.  
The png++ library.  

## Compiling it
> c++ -std=c++11 -O2 -lpng -lpthread main.cpp -o main

# Using it
Just call the generated binary. It supports those command-line options:  
 * **-no-euclid** - use the manhattan distance to determinate every point's cluster.  
 * **-progress** - show stats while generating the image.  
