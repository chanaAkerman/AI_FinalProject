#include "Node.h"
#include <math.h>



Node::Node()
{
	value = SPACE;
	parent = nullptr;
	child = nullptr;
	hit = 0;
	g = 0;
}


Node::~Node()
{
}

Node::Node(Point2D &pt, Point2D* t, int v, double g, Node* pr) {
	point = pt;
	target = t;
	value = v;
	parent = pr;
	this->g = g;
	hit = 0;
	h = ComputeH();
}

void Node::SetValue( int value)
{
	this->value = value;
}

int Node::GetValue()
{
	return value;
}

double Node::getG()
{
	return g;
}



double Node::ComputeH()
{
	return sqrt(pow(point.getRow()-target->getRow(),2)+
		pow(point.getCol() - target->getCol(), 2));
}

double Node::getF()
{
	return g+h;
}

double Node::getH()
{
	return h;
}

void Node::setHit(int hit)
{
	this->hit = hit;
}

int Node::getHit()
{
	return hit;
}

Point2D Node::getPoint()
{
	return point;
}

Node * Node::getParent()
{
	return parent;
}

void Node::setParent(Node* parent) 
{
	this->parent = parent;
}

Point2D * Node::getTarget()
{
	return target;
}

Node* Node::getChild()
{
	return child;
}
void Node::setChild(Node* node)
{
	this->child = node;
}	
