#include "Node.h"

using namespace std;
#pragma once

const int MSZ = 100;
const double delta = 0.0001;

class Bullet
{
public:
	Bullet();
	Bullet(double x, double y);
	~Bullet();
	void showMe();
	void SetIsMoving(bool move);
	bool GetIsMoving();
	void setOriginTeam(int team);
	void move(Node maze[MSZ][MSZ]);
	double getX();
	double getY();
	void SetDir(double angle);
	double getDistanceFronOriginal();
	void SimulateMotion(double map[MSZ][MSZ], Node maze[MSZ][MSZ]);
	
private:
	double x, y;
	double originalX, originalY;
	int pre_i, pre_j;
	double dirx, diry;
	bool isMoving;
	int originTeam;
};

