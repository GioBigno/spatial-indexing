#include <cstddef>

class RTree{

	private:

	struct Point;
	struct Rect;
	struct Node;
	
	std::size_t minNodes;
	std::size_t maxNodes;

	public:

	RTree(std::size_t minNodes, std::size_t maxNodes);
	void insert(Rect a);
};

