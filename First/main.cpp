#include "GLUT.h"
#include <math.h>
#include <time.h>
#include "Node.h"
#include "Room.h"
#include "Player.h"
#include <vector>
#include <iostream>
#include <queue>
#include "CompareNodes.h"
#include "Bullet.h"
#include "Granade.h"

using namespace std;

const int W = 600; // window width
const int H = 600; // window height

const int NUM_ROOMS = 20;
const int NUM_PLAYERS_EACH_TEAM = 2;

bool run_bfs = false;

Node maze[MSZ][MSZ];
double map[MSZ][MSZ] = { 0 };
Room rooms[NUM_ROOMS];
int numExistingRooms = 0;

//Bullet* pb = NULL;
//Granade* pg;
Player* pl;
bool move_on = false;
bool start_game = false;

Player team_1[NUM_PLAYERS_EACH_TEAM];
Player team_2[NUM_PLAYERS_EACH_TEAM];
bool team_1_is_set = false;
bool team_2_is_set = false;

int meds_num = 0;
int ammu_num = 0;

bool createMapOnlyOnce = true;

vector <Node> gray; // gray nodes

bool l = true;
void SetupMaze();


void init()
{
	srand(time(0)); // pseudo randomizer

	glClearColor(0.7, 0.7, 0.7, 0);

	SetupMaze();

	glOrtho(-1, 1, -1, 1, -1, 1);

}

Room GenerateRoom()
{
	int w, h,ci,cj;
	Room* pr=nullptr;
	bool isOveralaping;

	do
	{
		delete pr;
		isOveralaping = false;
		w = 6 + rand() % 10;
		h = 6 + rand() % 10;

		ci = h / 2 + rand() % (MSZ - h);
		cj = w / 2 + rand() % (MSZ - w);

		pr = new Room(ci, cj, w, h);
//		cout << "check new Room " << "center: (" << ci << "," << cj << "), width: " << w << ", height" << h << endl;
		for (int i = 0; i < numExistingRooms && !isOveralaping; i++)
		{
//			cout << "room # " << i << " ";
//			rooms[i].toString();
			if (rooms[i].CheckOverlapping(pr))
				isOveralaping = true;
			
		}
	} while (isOveralaping);

	// pr is not overlapping with other rooms
	for (int i = pr->getLeftTop().getRow(); i <= pr->getRightBottom().getRow(); i++)
		for (int j = pr->getLeftTop().getCol(); j <= pr->getRightBottom().getCol(); j++)
			maze[i][j].SetValue(SPACE);
	return *pr;
}

// check if the node at row,col is white or gray that is better then the previous one
// and if so add it to pq
void AddNode(int row, int col, Node* pn, vector<Node> &gray, vector<Node> &black,
	priority_queue <Node*, vector<Node*>, CompareNodes> &pq)
{
	Point2D pt;
	Node* pn1;
	vector<Node>::iterator gray_it;
	vector<Node>::iterator black_it;
	double cost = 0;

	pt.setRow(row);
	pt.setCol(col);
	if (maze[row][col].GetValue() == SPACE)
		cost = 0.1; // space cost
	else if (maze[row][col].GetValue() == WALL)
		cost = 3;
	// cost depends on is it a wall or a space
	pn1 = new Node(pt, pn->getTarget(), maze[pt.getRow()][pt.getCol()].GetValue(), pn->getG()+cost, pn);

	black_it = find(black.begin(), black.end(), *pn1);
	gray_it = find(gray.begin(), gray.end(), *pn1);
	if (black_it == black.end() && gray_it==gray.end()) // it is not black and not gray!
	{// i.e. it is white
		pq.push(pn1);
		gray.push_back(*pn1);
	}
}

void AddNeighbours(Node* pn, vector<Node> &gray, vector<Node> &black,
	priority_queue <Node*, vector<Node*>, CompareNodes> &pq)
{
	// try down
	if(pn->getPoint().getRow()<MSZ-1)
		AddNode(pn->getPoint().getRow() + 1, pn->getPoint().getCol(), pn, gray, black, pq);
	// try up
	if (pn->getPoint().getRow() >0)
		AddNode(pn->getPoint().getRow() - 1, pn->getPoint().getCol(), pn, gray, black, pq);
	// try left
	if (pn->getPoint().getCol() > 0)
		AddNode(pn->getPoint().getRow() , pn->getPoint().getCol()- 1, pn, gray, black, pq);
	// try right
	if (pn->getPoint().getCol() <MSZ-1)
		AddNode(pn->getPoint().getRow(), pn->getPoint().getCol() + 1, pn, gray, black, pq);
}

// implement A* from start to target
void GeneratePath(Point2D start, Point2D target)
{
	priority_queue <Node*, vector<Node*>, CompareNodes> pq;
	vector<Node> gray;
	vector<Node> black;
	Node *pn;
	bool stop = false;
	vector<Node>::iterator gray_it;
	vector<Node>::iterator black_it;
	double wall_cost = 10;
	double space_cost = 0.2;
	pn = new Node(start, &target, maze[start.getRow()][start.getCol()].GetValue(), 0, nullptr);
	pq.push(pn);
	gray.push_back(*pn);
	while (!pq.empty()&&!stop)
	{
		// take the best node from pq
		pn = pq.top();
			// remove top Node from pq
		pq.pop();
		if (pn->getPoint() == target) // the path has been found
		{
			stop = true;
			// restore path to dig tunnels
			// set SPACE instead of WALL on the path
			while (!(pn->getPoint() == start))
			{
				maze[pn->getPoint().getRow()][pn->getPoint().getCol()].SetValue(SPACE);
				pn = pn->getParent();
			}
			return;
		}
		else // pn is not target
		{
			// remove Node from gray and add it to black
			gray_it = find(gray.begin(), gray.end(), *pn); // operator == must be implemented in Node
			if (gray_it != gray.end())
				gray.erase(gray_it);
			black.push_back(*pn);
			// check the neighbours
			AddNeighbours(pn, gray, black, pq);
			// try up
/*			if (pn->getPoint().getRow() < MSZ - 1)
			{
				Point2D pt;
				pt.setCol(pn->getPoint().getCol());
				pt.setRow(pn->getPoint().getRow() + 1); // going up
				int value = maze[pt.getRow()][pt.getCol()].GetValue();
				double cost;
				if (value == SPACE) cost = space_cost;
				else cost = wall_cost;
				pn1 = new Node(pt, &target, value, pn->getG() + cost, pn);
				// check if this is not black neighbour
				black_it = find(black.begin(), black.end(), pn1); // operator == must be implemented in Node
				if (black_it != black.end())
				{
					// check if pn1 is gray
					gray_it = find(gray.begin(), gray.end(), pn1); // operator == must be implemented in Node
					if (gray_it != gray.end()) // it is already gray
					{
						// check if pn1 has lower f then what was foud before
						if (pn1->getF() < (*gray_it)->getF())
						{
							(*gray_it) = pn1;
							// and update it in PQ!!!!!
						}
					}
						// add pn1 to pq and to gray
				}
			}*/

		}
	}
}

void DigTunnels()
{
	int i, j;

	for (i = 0; i < NUM_ROOMS; i++)
	{
		cout << "Path from " << i << endl;
		for (j = i + 1; j < NUM_ROOMS; j++)
		{
			cout << " to " << j << endl;
			GeneratePath(rooms[i].getCenter(), rooms[j].getCenter());
		}
	}
}

void spreadResources(int type, int from=0)
{
	for (from; from < 4; from++)
	{
		int i, j;
		do
		{
			i = rand() % MSZ;
			j = rand() % MSZ;
		} while (maze[i][j].GetValue() != SPACE);
		maze[i][j].SetValue(type); 
		type == 7? printf("NED: [ %d, %d]\n", i, j): printf("EMMU: [ %d, %d]\n", i, j);
	}
}

void SetupMaze()
{
	int i, j, k;

	for (i = 0; i < MSZ; i++)
		for (j = 0; j < MSZ; j++)
			maze[i][j].SetValue(WALL);

	for (numExistingRooms = 0; numExistingRooms < NUM_ROOMS; numExistingRooms++)
		rooms[numExistingRooms] = GenerateRoom();

	//Little Walls
	for (k = 0; k < 100; k++)
	{
		i = rand() % MSZ;
		j = rand() % MSZ;

		maze[i][j].SetValue(WALL);

		int num = rand() % 3; // (0-2)
		switch (num)
		{
		case 0:
			maze[i][j].SetValue(WALL);
			maze[i][j + 1].SetValue(WALL);
			maze[i][j + 2].SetValue(WALL);
			break;
		case 1:
			maze[i][j].SetValue(WALL);
			maze[i + 1][j].SetValue(WALL);
			maze[i + 2][j].SetValue(WALL);
			break;
		case 2:
			maze[i][j].SetValue(WALL);
			maze[i + 1][j].SetValue(WALL);
			maze[i][j + 1].SetValue(WALL);
			maze[i + 1][j + 1].SetValue(WALL);
		default:
			break;
		}
	}
	DigTunnels();

	spreadResources(AMMU);
	spreadResources(MED);
}

void DrawMaze()
{
	int i, j;
	double sz, x, y;

	for(i=0;i<MSZ;i++)
		for (j = 0; j < MSZ; j++)
		{
			// set color
			switch (maze[i][j].GetValue())
			{
			case SPACE:
				glColor3d(1, 1, 1); // white
				break;
			case WALL:
				glColor3d(0.6, 0, 0.6); // dark red
				break;
			case START:
				glColor3d(0.4, 0.8, 1); // light blue
				break;
			case TARGET:
				glColor3d(1, 0, 0); // red
				break;
			case MED:
				glColor3d(1, 1, 0); // yellow
				break;
			case AMMU:
				glColor3d(0, 0, 0); // black
				break;
			case path:
				glColor3d(1, 0, 1);
				break;
			case BULLET:
				glColor3d(1, 1, 1);
				break;
			case PLAYER_1:
				glColor3d(1, 1, 1);
				break;
			case PLAYER_2:
				glColor3d(1, 1, 1);
				break;
			default:
				break;
			}
			// draw rectangle
			sz = 2.0 / MSZ;
			x = j * sz -1;
			y = i * sz -1;

			glBegin(GL_POLYGON);
				glVertex2d(x, y);
				glVertex2d(x+sz, y);
				glVertex2d(x + sz, y+sz);
				glVertex2d(x, y + sz);

			glEnd();
		}
}

void DrawMap()
{
	int i, j;
	double sz, xx, yy;

	for (i = 0; i < MSZ; i++)
		for (j = 0; j < MSZ; j++)
		{
			if (maze[i][j].GetValue() == SPACE)
			{
				double c;
				c = 1 - map[i][j];// 1(white) is very safe, 0(black) is very dangerous
				glColor3d(c, c, c);
				// draw rectangle
				sz = 2.0 / MSZ;
				xx = (j * sz - 1);
				yy = i * sz - 1;

				glBegin(GL_POLYGON);
				glVertex2d(xx, yy);
				glVertex2d(xx + sz, yy);
				glVertex2d(xx + sz, yy + sz);
				glVertex2d(xx, yy + sz);

				glEnd();
			}
		}
}

void GenerateMap()
{
	int num_tries = 10000;
	int i;
	int col, row;
	double x, y,sz;
	Granade* pg = nullptr;

	for (i = 0; i < num_tries; i++)
	{
		do
		{
			col = rand() % MSZ;
			row = rand() % MSZ;
		} while (maze[row][col].GetValue() != SPACE);
		sz = 2.0 / MSZ;
		x = col * sz - 1;
		y = row * sz - 1;
		pg = new Granade(x, y);
		pg->SimulateExplosion(map,maze);
	}
}


void display()
{
	glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer

	DrawMaze();

	// only once
	if (createMapOnlyOnce)
	{
		GenerateMap();
		createMapOnlyOnce = false;
	}

	if (pl != NULL)
	{
		pl->showMe();
	}

	if (team_1_is_set)
	{
		for (int i = 0; i < NUM_PLAYERS_EACH_TEAM; i++)
		{
			if(team_1[i].getMode()!=Dead)
				team_1[i].showMe();
		}
	}

	if (team_2_is_set)
	{
		for (int i = 0; i < NUM_PLAYERS_EACH_TEAM; i++)
		{
			if (team_2[i].getMode() != Dead)
				team_2[i].showMe();
		}
	}

	glutSwapBuffers();// show what was drawn in "frame buffer"
}

//void displayMap()
//{
//	glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer
//
//	DrawMaze();
//	DrawMap();
//
//	glutSwapBuffers();// show what was drawn in "frame buffer"
//}

Point2D findClosestTarget(Player player)
{
	int i, j;
	double minDistance = 0.0;
	Point2D rePoint;
	for(i=0;i <MSZ; i++)
		for (j = 0; j < MSZ; j++)
		{
			if ( (maze[i][j].GetValue() == MED && player.getMode() == HealthImproving) || 
				 (maze[i][j].GetValue() == AMMU && player.getMode() == AmmuFilling) ||
				 (maze[i][j].GetValue() == PLAYER_1 && player.getTeam() == 2 && player.getMode() == Fight) ||
				 (maze[i][j].GetValue() == PLAYER_2 && player.getTeam() == 1 && player.getMode() == Fight) || 
				// 1(white) is very safe, 0(black) is very dangerous
				 (maze[i][j].GetValue() == SPACE && player.getMode() == Survive && (1 - map[i][j])>0.7))
			{
				// sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2));
				double currentDistance = sqrt( (double)( (player.getLocation().getRow() -i)*(player.getLocation().getRow() - i)) +
					((player.getLocation().getCol() - j)*(player.getLocation().getCol() -j)));
				if (minDistance == 0.0)
				{
					minDistance = currentDistance;
					rePoint.setRow(i); rePoint.setCol(j);
				}
				else if (currentDistance < minDistance)
				{
					minDistance = currentDistance;
					rePoint.setRow(i); rePoint.setCol(j);
				}
			}
		}
	return rePoint;
}

void fillResources()
{
	int countMeds = 0, countAmmu = 0;
	int i, j;
	for(i=0;i<MSZ; i++)
		for (j=0; j < MSZ; j++)
		{
			if (maze[i][j].GetValue() == MED)
				countMeds++;
			else if (maze[i][j].GetValue() == AMMU)
				countAmmu++;
		}
	spreadResources(MED, countMeds);
	spreadResources(AMMU, countAmmu);
}

void checkHits()
{
	int i, j;
	for(i=0; i<MSZ; i++)
		for (j = 0; j < MSZ; j++)
		{
			// detect hit
			if (maze[i][j].getHit() != 0)
			{
				if (team_1[0].getLocation().getRow() == i && team_1[0].getLocation().getCol() == j)
					team_1[0].loseHealth(maze[i][j].getHit());
				if (team_1[1].getLocation().getRow() == i && team_1[1].getLocation().getCol() == j)
					team_1[1].loseHealth(maze[i][j].getHit());

				if (team_2[0].getLocation().getRow() == i && team_2[0].getLocation().getCol() == j)
					team_2[0].loseHealth(maze[i][j].getHit());
				if (team_2[1].getLocation().getRow() == i && team_2[1].getLocation().getCol() == j)
					team_2[1].loseHealth(maze[i][j].getHit());
				maze[i][j].setHit(0);
			}
		}
}

void checkEndGame()
{
	if (team_1[0].getMode() == Dead && team_1[1].getMode() == Dead)
	{
		start_game = false;
		printf("Game Over!!\n***Team 2 won***\n");
	}
	if (team_2[0].getMode() == Dead && team_2[1].getMode() == Dead)
	{
		start_game = false;
		printf("Game Over!!\n***Team 1 won***\n");
	}

}

void idle()
{

	if (team_1_is_set && team_2_is_set && start_game)
	{
		// Show shooting and granades
		for (int p = 0; p < NUM_PLAYERS_EACH_TEAM; p++)
		{
			team_1[p].AmmuMotion(maze);
			team_2[p].AmmuMotion(maze);
		}
		// make sure that the resources stil around
		fillResources();
		checkHits();
		checkEndGame();


		//if (!team_1[0].IsPathFound())
		//{
		//	// Define target
		//	team_1[0].updateMode();
		//	// Find Target
		//	Point2D target = findClosestTarget(team_1[0]);
		//	// Find Path
		//	team_1[0].findPath(maze, target);
		//}else
		//	team_1[0].move(maze);

		for (int i = 0; i < NUM_PLAYERS_EACH_TEAM; i++) {
			if (!team_1[i].IsPathFound())
			{
				// Define target
				team_1[i].updateMode(maze);
				// Find Target
				Point2D target = findClosestTarget(team_1[i]);
				// Find Path
				team_1[i].findPath(maze, target);
			}
			else
			{
				if (team_1[i].getMode() != Dead) {
					// Find Target
					Point2D target = findClosestTarget(team_1[i]);
					team_1[i].move(maze, target);
				}
			}
		}

		for (int i = 0; i < NUM_PLAYERS_EACH_TEAM; i++) {
			if (!team_2[i].IsPathFound())
			{
				// Define target
				team_2[i].updateMode(maze);
				// Find Target
				Point2D target = findClosestTarget(team_2[i]);
				// Find Path
				team_2[i].findPath(maze, target);
			}
			else
			{
				// Find Target
				if (team_2[i].getMode() != Dead) {
					Point2D target = findClosestTarget(team_2[i]);
					team_2[i].move(maze, target);
				}
			}
		}

	}
	glutPostRedisplay();// calls indirectly to display
}

void Menu(int choice)
{
	if (choice == 1)
	{
		// team 1
		team_1[0] = *pl;
		team_1[0].setTeam(1);
		maze[pl->getLocation().getRow()][pl->getLocation().getCol()].SetValue(PLAYER_1);

		Player* sec = new Player(pl->getX()+0.05, pl->getY());
		team_1[1] = *sec;
		team_1[1].setTeam(1);
		maze[sec->getLocation().getRow()][sec->getLocation().getCol()].SetValue(PLAYER_1);

		team_1_is_set = true;
		pl = NULL;
	}
	else if (choice == 2)
	{
		// team 2
		team_2[0] = *pl;
		team_2[0].setTeam(2);
		maze[pl->getLocation().getRow()][pl->getLocation().getCol()].SetValue(PLAYER_2);

		Player* sec = new Player(pl->getX()+0.05, pl->getY());
		team_2[1] = *sec;
		team_2[1].setTeam(2);
		maze[sec->getLocation().getRow()][sec->getLocation().getCol()].SetValue(PLAYER_2);

		team_2_is_set = true;
		pl = NULL;
	}
	else if (choice == 3)
	{
		// Start Game
		start_game = true;
	}
	else if(choice == 4)
	{
		// Show players info
		team_1[0].info();
		team_1[1].info();
		team_2[0].info();
		team_2[1].info();
	}
	//else if (choice == 5)
	//{ // generate security map
	//	glutDisplayFunc(displayMap);
	//	GenerateMap();
	//}

}

void mouse(int button, int state, int x, int y)
{
	double xx, yy;
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		xx = 2 * (double)x / W - 1;
		yy = 2 * ((double)H - y) / H - 1;

		// pg = new Granade(xx, yy);
		if (!(team_1_is_set && team_2_is_set)) 
			pl = new Player(xx, yy);
	}

}

void main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(W, H);
	glutInitWindowPosition(200, 100);
	glutCreateWindow("Dungeon ");

	glutDisplayFunc(display); // refresh function
	glutIdleFunc(idle); // idle: when nothing happens
	glutMouseFunc(mouse);
	// menu
	glutCreateMenu(Menu);
	glutAddMenuEntry("Team 1", 1);
	glutAddMenuEntry("Team 2", 2);
	glutAddMenuEntry("Start", 3);
	glutAddMenuEntry("Players info", 4);
	//glutAddMenuEntry("Map", 5);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	init();

	glutMainLoop();
}