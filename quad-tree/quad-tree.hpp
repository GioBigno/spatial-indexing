#include <vector>
#include <memory>
#include "../utils/point.hpp"
#include "../utils/rect.hpp"

namespace bigno{

class QuadTree{


	public:

		QuadTree(const Rect area, std::size_t maxPoints=3){
			this->root = new Node(area);
			this->MAX_POINTS = maxPoints;
		}

		bool insert(Point &p){
			
			Node* currentNode = nullptr;
			
			//check the root is exist and the point is in the tree's area
			if(root != nullptr && root->area.contains(p)){
				currentNode = root;
			}else{
				return false;
			}

			//TODO
			
			return false;
		}

	private:

		struct Node{

			Rect area;
			std::vector<std::unique_ptr<Node>> childNodes;
			std::vector<Point> points;
		
			Node(const Rect &area){
				this->area = area;
			}

			bool isLeaf(){
				return childNodes.empty();
			}
		};

		std::size_t MAX_POINTS;
		Node* root = nullptr;

};

}
