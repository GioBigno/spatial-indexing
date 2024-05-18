#ifndef RECT_HPP
#define RECT_HPP

#include "point.hpp"

namespace bigno{

class Rect{

	public:

	Rect(){
		this->bottomLeft = Point({0, 0});
		this->topRight = Point({0, 0});
	}

	Rect(const Point bottomLeft, const Point topRight){
		this->bottomLeft = bottomLeft;
		this->topRight = topRight;
	}

	bool contains(const Point& p) const {
		return p.getX() > this->bottomLeft.getX() && 
			   p.getX() < this->topRight.getX() &&
			   p.getY() > this->bottomLeft.getY() &&
			   p.getY() < this->topRight.getY();
	}

	bool contains(const Rect& r) const {
		return this->contains(r.bottomLeft) && this->contains(r.topRight);
	}

	bool intersects(const Rect& r) const {		
		return !((r.bottomLeft.getX() < this->bottomLeft.getX() && r.topRight.getX() < this->bottomLeft.getX()) ||
			   (r.bottomLeft.getX() > this->topRight.getX() && r.topRight.getX() > this->topRight.getX()) ||
			   (r.bottomLeft.getY() < this->bottomLeft.getY() && r.topRight.getY() < this->bottomLeft.getY()) ||
			   (r.bottomLeft.getY() > this->topRight.getY() && r.topRight.getY() > this->topRight.getY()));
	}

	double area() const {
		return (this->topRight.getX() - this->bottomLeft.getX()) * (this->topRight.getY() - this->bottomLeft.getY());
	}

	private:

	Point bottomLeft;
	Point topRight;
};

}

#endif //RECT
