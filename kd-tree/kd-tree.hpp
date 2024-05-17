#include <iostream>
#include <utility>
#include <vector>
#include <limits>
#include <cmath>
#include <algorithm>
#include <memory>

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

class KdTree{

	public:

		KdTree(const std::size_t dimensions, std::vector<Point>& points){
			this->dimensions = dimensions;
			insert(this->root, points.begin(), points.end(), 0);
		}
	
		Point query_neighbor(const Point& target) const {
			Point a;
			return this->query(target, this->root, {-1, a}).second;
		}
		

	private:

		struct Node{
			Point point;
			std::size_t dimension;
			std::unique_ptr<Node> left;
			std::unique_ptr<Node> right;

			Node(const Point point, const std::size_t dimension){
				this->point = point;
				this->dimension = dimension;
			}

			bool isLeaf(){
				return (this->left).get() == nullptr && (this->right).get() == nullptr;
			}

		};

		void insert(std::unique_ptr<Node>& node, std::vector<Point>::iterator begin, std::vector<Point>::iterator end, const std::size_t dimension){
			
			const std::size_t size = end-begin;

			if(size == 0){
				return;
			}

			std::sort(begin, end, [dimension](const Point& a, const Point& b){
												   	   return a.getCoordinate(dimension) < b.getCoordinate(dimension);
												   });
			
			Point p = *(begin + (size/2));
			
#ifdef DEBUG_TREE
			std::cout<<"[insert] new node split "<<dimension<<" dimension, point="<<p.toString()<<std::endl;
#endif
			node = std::make_unique<Node>(p, dimension);
			
			insert((node.get())->left, begin, (begin + (size/2)), (dimension+1)%(this->dimensions));
			insert((node.get())->right, (begin + (size/2))+1, end, (dimension+1)%(this->dimensions));
		}
		
		std::pair<double, Point> query(const Point& target, const std::unique_ptr<Node>& node, std::pair<double, Point> currBest) const {

			bool isLeaf = false;
			int cmp = target.cmp(node.get()->point, node.get()->dimension);

			if(cmp < 0){
				if(node.get()->left != nullptr){
					std::pair<double, Point> temp = this->query(target, node.get()->left, currBest);
					if(temp.first < currBest.first || currBest.first == -1){
						currBest = temp;
					}
				}else{
					isLeaf = true;
				}
			}else if(cmp >= 0){
				if(node.get()->right != nullptr){
					std::pair<double, Point> temp = this->query(target, node.get()->right, currBest);
					if(temp.first < currBest.first || currBest.first == -1){
						currBest = temp;
					}
				}else{
					isLeaf = true;
				}
			}

			if(isLeaf){
				double distance = target.distance(node.get()->point);
				if(currBest.first == -1 || distance < currBest.first){
					return std::make_pair(distance, node.get()->point);
				}
				return currBest;
			}

			if(fabs(target.getCoordinate(node.get()->dimension) - node.get()->point.getCoordinate(node.get()->dimension)) < currBest.first){
				
				double distanceFromMe = target.distance(node.get()->point);
				if(distanceFromMe < currBest.first){
					currBest = std::make_pair(distanceFromMe, node.get()->point);
				}

				if(cmp < 0){
					if(node.get()->right != nullptr){
						std::pair<double, Point> temp = this->query(target, node.get()->right, currBest);
						if(temp.first < currBest.first){
							currBest = temp;
						}
					}
				}else if(cmp > 0){
					if(node.get()->left != nullptr){
						std::pair<double, Point> temp = this->query(target, node.get()->left, currBest);
						if(temp.first < currBest.first){
							currBest = temp;
						}
					}
				}
			}

			return currBest;
		}

		std::size_t dimensions;
		std::unique_ptr<Node> root;
};

}
