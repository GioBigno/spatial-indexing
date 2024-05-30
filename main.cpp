#include <geom/Coordinate.h>
#include <geos/geom/Point.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/index/kdtree/KdTree.h>
#include <geos/index/quadtree/Quadtree.h>
#include <geos/index/strtree/STRtree.h>
#include <cli/cli.h>
#include <cli/clilocalsession.h>
#include <cli/filehistorystorage.h>
#include <cli/loopscheduler.h>
#include <iostream>
#include <memory>
#include <limits>
#include "utils/headers/shpreader.h"

std::vector<std::shared_ptr<geos::geom::Geometry>> geometries;

std::unique_ptr<geos::index::kdtree::KdTree> kdTree;
std::unique_ptr<geos::index::quadtree::Quadtree> quadTree;
std::unique_ptr<geos::index::strtree::STRtree> rTree;

void cmd_build(std::ostream& out, const std::string& type, const std::string& inputFile);
bool readShapeFile(const std::string& fileName, std::vector<std::shared_ptr<geos::geom::Geometry>>& geometries);

int main() {

	auto rootMenu = std::make_unique<cli::Menu>("cli");

    rootMenu->Insert(
        "build",
		{"type", "input"},
		[](std::ostream& out, const std::string& type, const std::string& inputFile){cmd_build(out, type, inputFile);},
        "--type [kd-tree|quad-tree|r-tree] --input [file.shp]"
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

bool buildKdTree(std::ostream& out){

	kdTree = std::make_unique<geos::index::kdtree::KdTree>(std::numeric_limits<double>::epsilon());

	for(const std::shared_ptr<geos::geom::Geometry>& geom : geometries){
		if(geom->getGeometryTypeId() != geos::geom::GEOS_POINT){
			out<<"Error: in kd-tree all geometries must be points"<<std::endl;
			return false; 
		}
		geos::geom::Coordinate coord(*std::static_pointer_cast<geos::geom::Point>(geom)->getCoordinate());
		kdTree->insert(coord, geom.get());
	}

	return true;
}

void buildQuadTree(std::ostream& out){

	quadTree = std::make_unique<geos::index::quadtree::Quadtree>();

	for(const std::shared_ptr<geos::geom::Geometry>& geom : geometries){
		quadTree->insert(geom->getEnvelopeInternal(), geom.get());
	}
}

void buildRTree(std::ostream& out){

	rTree = std::make_unique<geos::index::strtree::STRtree>();

	for(const std::shared_ptr<geos::geom::Geometry>& geom : geometries){
		rTree->insert(geom->getEnvelopeInternal(), geom.get());
	}
}

void cmd_build(std::ostream& out, const std::string& type, const std::string& inputFile){
	
	if(!readShapeFile(inputFile, geometries)){
		return;
	}

	//build the data structure

	std::chrono::duration<double> duration;
	
	if(type == "kd-tree"){

		const auto start = std::chrono::steady_clock::now();
		bool buildSuccess = buildKdTree(out);
		const auto end = std::chrono::steady_clock::now();
		duration = end - start;

		if(!buildSuccess){
			return;
		}

	}else if(type == "quad-tree"){
		
		const auto start = std::chrono::steady_clock::now();
		buildQuadTree(out);
		const auto end = std::chrono::steady_clock::now();
		duration = end - start;

	}else if(type == "r-tree"){
		
		const auto start = std::chrono::steady_clock::now();
		buildRTree(out);
		const auto end = std::chrono::steady_clock::now();
		duration = end - start;

	}else{
		out<<"Error: Invalid data structure type '"<<type<<"'"<<std::endl;
		return;
	}

	out<<type<<" built successfully"<<std::endl
	<<"time: "<<(duration.count()*1000)<<" milli-seconds"<<std::endl
	<<"geometries: "<<geometries.size()<<std::endl;

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
