#ifndef KDTREE_HPP
#define KDTREE_HPP

#include <utility>
#include <vector>
#include <cmath>
#include <algorithm>
#include <memory>
#include "../utils/headers/point.hpp"

namespace bigno{

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
			
			node = std::make_unique<Node>(p, dimension);
			
			insert(node->left, begin, (begin + (size/2)), (dimension+1)%(this->dimensions));
			insert(node->right, (begin + (size/2))+1, end, (dimension+1)%(this->dimensions));
		}
		
		std::pair<double, Point> query(const Point& target, const std::unique_ptr<Node>& node, std::pair<double, Point> currBest) const {

			bool isLeaf = false;
			int cmp = target.cmp(node->point, node->dimension);

			if(cmp < 0){
				if(node->left != nullptr){
					std::pair<double, Point> temp = this->query(target, node->left, currBest);
					if(temp.first < currBest.first || currBest.first == -1){
						currBest = temp;
					}
				}else{
					isLeaf = true;
				}
			}else if(cmp >= 0){
				if(node->right != nullptr){
					std::pair<double, Point> temp = this->query(target, node->right, currBest);
					if(temp.first < currBest.first || currBest.first == -1){
						currBest = temp;
					}
				}else{
					isLeaf = true;
				}
			}

			if(isLeaf){
				double distance = target.distance(node->point);
				if(currBest.first == -1 || distance < currBest.first){
					return std::make_pair(distance, node->point);
				}
				return currBest;
			}

			if(fabs(target.getCoordinate(node->dimension) - node->point.getCoordinate(node->dimension)) < currBest.first){
				
				double distanceFromMe = target.distance(node->point);
				if(distanceFromMe < currBest.first){
					currBest = std::make_pair(distanceFromMe, node->point);
				}

				if(cmp < 0){
					if(node->right != nullptr){
						std::pair<double, Point> temp = this->query(target, node->right, currBest);
						if(temp.first < currBest.first){
							currBest = temp;
						}
					}
				}else if(cmp > 0){
					if(node->left != nullptr){
						std::pair<double, Point> temp = this->query(target, node->left, currBest);
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

#endif //KDTREE_HPP
