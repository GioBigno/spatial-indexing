#include <vector>
#include <memory>
#include "../utils/point.hpp"
#include "../utils/rect.hpp"

#include <iostream>

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

		void debug(){
			this->debug(this->root);
		}

	private:

		struct Node{

			Rect area;
			std::vector<std::unique_ptr<Node>> childNodes;
			std::vector<Point> points;
		
			Node(const Rect& area){
				this->area = area;
			}

			bool isLeaf(){
				return childNodes.empty();
			}
		};

		void debug(std::unique_ptr<Node>& node){

			std::cout<<"node: "<< node.get()->area.getBottomLeft().toString() << ", " << node.get()->area.getTopRight().toString() <<std::endl;
			std::cout<<"points: "<<std::endl;
			for(Point p : node.get()->points){
				std::cout<<p.toString()<<" ";
			}
			std::cout<<std::endl;
			std::cout<<"childs: "<<node.get()->childNodes.size()<<std::endl;

			for(std::unique_ptr<Node>& n : node.get()->childNodes){
				this->debug(n);
			}
		}

		bool insert(std::unique_ptr<Node>& node, const Point& p){
		
			if(!node.get()->area.contains(p)){
				return false;
			}

			if(node.get()->isLeaf()){
			
				if(node.get()->points.size() < this->MAX_POINTS){
					node.get()->points.push_back(p);
					return true;
				}

				Rect currArea = node.get()->area;
				double leftX = currArea.getBottomLeft().getX();
				double rightX = currArea.getTopRight().getX();
				double topY = currArea.getTopRight().getY();
				double bottomY = currArea.getBottomLeft().getY();
				double middleX = (currArea.getBottomLeft().getX() + currArea.getTopRight().getX())/2.0;
				double middleY = (currArea.getBottomLeft().getY() + currArea.getTopRight().getY())/2.0;

				node.get()->childNodes.push_back(std::make_unique<Node>(Rect(Point({leftX, middleY}), Point({middleX, topY}))));
				node.get()->childNodes.push_back(std::make_unique<Node>(Rect(Point({middleX, middleY}), Point({rightX, topY}))));
				node.get()->childNodes.push_back(std::make_unique<Node>(Rect(Point({leftX, bottomY}), Point({middleX, middleY}))));
				node.get()->childNodes.push_back(std::make_unique<Node>(Rect(Point({middleX, bottomY}), Point({rightX, middleY}))));
			
				for(std::unique_ptr<Node>& n : node.get()->childNodes){
					this->insert(n, p);
				}

				for(const Point& oldPoint : node.get()->points){
					for(std::unique_ptr<Node>& n : node.get()->childNodes){
						this->insert(n, oldPoint);
					}
				}

				node.get()->points.clear();
				return true;

			}else{
				
				for(std::unique_ptr<Node>& n : node.get()->childNodes){
					if(n.get()->area.contains(p)){
						return this->insert(n, p);
					}
				}

				return false;
			}
		}

		std::size_t MAX_POINTS;
		std::unique_ptr<Node> root = nullptr;

};

}
