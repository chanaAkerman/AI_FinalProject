#pragma once
#include "Bullet.h"
#include "Node.h"

const int NUM_BULLETS = 16;

class Granade
{
public:
	Granade();
	Granade(double x, double y);
	~Granade();
	void explode();
	void showMe();
	void setOriginTeam(int team);
	void moveBullets(Node maze[MSZ][MSZ]);
	void SimulateExplosion(double map[MSZ][MSZ], Node maze[MSZ][MSZ]);
private:
	double x, y;
	int originTeam;
	Bullet* bullets[NUM_BULLETS];

};

