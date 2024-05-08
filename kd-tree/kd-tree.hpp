#include <iostream>
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

		Point(std::vector<double>& v, void* data = nullptr){
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

		int cmp(const Point target, const std::size_t dimension) const {
			double ret = this->coordinates[dimension] - target.coordinates[dimension];

			if(fabs(ret) < std::numeric_limits<double>::epsilon()){
				return 0;
			}else{
				return ret < 0 ? -1 : 1;
			}
		}

		std::string toString(){
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
		
		std::vector<Point> query_neighbors(const Point start, const std::size_t numNeighbors) const {

			if(this->root == nullptr){
				return {};
			}

			Node* targetLeaf = this->root;
			bool finished = false;

			while(targetLeaf != nullptr && !finished){

				std::cout<<"current target: "<<targetLeaf->point.getDataString()<<std::endl;

				int cmp = start.cmp(targetLeaf->point, targetLeaf->dimension);

				if(cmp < 0){
					if(targetLeaf->left != nullptr){
						targetLeaf = targetLeaf->left;
					}else{
						finished = true;
					}
				}else if(cmp > 0){
					if(targetLeaf->right != nullptr){
						targetLeaf = targetLeaf->right;
					}else{
						finished = true;
					}
				}
			}

			std::cout<<"targetLeaf: "<<targetLeaf->point.toString()<<std::endl;
			
			//temp
			return {};
		}

	private:

		struct Node{
			Point point;
			std::size_t dimension;
			Node* left = nullptr;
			Node* right = nullptr;

			Node(const Point point, const std::size_t dimension){
				this->point = point;
				this->dimension = dimension;
			}

			bool isLeaf(){
				return this->left == nullptr && this->right == nullptr;
			}

		};

		void insert(Node*& node, std::vector<Point>::iterator begin, std::vector<Point>::iterator end, const std::size_t dimension){
			
			const std::size_t size = end-begin;

			if(size == 0){
				return;
			}

			std::sort(begin, end, [dimension](const Point& a, const Point& b){
												   	   return a.getCoordinate(dimension) < b.getCoordinate(dimension);
												   });
			
			Point p = *(begin + (size/2));
			std::cout<<"[insert] new node split "<<dimension<<" dimension, point="<<p.toString()<<std::endl;

			//std::cout<<"based on: "<<std::endl;
			//for(auto i = begin; i != end; i++){
			//	std::cout<<(*i).toString()<<std::endl;
			//}

			node = new Node(p, dimension);
			
			insert(node->left, begin, (begin + (size/2)), (dimension+1)%(this->dimensions));
			insert(node->right, (begin + (size/2))+1, end, (dimension+1)%(this->dimensions));
		}
		
		std::size_t dimensions;
		Node* root = nullptr;
};

}
