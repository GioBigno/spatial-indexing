#include <vector>
#include <memory>
#include <algorithm>
#include "../utils/point.hpp"
#include "../utils/rect.hpp"

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

		std::vector<Point> query_envelope(const Rect& envelope){
			
			return this->query_envelope(this->root, envelope);
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

		std::vector<Point> query_envelope(std::unique_ptr<Node>& node, const Rect& envelope) const {
		
			if(!node.get()->area.intersects(envelope)){
				return {};
			}

			std::vector<Point> res;

			if(node.get()->isLeaf()){
				std::copy_if(node.get()->points.begin(), node.get()->points.end(), std::back_inserter(res), [envelope](const Point& p){return envelope.contains(p);});
			}else{
				for(std::unique_ptr<Node>& child : node.get()->childNodes){
					std::vector<Point> childResult = this->query_envelope(child, envelope);
					res.insert(res.end(), childResult.begin(), childResult.end());
				}
			}

			return res;
		}

		std::size_t MAX_POINTS;
		std::unique_ptr<Node> root = nullptr;
};

}
