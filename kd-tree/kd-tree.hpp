#include <iostream>
#include <vector>
#include <algorithm>

namespace bigno{

struct Point{

		Point(){
			coordinates = {};
		}

		Point(std::vector<double>& v){
			coordinates = v;
		}

		std::size_t getDimensions() const {
			return coordinates.size();
		}

		std::vector<double> coordinates;
};

class KdTree{

	public:

		KdTree(const std::size_t dimensions, std::vector<Point>& points){
			this->dimensions = dimensions;
			insert(this->root, points.begin(), points.end(), 0);
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
		};

		void insert(Node* node, std::vector<Point>::iterator begin, std::vector<Point>::iterator end, const std::size_t dimension){
			
			const std::size_t size = end-begin;

			if(size == 0){
				return;
			}

			std::sort(begin, end, [dimension](const Point& a, const Point& b){
												   	   return a.coordinates[dimension] > b.coordinates[dimension];
												   });
			
			Point p = *(begin + (size/2));
			std::cout<<"[insert] new node, point=("<<p.coordinates[0]<<", "<<p.coordinates[1]<<")"<<std::endl; 

			node = new Node(p, dimension);

			insert(node->left, begin, (begin + (size/2)), (dimension+1)%(this->dimensions));
			insert(node->right, (begin + (size/2))+1, end, (dimension+1)%(this->dimensions));
		}
		
		std::size_t dimensions;
		Node* root = nullptr;
};

}
