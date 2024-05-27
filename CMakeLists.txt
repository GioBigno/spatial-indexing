cmake_minimum_required(VERSION 3.10)

project(giorgio VERSION 1.0)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(GEOS_PATH "/home/giorgio/dev/lib/bin/geos/" CACHE PATH "Path of geos library package")
set(SHAPELIB_PATH "/home/giorgio/dev/lib/bin/shapelib/" CACHE PATH "Path of shapelib library package")
set(CLI_PATH "/home/giorgio/dev/lib/bin/cli/lib/cmake/cli" CACHE PATH "Path of cli library package")

list(APPEND CMAKE_PREFIX_PATH "${GEOS_PATH}")
list(APPEND CMAKE_PREFIX_PATH "${SHAPELIB_PATH}")
list(APPEND CMAKE_PREFIX_PATH "${CLI_PATH}")

find_package(GEOS REQUIRED 3.13.0)
find_package(shapelib REQUIRED)
find_package(cli REQUIRED)

add_executable(giorgio main.cpp utils/src/shpformat.cpp utils/src/shpreader.cpp)

target_link_libraries(giorgio 
	PRIVATE GEOS::geos 
	PRIVATE ${shapelib_LIBRARIES}
	PRIVATE cli::cli
)

target_include_directories(giorgio PRIVATE ${GEOS_INCLUDE_DIRS} utils)

# Move compile_commands.json to the project root
add_custom_command(
    TARGET giorgio POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_BINARY_DIR}/compile_commands.json
    ${CMAKE_SOURCE_DIR}/compile_commands.json
)
