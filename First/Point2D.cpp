#include "Point2D.h"



Point2D::Point2D()
{
}


Point2D::~Point2D()
{
}

int Point2D::getRow()
{
	return row;
}
int Point2D::getCol() {
	return col;
}
void Point2D::setRow(int r) {
	row = r;
}
void Point2D::setCol(int c)
{
	col = c;
}

void Point2D::setHit(int h)
{
	this->hit = h;
}

int Point2D::getHit()
{
	return this->hit;
}

int Point2D::getpreColor()
{
	return preColor;
}

void Point2D::setpreColor(int color)
{
	this->preColor = color;
}
