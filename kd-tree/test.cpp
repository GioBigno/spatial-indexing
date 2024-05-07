#include <iostream>
#include <vector>
#include <random>
#include "kd-tree.hpp"


const std::size_t numPoints = 5;
const std::size_t dimensions = 2;
const double coordinateMin = 0;
const double coordinateMax = 10;

int main(){

	
	std::vector<bigno::Point> points;

	for(std::size_t i=0; i<numPoints; i++){
		
		std::vector<double> coordinates;

		for(std::size_t j=0; j<dimensions; j++){
			std::default_random_engine rnd{std::random_device{}()};
			std::uniform_real_distribution<double> dist(coordinateMin, coordinateMax);
			coordinates.push_back(dist(rnd));
		}

		bigno::Point p(coordinates);
		points.push_back(p);
	}

	std::cout<<"points:"<<std::endl;
	for(bigno::Point p : points){
		std::cout<<"(";
		for(std::size_t i=0; i<dimensions; i++){
			std::cout<<p.coordinates[i]<<", ";
		}
		std::cout<<")"<<std::endl;
	}

	bigno::KdTree tree(dimensions, points);


	return 0;
}
