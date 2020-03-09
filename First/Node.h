#pragma once
#include "Point2D.h"

const int SPACE = 1;
const int WALL = 2;
const int START = 3;
const int TARGET = 4;
const int PATH = 5; // belongs to the path to target
const int GRAY = 6; // Fringe
const int BLACK = 7; // VISITED
const int MED = 8;
const int AMMU = 9;
const int PLAYER_1 = 10;
const int PLAYER_2 = 11;
const int BULLET = 12;
const int path = 13;

class Node
{
public:
	Node();
	~Node();

	Node(Point2D & pt, Point2D * t, int v, double g, Node * pr);

private:
	int value;
	double h, g;
	Node* parent;
	Node* child;
	Point2D* target;
	Point2D point;
	int hit;


public:
	void SetValue(int value);
	int GetValue();
	double getG();
	double ComputeH();
	double getF();
	double getH();
	void setHit(int hit);
	int getHit();
	Point2D getPoint();
	Node* getParent();
	void setParent(Node* parent);
	Point2D* getTarget();
	Node* getChild();
	void setChild(Node* child);
	bool operator == (const Node &other) {
		return point == other.point;
	}
};

