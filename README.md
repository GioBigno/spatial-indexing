## Simple CLI Program for Comparing Spatial Indexing Methods

I gave a talk at the Italian C++ Community where I explain how indices work, when they are useful, and the code I wrote.  
Link to the video on youtube: [link](https://www.youtube.com/watch?v=Cnjix7shv00&t=3111s&ab_channel=ItalianCppCommunity)

## How to Build

### Dependencies

- [GEOS](https://github.com/libgeos/geos)
- [Shapelib](https://github.com/OSGeo/shapelib)
- [CLI](https://github.com/daniele77/cli)

### build with cmake
```
mkdir build && cd build
cmake ..
cmake --build .
```

If you have installed the dependency libraries in a non-standard path, you must specify the path before building with:

```
cmake .. -DQT_PATH:PATH=<your_path_to_qt>
cmake .. -DGEOS_PATH:PATH=<your_path_to_geos>
cmake .. -DSHAPELIB_PATH:PATH=<your_path_to_shapelib>
```

## Functionality

I created a CLI with several commands:

- `help`  
  Displays the list of commands and how to use them.

- `load [file.shp]`  
  Reads a shapefile containing geometries and saves them in memory.

- `build [kd-tree|quad-tree|r-tree|geohash]`  
  Builds the specified data structure with the previously loaded geometries.

- `search_range [kd-tree|quad-tree|r-tree|geohash] --x1 --y1 --x2 --y2`  
  Performs a query on the specified data structure using the rectangle defined by the given coordinates.

- `search_range [kd-tree|quad-tree|r-tree|geohash]`  
  Performs a query on the specified data structure using a randomly generated rectangle.

- `compare <iterations>`  
  Performs n queries on the already built data structures and prints the times.

- `compare --x1 --y1 --x2 --y2`  
  Performs a query on the already built data structures using the rectangle defined by the given coordinates and prints the times.
