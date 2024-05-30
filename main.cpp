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
#include <limits>
#include "utils/headers/shpreader.h"

std::vector<std::shared_ptr<geos::geom::Geometry>> geometries;

std::unique_ptr<geos::index::kdtree::KdTree> kdTree;
std::unique_ptr<geos::index::quadtree::Quadtree> quadTree;
std::unique_ptr<geos::index::strtree::STRtree> rTree;

void cmd_build(std::ostream& out, const std::string& type, const std::string& inputFile);
void cmd_search_range(std::ostream& out, const std::string& type, const double x1, const double y1, const double x2, const double y2);
bool readShapeFile(const std::string& fileName, std::vector<std::shared_ptr<geos::geom::Geometry>>& geometries);

int main() {

	auto rootMenu = std::make_unique<cli::Menu>("cli");

    rootMenu->Insert(
        "build",
		{"type", "input"},
		[](std::ostream& out, const std::string& type, const std::string& inputFile){cmd_build(out, type, inputFile);},
        "--type [kd-tree|quad-tree|r-tree] --input [file.shp]"
        );

    rootMenu->Insert(
        "search_range",
		{"type", "query", "params"},
		[](std::ostream& out, const std::string& type, const double x1, const double y1, const double x2, const double y2){cmd_search_range(out, type, x1, y1, x2, y2);},
        "--type [kd-tree|quad-tree|r-tree] --x1 --y1 --x2 --y2"
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

void cmd_build(std::ostream& out, const std::string& type, const std::string& inputFile){
	
	if(!readShapeFile(inputFile, geometries)){
		return;
	}

	std::chrono::duration<double, std::milli> duration;
	const auto start = std::chrono::steady_clock::now();
	
	if(type == "kd-tree"){
	
		kdTree = std::make_unique<geos::index::kdtree::KdTree>(std::numeric_limits<double>::epsilon());

		for(const std::shared_ptr<geos::geom::Geometry>& geom : geometries){
			if(geom->getGeometryTypeId() != geos::geom::GEOS_POINT){
				out<<"Error: in kd-tree all geometries must be points"<<std::endl;
				return; 
			}
			geos::geom::Coordinate coord(*std::static_pointer_cast<geos::geom::Point>(geom)->getCoordinate());
			kdTree->insert(coord, geom.get());
		}

	}else if(type == "quad-tree"){
		
		quadTree = std::make_unique<geos::index::quadtree::Quadtree>();

		for(const std::shared_ptr<geos::geom::Geometry>& geom : geometries){
			quadTree->insert(geom->getEnvelopeInternal(), geom.get());
		}

	}else if(type == "r-tree"){
	
		rTree = std::make_unique<geos::index::strtree::STRtree>();

		for(const std::shared_ptr<geos::geom::Geometry>& geom : geometries){
			rTree->insert(geom->getEnvelopeInternal(), geom.get());
		}

	}else{
		out<<"Error: Invalid data structure type '"<<type<<"'"<<std::endl;
		return;
	}

	const auto end = std::chrono::steady_clock::now();
	duration = end - start;

	out<<type<<" built successfully"<<std::endl
	<<"time: "<<(duration.count())<<" milli-seconds"<<std::endl
	<<"geometries: "<<geometries.size()<<std::endl;

}

void cmd_search_range(std::ostream& out, const std::string& type, const double x1, const double y1, const double x2, const double y2){

	geos::geom::Envelope envelope(x1, x2, y1, y2);
	std::size_t numGeom = 0;

	out<<"params:"<<std::endl;
	
	out<<std::endl;

	std::chrono::duration<double, std::milli> duration;
	const auto start = std::chrono::steady_clock::now();
	
	if(type == "kd-tree"){
	
		std::vector<geos::index::kdtree::KdNode*> result;
		
		kdTree->query(envelope, result);

		numGeom = result.size();

		for(geos::index::kdtree::KdNode* node : result){
			node->getData();
		}

	}else if(type == "quad-tree"){
		
	}else if(type == "r-tree"){
		
	}else{
		out<<"Error: Invalid data structure type '"<<type<<"'"<<std::endl;
		return;
	}

	out<<type<<" built successfully"<<std::endl
	<<"time: "<<(duration.count())<<" milli-seconds"<<std::endl
	<<"geometries: "<<numGeom<<std::endl;

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
