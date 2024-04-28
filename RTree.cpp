#include "RTree.h"
#include <vector>

struct RTree::Point{

	double x;
	double y;
};

struct RTree::Rect{

	Point bottomLeft;
	Point topRight;
};

struct RTree::Node{

	std::size_t count;
	Rect mbr;
	std::vector<Node*> childs;
};

RTree::RTree(std::size_t minNodes, std::size_t maxNodes):
	minNodes(minNodes),
	maxNodes(maxNodes)
{}
