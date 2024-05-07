#include <vector>

struct Point{
	
	double x;
	double y;
};

struct Rect{

	Point bottomLeft;
	Point topRight;
};

class QuadTree{


	public:

		QuadTree(Rect area) const {
			this->root = new Node(area);
		}

		bool insert(Point &p){
			
			Node* currentNode = nullptr;
			
			//check the root is exist and the point is in the tree's area
			if(root != nullptr && root->area.contains(p)){
				currentNode = root;
			}else{
				return false;
			}

			//searching the leaf in which insert the point
			while(!currentNode->isLeaf()){

				bool found = false;
				for(Node* n : currentNode->childs){
					if(n->area.contains(p)){
						currentNode = n;
						found = true;
						break;
					}
				}
				
				if(!found){
					return false;
				}
			}

			//choose to split or not
			if(currentNode->points.size() >= MAX_POINTS){
					
				currentNode->childs.push_back(*(new Node({{currentNode->area.bottomLeft.x, 
													     (currentNode->area.bottomLeft.y + currentNode->area.topRight.y) / 2 
													    },
													    {(currentNode->area.bottomLeft.x + currentNode->area.topRight.x) / 2,
													     (currentNode->area.bottomLeft.y + currentNode->area.topRight.y) / 2
													    }
													   })));


			}



			//add

			

			


		}
		

	private:

		struct Node{

			Rect area;
			std::vector<Node*> childs;
			std::vector<Point> points;
		
			Node(Rect &area){
				this->area = area;
			}

			bool isLeaf(){
				return childs.empty();
			}
		};

		const std::size_t MAX_POINTS = 3;
		Node* root = nullptr;


};
