#include <geom/Coordinate.h>
#include <geom/Envelope.h>
#include <geos/geom/Point.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/index/kdtree/KdTree.h>
#include <geos/index/quadtree/Quadtree.h>
#include <geos/index/strtree/STRtree.h>
#include <cli/cli.h>
#include <cli/clilocalsession.h>
#include <cli/filehistorystorage.h>
#include <cli/loopscheduler.h>
#include <index/kdtree/KdNode.h>
#include <iostream>
#include <memory>
#include <random>
#include <limits>
#include "utils/headers/shpreader.h"

std::vector<std::shared_ptr<geos::geom::Geometry>> geometries;
double minX, minY, maxX, maxY;

std::unique_ptr<geos::index::kdtree::KdTree> kdTree;
std::unique_ptr<geos::index::quadtree::Quadtree> quadTree;
std::unique_ptr<geos::index::strtree::STRtree> rTree;

void cmd_load(std::ostream& out, const std::string& inputFile);
void cmd_build(std::ostream& out, const std::string& type);
void cmd_search_range_xy(std::ostream& out, const std::string& type, const double x1, const double y1, const double x2, const double y2);
void cmd_search_range_random(std::ostream& out, const std::string& type);
void cmd_compare_xy(std::ostream& out, const double x1, const double y1, const double x2, const double y2);
void cmd_compare_random(std::ostream& out, const std::size_t iterations);
bool readShapeFile(const std::string& fileName, std::vector<std::shared_ptr<geos::geom::Geometry>>& geometries);

int main() {

	auto rootMenu = std::make_unique<cli::Menu>("cli");

    rootMenu->Insert(
        "load",
		{"input"},
		[](std::ostream& out, const std::string& inputFile){cmd_load(out, inputFile);},
        "--input [file.shp]"
        );

    rootMenu->Insert(
        "build",
		{"type"},
		[](std::ostream& out, const std::string& type){cmd_build(out, type);},
        "--type [kd-tree|quad-tree|r-tree]"
        );

	rootMenu->Insert(
        "search_range",
		{"type", "envelope"},
		[](std::ostream& out, const std::string& type, const double x1, const double y1, const double x2, const double y2){cmd_search_range_xy(out, type, x1, y1, x2, y2);},
        "--type [kd-tree|quad-tree|r-tree] --x1 --y1 --x2 --y2"
        );
    
	rootMenu->Insert(
        "search_range",
		{"type"},
		[](std::ostream& out, const std::string& type){cmd_search_range_random(out, type);},
        "--type [kd-tree|quad-tree|r-tree]"
        );
	
	rootMenu->Insert(
        "compare",
		{"iterations"},
		[](std::ostream& out, const std::size_t iterations){cmd_compare_random(out, iterations);},
        "--iterations"
        );
	
	rootMenu->Insert(
        "compare",
		{"envelope"},
		[](std::ostream& out, const double x1, const double y1, const double x2, const double y2){cmd_compare_xy(out, x1, y1, x2, y2);},
        "--x1 --y1 --x2 --y2"
        );
	
	cli::Cli cli( std::move(rootMenu), std::make_unique<cli::FileHistoryStorage>(".cli") );
    cli.StdExceptionHandler(
        [](std::ostream& out, const std::string& cmd, const std::exception& e)
        {
            out << "Exception caught in cli handler: " << e.what()
                << " handling command: " << cmd << ".\n";
        }
        );

	cli::SetColor();

	cli::LoopScheduler scheduler;
	cli::CliLocalTerminalSession localSession(cli, scheduler, std::cout);
    localSession.ExitAction(
        [&scheduler](std::ostream& out)
        {
            out << "bye\n";
            scheduler.Stop();
        }
        );

    scheduler.Run();

    return 0;
}

std::string time_to_string(const double time){
	std::ostringstream oss;
	if(time < 1000){
		oss<<time<<" milliseconds";
	}else{
		oss<<time/1000.0<<" seconds";
	}
	return oss.str();
}

bool isValidType(const std::string& type){
	return (type == "kd-tree" ||type == "quad-tree" || type == "r-tree");
}

geos::geom::Envelope create_random_envelope(const double x1, const double y1, const double x2, const double y2){
	
	std::default_random_engine rnd{std::random_device{}()};

	std::uniform_real_distribution<double> dist1(x1, x2);
	double random_x1 = dist1(rnd); 

	std::uniform_real_distribution<double> dist2(y1, y2);
	double random_y1 = dist2(rnd);

	std::uniform_real_distribution<double> dist3(random_x1, x2);
	double random_x2 = dist3(rnd);

	std::uniform_real_distribution<double> dist4(random_y1, y2);
	double random_y2 = dist4(rnd);


	return geos::geom::Envelope(random_x1, random_x2, random_y1, random_y2);
}

void cmd_load(std::ostream& out, const std::string& inputFile){

	if(!readShapeFile(inputFile, geometries)){
		return;
	}

	minX = std::numeric_limits<double>::max();
    minY = std::numeric_limits<double>::max();
    maxX = std::numeric_limits<double>::lowest();
    maxY = std::numeric_limits<double>::lowest();

    for (const auto& geom : geometries) {
        if (geom) {
            const geos::geom::Envelope* envelope = geom->getEnvelopeInternal();
            if (envelope) {
                if (envelope->getMinX() < minX) minX = envelope->getMinX();
                if (envelope->getMinY() < minY) minY = envelope->getMinY();
                if (envelope->getMaxX() > maxX) maxX = envelope->getMaxX();
                if (envelope->getMaxY() > maxY) maxY = envelope->getMaxY();
            }
        }
    }

	kdTree.reset();
	quadTree.reset();
	rTree.reset();
}

bool build(const std::string& type){

	if(type == "kd-tree"){
	
		kdTree = std::make_unique<geos::index::kdtree::KdTree>(std::numeric_limits<double>::epsilon());

		for(size_t i=0; i<geometries.size(); i++){
			
			if(geometries[i]->getGeometryTypeId() != geos::geom::GEOS_POINT){
				return false; 
			}
			geos::geom::Coordinate coord(*std::static_pointer_cast<geos::geom::Point>(geometries[i])->getCoordinate());
			kdTree->insert(coord, reinterpret_cast<void*>(i));
		}

	}else if(type == "quad-tree"){
		
		quadTree = std::make_unique<geos::index::quadtree::Quadtree>();

		for(size_t i=0; i<geometries.size(); i++){
			quadTree->insert(geometries[i]->getEnvelopeInternal(), reinterpret_cast<void*>(i));
		}

	}else if(type == "r-tree"){
	
		rTree = std::make_unique<geos::index::strtree::STRtree>();

		for(size_t i=0; i<geometries.size(); i++){
			rTree->insert(geometries[i]->getEnvelopeInternal(), reinterpret_cast<void*>(i));
		}
	}

	return true;
}

void cmd_build(std::ostream& out, const std::string& type){
	
	if(!isValidType(type)){
		out<<"Error: Invalid data structure type '"<<type<<"'"<<std::endl;
		return;
	}

	if(geometries.empty()){
		out<<"Error: no geometries loaded"<<std::endl;
		return;
	}

	std::chrono::duration<double, std::milli> duration;
	const auto start = std::chrono::steady_clock::now();

	if(!build(type)){
		out<<"Error building the data structure"<<std::endl;
		return;
	}

	const auto end = std::chrono::steady_clock::now();
	duration = end - start;

	out<<type<<" built successfully"<<std::endl
	<<"time: "<<time_to_string(duration.count())<<std::endl
	<<"geometries: "<<geometries.size()<<std::endl;
}

bool search(const std::string& type, const geos::geom::Envelope& envelope, std::vector<std::size_t>& geometriesFound){

	if(type == "kd-tree"){

		if(!kdTree){
			return false;
		}
	
		std::vector<geos::index::kdtree::KdNode*> result;
		
		kdTree->query(envelope, result);

		for(geos::index::kdtree::KdNode* node : result){
			geometriesFound.push_back(reinterpret_cast<std::size_t>(node->getData()));
		}
	}else if(type == "quad-tree"){
		
		if(!quadTree){
			return false;
		}
	
		std::vector<void*> result;
		
		quadTree->query(&envelope, result);

		for(const void* ptr : result){
			geometriesFound.push_back(reinterpret_cast<std::size_t>(ptr));
		}
	}else if(type == "r-tree"){
		
		if(!rTree){
			return false;
		}
	
		std::vector<void*> result;
		
		rTree->query(&envelope, result);
		
		for(const void* ptr : result){
			geometriesFound.push_back(reinterpret_cast<std::size_t>(ptr));
		}
	}

	return true;
}

void cmd_search_range_xy(std::ostream& out, const std::string& type, const double x1, const double y1, const double x2, const double y2){

	if(!isValidType(type)){
		out<<"Error: Invalid data structure type '"<<type<<"'"<<std::endl;
		return;
	}

	geos::geom::Envelope envelope(x1, x2, y1, y2);
	std::vector<size_t> geometriesFound;

	std::chrono::duration<double, std::milli> duration;
	const auto start = std::chrono::steady_clock::now();
	
	if(!search(type, envelope, geometriesFound)){
		out<<type<<" not built yet"<<std::endl;
		return;
	}
	
	const auto end = std::chrono::steady_clock::now();
	duration = end - start;

	out<<"geometries: "<<geometriesFound.size()<<std::endl
	<<"time: "<<time_to_string(duration.count())<<std::endl;
}

void cmd_search_range_random(std::ostream& out, const std::string& type){

	if(!isValidType(type)){
		out<<"Error: Invalid data structure type '"<<type<<"'"<<std::endl;
		return;
	}

	geos::geom::Envelope envelope = create_random_envelope(minX, minY, maxX, maxY);
	std::vector<size_t> geometriesFound;

	std::chrono::duration<double, std::milli> duration;
	const auto start = std::chrono::steady_clock::now();
	
	if(!search(type, envelope, geometriesFound)){
		out<<type<<" not built yet"<<std::endl;
		return;
	}
	
	const auto end = std::chrono::steady_clock::now();
	duration = end - start;

	out<<"random envelope: "<<envelope.getMinX()<<", "<<envelope.getMinY()<<", "<<envelope.getMaxX()<<", "<<envelope.getMaxY()<<std::endl
	<<"geometries: "<<geometriesFound.size()<<std::endl
	<<"time: "<<time_to_string(duration.count())<<std::endl;
}

void cmd_compare_xy(std::ostream& out, const double x1, const double y1, const double x2, const double y2){

	std::vector<std::string> avaibleDataStructures;

	if(kdTree){
		avaibleDataStructures.push_back("kd-tree");	
	}
	if(quadTree){
		avaibleDataStructures.push_back("quad-tree");	
	}
	if(rTree){
		avaibleDataStructures.push_back("r-tree");	
	}

	for(const std::string& type : avaibleDataStructures){
	
		out<<std::string(20, '-')<<type<<std::string(20, '-')<<std::endl;

		cmd_search_range_xy(out, type, x1, y1, x2, y2);

		out<<std::string(40 + type.size(), '-')<<std::endl;
	}
}

void cmd_compare_random(std::ostream& out, const std::size_t iterations){

	std::vector<std::string> avaibleDataStructures;

	if(kdTree){
		avaibleDataStructures.push_back("kd-tree");	
	}
	if(quadTree){
		avaibleDataStructures.push_back("quad-tree");	
	}
	if(rTree){
		avaibleDataStructures.push_back("r-tree");	
	}

	for(const std::string& type : avaibleDataStructures){
	
		out<<std::string(20, '-')<<type<<std::string(20, '-')<<std::endl;

		std::size_t totalGeometriesFound = 0;
		std::chrono::duration<double, std::milli> duration;
		const auto start = std::chrono::steady_clock::now();

		for(size_t i=0; i<iterations; i++){
			
			geos::geom::Envelope envelope = create_random_envelope(minX, minY, maxX, maxY);
			std::vector<size_t> geometriesFound;

			search(type, envelope, geometriesFound);
			totalGeometriesFound += geometriesFound.size();
		}

		const auto end = std::chrono::steady_clock::now();
		duration = end - start;

		out<<"geometries: "<<totalGeometriesFound<<std::endl
		<<"average time: "<<time_to_string(duration.count()/iterations)<<std::endl
		<<"total time: "<<time_to_string(duration.count())<<std::endl
		<<std::string(40 + type.size(), '-')<<std::endl;
	}
}

bool readShapeFile(const std::string& fileName, std::vector<std::shared_ptr<geos::geom::Geometry>>& geometries){

    geometries.clear();

    bpp::ShpReader reader;
	std::string openError;
    reader.setFile(fileName);
    bool retval = reader.open(true, true, openError);

	if(!retval){
		return false;
	}

	geos::geom::Geometry* geomRead;

    while(reader.next()){

        std::unique_ptr<geos::geom::Geometry> currentGeom(nullptr);

        switch(reader.getGeomType()){

        case bpp::gPoint:
			geomRead = reader.readPoint();
			if(geomRead){
            	currentGeom = std::move(geomRead->clone());
			}
            break;
        case bpp::gMultiPoint:
			geomRead = reader.readMultiPoint();
			if(geomRead){
            	currentGeom = std::move(geomRead->clone());
			}
            break;
        case bpp::gLine:
			geomRead = reader.readLineString();
			if(geomRead){
            	currentGeom = std::move(geomRead->clone());
			}
            break;
        case bpp::gPolygon:
			geomRead = reader.readMultiPolygon();
			if(geomRead){
            	currentGeom = std::move(geomRead->clone());
			}
			break;
        case bpp::gUnknown:
			std::cout << "[shpReader]: geometry unknow";
            break;
        }

        if(currentGeom)
            geometries.push_back(std::move(currentGeom));
    }

	return true;
}
