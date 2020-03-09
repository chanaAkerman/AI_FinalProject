#include "Bullet.h"
#include "Granade.h"
#include <queue>
#include <vector>
#include "Node.h"
#include "CompareNodes.h"

using namespace std;

#pragma once

const int NUM_OF_BULLETS = 100;
const int NUM_OF_GRANADES = 20;
const int HEALTH = 100;

enum Mode { Fight=0, Survive=1, AmmuFilling=2 ,HealthImproving=3, Dead=4 };

class Player
{
public:
	Player();
	Player(double x, double y);
	void showMe();
	
	double getX();
	double getY();
	Mode getMode();
	void setX(double x);
	void setY(double y);
	void setMode(Mode mode);
	void info();
	
	int getHealth();
	void sethealth(int health);
	int getTeam();
	void setTeam(int team_number);
	void setFoundTarget(bool flag);
	void setFoundPath(bool flag);
	bool IsTargetFound();
	bool IsPathFound();

	void stockRenewalAmmu();
	void fillHealth();
	void loseHealth(int loss);

	void move(Node maze[MSZ][MSZ], Point2D target);
	void targetNearBy(Node maze[MSZ][MSZ]);
	
	void shoot(double dir);
	void moveBullets(Node maze[MSZ][MSZ]);
	void showBullets();
	void fight();
	
	void throwGranade();
	void moveGranadesBullets(Node maze[MSZ][MSZ]);
	void showGranades();
	
	void updateMode(Node maze[MSZ][MSZ]);
	void AmmuMotion(Node maze[MSZ][MSZ]);

	Point2D getLocation();
	double getAngle(Node maze[MSZ][MSZ]);


	void findPath(Node maze[MSZ][MSZ], Point2D target);
	void AddNeighbours(Node maze[MSZ][MSZ], Node* pn, vector<Node> &gray, vector<Node> &black,
		priority_queue <Node*, vector<Node*>, CompareNodes> &pq);
	void AddNode(Node maze[MSZ][MSZ], int row, int col, Node* pn, vector<Node> &gray, vector<Node> &black,
		priority_queue <Node*, vector<Node*>, CompareNodes> &pq);
	~Player();
private:
	double x, y;
	double dir;
	int health;
	Mode mode;

	Node* myPath;

	int restBullets;
	int restGranades;
	Bullet* bulletsStock[NUM_OF_BULLETS];
	Granade* granadesStock[NUM_OF_GRANADES];
	int team;
	bool foundTarget;
	bool foundPath;
	Point2D pre;
	Point2D target;
};

