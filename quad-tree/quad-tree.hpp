#ifndef QUADTREE_HPP
#define QUADTREE_HPP

#include <vector>
#include <memory>
#include <algorithm>
//#include <geos/geom/Point.h>
#include "../utils/headers/point.hpp"
#include "../utils/headers/rect.hpp"

namespace bigno{

class QuadTree{

	public:

		QuadTree(const Rect area, std::size_t maxPoints=3){
			this->root = std::make_unique<Node>(area);
			this->MAX_POINTS = maxPoints;
		}

		bool insert(const Point &p){
			
			if(this->root.get() == nullptr){
				return false;
			}

			return this->insert(this->root, p);
		}

		bool insert(){
		
		}

		std::vector<Point> query(const Rect& envelope){
			
			return this->query(this->root, envelope);
		}

	private:

		struct Node{

			Rect area;
			std::vector<std::unique_ptr<Node>> childNodes;
			std::vector<Point> points;
		
			Node(const Rect& area){
				this->area = area;
			}

			bool isLeaf() const {
				return childNodes.empty();
			}
		};

		bool insert(std::unique_ptr<Node>& node, const Point& p){
		
			if(!node->area.contains(p)){
				return false;
			}

			if(node->isLeaf()){
			
				if(node->points.size() < this->MAX_POINTS){
					node->points.push_back(p);
					return true;
				}

				Rect currArea = node->area;
				double leftX = currArea.getBottomLeft().getX();
				double rightX = currArea.getTopRight().getX();
				double topY = currArea.getTopRight().getY();
				double bottomY = currArea.getBottomLeft().getY();
				double middleX = (currArea.getBottomLeft().getX() + currArea.getTopRight().getX())/2.0;
				double middleY = (currArea.getBottomLeft().getY() + currArea.getTopRight().getY())/2.0;

				node->childNodes.push_back(std::make_unique<Node>(Rect(Point({leftX, middleY}), Point({middleX, topY}))));
				node->childNodes.push_back(std::make_unique<Node>(Rect(Point({middleX, middleY}), Point({rightX, topY}))));
				node->childNodes.push_back(std::make_unique<Node>(Rect(Point({leftX, bottomY}), Point({middleX, middleY}))));
				node->childNodes.push_back(std::make_unique<Node>(Rect(Point({middleX, bottomY}), Point({rightX, middleY}))));
			
				for(std::unique_ptr<Node>& n : node->childNodes){
					this->insert(n, p);
				}

				for(const Point& oldPoint : node->points){
					for(std::unique_ptr<Node>& n : node->childNodes){
						this->insert(n, oldPoint);
					}
				}

				node->points.clear();
				return true;

			}else{
				
				for(std::unique_ptr<Node>& n : node->childNodes){
					if(n->area.contains(p)){
						return this->insert(n, p);
					}
				}

				return false;
			}
		}

		std::vector<Point> query(std::unique_ptr<Node>& node, const Rect& envelope) const {
		
			if(!node->area.intersects(envelope)){
				return {};
			}

			std::vector<Point> res;

			if(node->isLeaf()){
				std::copy_if(node->points.begin(), node->points.end(), std::back_inserter(res), [envelope](const Point& p){return envelope.contains(p);});
			}else{
				for(std::unique_ptr<Node>& child : node->childNodes){
					std::vector<Point> childResult = this->query(child, envelope);
					res.insert(res.end(), childResult.begin(), childResult.end());
				}
			}

			return res;
		}

		std::size_t MAX_POINTS;
		std::unique_ptr<Node> root = nullptr;
};

}

#endif //QUADTREE_HPP
