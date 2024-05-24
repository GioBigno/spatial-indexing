#include <geos/geom/Point.h>
#include <geos/geom/GeometryFactory.h>
#include <iostream>
#include "utils/headers/shpreader.h"


void readShapeFile(const std::string& fileName);

int main() {

	

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
