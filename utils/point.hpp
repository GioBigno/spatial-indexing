#ifndef POINT_HPP
#define POINT_HPP

#include <string>
#include <vector>
#include <limits>
#include <cmath>
#include <algorithm>

namespace bigno{

class Point{

	public:
		Point(){
			this->coordinates = {};
		}
		
		Point(std::vector<double> v, void* data = nullptr){
			this->coordinates = v;
			this->setData(data);
		}

		std::size_t getDimensions() const {
			return this->coordinates.size();
		}

		double getCoordinate(const std::size_t dimension) const {
			return this->coordinates[dimension];
		}

		double getX() const {
			return this->coordinates[0];
		}
		
		double getY() const {
			return this->coordinates[1];
		}

		void* getData() const {
			return this->data;
		}

		std::string getDataString() const {
			return this->data == nullptr ? "" : *(static_cast<std::string*>(this->getData()));
		}

		void setData(void* data){
			this->data = data;
		}

		int cmp(const Point& target, const std::size_t dimension) const {
			double ret = this->coordinates[dimension] - target.coordinates[dimension];

			if(fabs(ret) < std::numeric_limits<double>::epsilon()){
				return 0;
			}else{
				return ret < 0 ? -1 : 1;
			}
		}

		double distance(const Point& p) const {
			
			std::size_t dimensions = std::min(this->coordinates.size(), p.coordinates.size());
			double sum = 0;

			for(std::size_t i=0; i<dimensions; i++){
				sum += std::pow(this->coordinates[i] - p.coordinates[i], 2);
			}

			return std::sqrt(sum);
		}

		std::string toString() const {
			std::string ret = this->getDataString();
			ret += "(";
			for(std::size_t i=0; i<this->coordinates.size(); i++){
				ret += std::to_string(this->coordinates[i]);
				if(i != this->coordinates.size()-1)
					ret += ", ";
			}
			ret += ")";
			return ret;
		}

	private:

		std::vector<double> coordinates;
		void* data = nullptr;
};

}

#endif //POINT_HPP
