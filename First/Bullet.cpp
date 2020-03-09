#include "Bullet.h"
#include "GLUT.H"
#include <math.h>
#include <stdio.h>

Bullet::Bullet()
{
}

Bullet::Bullet(double x, double y)
{
	double len;
	this->x = x;
	this->y = y;
	this->originalX = x;
	this->originalY = y;
	this->pre_i = -1; this->pre_j = -1;
	this->originTeam = 0;

	dirx = (rand() % 101)-50;
	diry = (rand() % 101)-50;
	len = sqrt(dirx*dirx + diry * diry);
	dirx /= len;
	diry /= len;
	isMoving = false;
}


Bullet::~Bullet()
{
}

void Bullet::showMe()
{
	glColor3d(0, 0, 0);
	glBegin(GL_POLYGON);
	glVertex2d(x - 0.01, y);
	glVertex2d(x , y+ 0.01);
	glVertex2d(x + 0.01, y);
	glVertex2d(x, y - 0.01);
	glEnd();
}

void Bullet::SetIsMoving(bool move)
{
	isMoving = move;
}

bool Bullet::GetIsMoving()
{
	return isMoving;
}

void Bullet::setOriginTeam(int team)
{
	this->originTeam = team;
}

void Bullet::move(Node maze[MSZ][MSZ])
{
	int i, j;
	i = MSZ * (y + 1) / 2;
	j = MSZ * (x + 1) / 2;

	if (isMoving && maze[i][j].GetValue() == SPACE)
	{
		x += 0.002*dirx;
		y += 0.002*diry;
		if (!(pre_i == -1 && pre_j == -1))
		{
			maze[this->pre_i][pre_j].SetValue(SPACE);
		}
		maze[i][j].SetValue(BULLET);
		this->pre_i = i; this->pre_j = j;
	}

	else if (isMoving && maze[i][j].GetValue()==BULLET)
	{
		// continue in that path
		x += 0.002*dirx;
		y += 0.002*diry;
	}
	else if (isMoving && ( (originTeam == 1 && maze[i][j].GetValue() == PLAYER_1) || (originTeam == 2 && maze[i][j].GetValue() == PLAYER_2)))
	{
		// might be the exact origin where the player shot
		// or hit the other team player
		// check this by distance
		double distance = getDistanceFronOriginal();
		if (distance < 0.02)
		{
			// origin location
			x += 0.002*dirx;
			y += 0.002*diry;
		}
		else
		{
			// hit team player - make the bullet inactive
			isMoving = false;
		}
	}
	else if (isMoving && ((originTeam == 2 && maze[i][j].GetValue() == PLAYER_1) || (originTeam == 1 && maze[i][j].GetValue() == PLAYER_2)) )
	{
		// hit. check the damage
		double distance = getDistanceFronOriginal();
		isMoving = false;

		// 3-hard damage 2-medium damage 1-light damage 0-no damage
		if (distance < 0.020)
		{
			// hard damage
			printf("hard %f\n", distance);
			maze[i][j].setHit(30);
		}
		else if (distance < 0.040)
		{
			// medium damage
			printf("medium %f\n", distance);
			maze[i][j].setHit(20);
		}
		else {
			// light damage
			printf("light %f\n", distance);
			maze[i][j].setHit(10);
		}
	}
}

double Bullet::getX()
{
	return x;
}

double Bullet::getY()
{
	return y;
}

void Bullet::SetDir(double angle)
{
	dirx = cos(angle);
	diry = sin(angle);
}

double Bullet::getDistanceFronOriginal()
{
	// sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2));
	return sqrt((originalX-x)*(originalX-x) + (originalY-y)*(originalY-y));
}

void Bullet::SimulateMotion(double map[MSZ][MSZ], Node maze[MSZ][MSZ])
{
	int i, j;
	i = MSZ * (y + 1) / 2;
	j = MSZ * (x + 1) / 2;

	while(maze[i][j].GetValue() == SPACE)
	{
		map[i][j] += delta;
		x += 0.001*dirx;
		y += 0.001*diry;
		i = MSZ * (y + 1) / 2;
		j = MSZ * (x + 1) / 2;
	}

}
