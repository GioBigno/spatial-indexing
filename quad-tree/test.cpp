#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <random>
#include <chrono>
#include "quad-tree.hpp"

const std::size_t numPoints = 60000;
const std::size_t dimensions = 2;
const double coordinateMin = 0;
const double coordinateMax = 200;
const size_t nodeMaxPoints = 300;

bool generate_points(std::vector<bigno::Point>& points);
bool import_points(const std::string fileName, std::vector<bigno::Point>& points);
bool export_points(const std::string fileName, const std::vector<bigno::Point>& points);

int main(){
	
	std::vector<bigno::Point> points;

	generate_points(points);
	export_points("p2.txt", points);
	
	//import_points("p1.txt", points);

	std::cout << "[POINTS]"<<std::endl;
	for(bigno::Point p : points){
		std::cout << p.toString() << std::endl;
	}

	//QUAD TREE

	std::cout << "[CREATION]" << std::endl;
	bigno::QuadTree tree(bigno::Rect(bigno::Point({coordinateMin, coordinateMin}), bigno::Point({coordinateMax, coordinateMax})), nodeMaxPoints);

	for(bigno::Point p : points){
		tree.insert(p);
	}
	
	bigno::Rect r1(bigno::Point({40, 60}), bigno::Point({120.5, 100}));
	
	std::cout << "[QUERY]" << std::endl;
	std::cout << "searching points in envelope: " << r1.getBottomLeft().toString() << ", " << r1.getTopRight().toString() << std::endl;
	

	const auto start = std::chrono::steady_clock::now();

	std::vector<bigno::Point> res = tree.query_envelope(r1);

	const auto end = std::chrono::steady_clock::now();
	std::chrono::duration<double> elapsed = end - start;
	
	std::cout << "quad-tree found " << res.size() << " points in " << elapsed.count() << " seconds" << std::endl;

	//LINEAR SEARCH

	const auto start2 = std::chrono::steady_clock::now();

	std::size_t count = 0;
	for(bigno::Point p : points){
		if(r1.contains(p)){
			count++;
		}
	}
	
	const auto end2 = std::chrono::steady_clock::now();
	std::chrono::duration<double> elapsed2 = end2 - start2;
	
	std::cout << "linear-search found " << count << " points in " << elapsed2.count() << " seconds" << std::endl;

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

		std::string name = "P" + std::to_string(i);
		bigno::Point p(coordinates, new std::string(name));
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
