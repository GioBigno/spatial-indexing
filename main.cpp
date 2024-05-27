#include <geos/geom/Point.h>
#include <geos/geom/GeometryFactory.h>
#include <cli/cli.h>
#include <cli/clilocalsession.h>
#include <cli/filehistorystorage.h>
#include <cli/loopscheduler.h>
#include <iostream>
#include "utils/headers/shpreader.h"

void readShapeFile(const std::string& fileName, std::vector<std::unique_ptr<geos::geom::Geometry>>& geometries);

int main() {

	std::vector<std::unique_ptr<geos::geom::Geometry>> geometries;

	auto rootMenu = std::make_unique<cli::Menu>("cli");

    rootMenu->Insert(
        "add", {"op1", "op2"},
        [](std::ostream& out, const int op1_, const int op2_){

            //std::string str_out;
            //std::ostringstream stream_out(str_out);
            //login(out, username_, password_);

			out<<op1_+op2_<<std::endl;

            //out<<stream_out.str();
        },
        "somma di due numeri interi"
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
            out << "Closing App...\n";
            scheduler.Stop();
        }
        );

    scheduler.Run();

    return 0;
}


void readShapeFile(const std::string& fileName, std::vector<std::unique_ptr<geos::geom::Geometry>>& geometries){

    geometries.clear();

    bpp::ShpReader reader;
    std::string openError;
    reader.setFile(fileName);
    bool retval = reader.open(true, false, openError);

    while(retval && reader.next()){

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
}
