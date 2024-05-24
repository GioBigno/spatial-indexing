cmake_minimum_required(VERSION 3.10)

project(giorgio VERSION 1.0)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(GEOS_PATH "/home/giorgio/dev/lib/bin/geos/" CACHE PATH "Path of geos library package")
set(SHAPELIB_PATH "/home/giorgio/dev/lib/bin/shapelib/" CACHE PATH "Path of shapelib library package")

list(APPEND CMAKE_PREFIX_PATH "${GEOS_PATH}")
list(APPEND CMAKE_PREFIX_PATH "${SHAPELIB_PATH}")

find_package(GEOS REQUIRED 3.13.0)
find_package(shapelib REQUIRED)

add_executable(giorgio main.cpp utils/src/shpformat.cpp utils/src/shpreader.cpp)

target_link_libraries(giorgio 
	PRIVATE GEOS::geos 
	PRIVATE ${shapelib_LIBRARIES}
)

target_include_directories(giorgio PRIVATE ${GEOS_INCLUDE_DIRS} utils)
