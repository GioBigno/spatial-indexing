#include <geos/geom/Point.h>
#include <geos/geom/GeometryFactory.h>
#include <cli/cli.h>
#include <cli/clilocalsession.h>
#include <cli/filehistorystorage.h>
#include <cli/loopscheduler.h>
#include <iostream>
#include <memory>
#include "headers/point.hpp"
#include "kd-tree/kd-tree.hpp"
#include "quad-tree/quad-tree.hpp"
#include "utils/headers/shpreader.h"

std::vector<std::shared_ptr<geos::geom::Geometry>> geometries;

std::unique_ptr<bigno::KdTree> kdTree = nullptr;
std::unique_ptr<bigno::QuadTree> quadTree = nullptr;

bool readShapeFile(const std::string& fileName, std::vector<std::shared_ptr<geos::geom::Geometry>>& geometries);

void cmd_build(std::ostream& out, const std::string& type, const std::string& inputFile);

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

	std::vector<bigno::Point> points;
	
	for(const std::shared_ptr<geos::geom::Geometry>& geom : geometries){
		if(geom->getGeometryTypeId() != geos::geom::GEOS_POINT){
			out<<"Error: in kd-tree all geometries must be points"<<std::endl;
			return false; 
		}
		std::shared_ptr<geos::geom::Point> point = std::static_pointer_cast<geos::geom::Point>(geom); 
		bigno::Point p({point->getX(), point->getY()}, nullptr);
		points.push_back(p);
	}

	kdTree = std::make_unique<bigno::KdTree>(2, points);

	return true;
}

void cmd_build(std::ostream& out, const std::string& type, const std::string& inputFile){
	
	if(!readShapeFile(inputFile, geometries)){
		return;
	}

	//build the data structure

	double time = 0;


	if(type == "kd-tree"){

		const auto start = std::chrono::steady_clock::now();
		bool buildSuccess = buildKdTree(out);
		const auto end = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsed = end - start;
		time = elapsed.count();

		if(!buildSuccess){
			return;
		}

	}else if(type == "quad-tree"){
		//TODO
	}else if(type == "r-tree"){
		//TODO
	}else{
		out<<"Error: Invalid data structure type '"<<type<<"'"<<std::endl;
		return;
	}

	out<<type<<" built successfully"<<std::endl
	<<"time: "<<time<<" seconds"<<std::endl
	<<"points: "<<geometries.size()<<std::endl;

}


bool readShapeFile(const std::string& fileName, std::vector<std::shared_ptr<geos::geom::Geometry>>& geometries){

    geometries.clear();

    bpp::ShpReader reader;
	std::string openError;
    reader.setFile(fileName);
    bool retval = reader.open(true, false, openError);

	if(!retval){
		return false;
	}

    while(reader.next()){

        std::unique_ptr<geos::geom::Geometry> currentGeom(nullptr);

        switch(reader.getGeomType()){

        case bpp::gPoint:
            currentGeom = std::move(reader.readPoint()->clone());
            break;
        case bpp::gMultiPoint:
            currentGeom = std::move(reader.readMultiPoint()->clone());
            break;
        case bpp::gLine:
            currentGeom = std::move(reader.readLineString()->clone());
            break;
        case bpp::gPolygon:
            currentGeom = std::move(reader.readMultiPolygon()->clone());
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
