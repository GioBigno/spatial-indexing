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

	Rect(const Point bottomLeft, const Point topRight, void* data = nullptr){
		this->bottomLeft = bottomLeft;
		this->topRight = topRight;
		this->setData(data);
	}

	Point getBottomLeft(){
		return this->bottomLeft;
	}

	Point getTopRight(){
		return this->topRight;
	}

	void* getData() const {
		return this->data;
	}

	std::string getDataString() const {
		return this->data == nullptr ? "" : *(static_cast<std::string*>(this->getData()));
	}

	void setData(void* data){
		this->data = data;
	}

	bool contains(const Point& p) const {
		return p.getX() >= this->bottomLeft.getX() && 
			   p.getX() <= this->topRight.getX() &&
			   p.getY() >= this->bottomLeft.getY() &&
			   p.getY() <= this->topRight.getY();
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
	void* data;
};

}

#endif //RECT
