#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <random>
#include "kd-tree.hpp"

const std::size_t numPoints = 10;
const std::size_t dimensions = 2;
const double coordinateMin = 0;
const double coordinateMax = 15;

bool generate_points(std::vector<bigno::Point>& points);
bool import_points(const std::string fileName, std::vector<bigno::Point>& points);
bool export_points(const std::string fileName, const std::vector<bigno::Point>& points);

int main(){
	
	std::vector<bigno::Point> points;

	//generate_points(points);
	//export_points("p1.txt", points);
	
	import_points("p1.txt", points);

	std::cout<<"points:"<<std::endl;
	for(bigno::Point p : points){
		std::cout<<p.toString()<<std::endl;
	}

	bigno::KdTree tree(dimensions, points);

	std::vector<double> p1 = {3.5, 12};
	tree.query_neighbors(bigno::Point(p1, new std::string("START")), 1);

	return 0;
}

bool generate_points(std::vector<bigno::Point>& points){

	for(std::size_t i=0; i<numPoints; i++){
		
		std::vector<double> coordinates;

		for(std::size_t j=0; j<dimensions; j++){
			std::default_random_engine rnd{std::random_device{}()};
			std::uniform_real_distribution<double> dist(coordinateMin, coordinateMax);
			coordinates.push_back(dist(rnd));
		}

		bigno::Point p(coordinates, new std::string(1, 'A'+i));
		points.push_back(p);
	}

	return true;
}

bool import_points(const std::string fileName, std::vector<bigno::Point>& points){

	std::ifstream infile(fileName);
	if(!infile.is_open()){
		return false;
	}

	std::string line;
	while(std::getline(infile, line)){
		std::stringstream ss(line);

		std::vector<double> coordinates;
		double coordinate;

		std::string data;
		bool dataFlag = true;
		
		while(std::getline(ss, line, ',')){
			std::stringstream coordinateStream(line);
			
			if(dataFlag){
				coordinateStream>>data;
				dataFlag = false;
			}else{
				coordinateStream>>coordinate;
				coordinates.push_back(coordinate);
			}
		}

		if(!coordinates.empty()){
			points.push_back(bigno::Point(coordinates, new std::string(data)));
		}
	}

	infile.close();

	return true;
}

bool export_points(const std::string fileName, const std::vector<bigno::Point>& points){

	std::ofstream outfile(fileName);
	if(!outfile.is_open()){
		return false;
	}

	for(const bigno::Point& point : points){
		outfile<<point.getDataString()<<',';
		for(std::size_t i=0; i<point.getDimensions(); i++){
			outfile<<point.getCoordinate(i);
			if(i < point.getDimensions()-1)
				outfile<<',';
		}
		outfile<<std::endl;
	}

	outfile.close();

	return true;
}
