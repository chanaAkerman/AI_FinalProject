#include "Player.h"
#include "GLUT.H"



Player::Player()
{
}

Player::Player(double x, double y)
{
	this->x = x;
	this->y = y;
	this->dir = 0;
	health = HEALTH;
	team = 0;
	mode = Fight;
	restBullets = NUM_OF_BULLETS;
	restGranades = NUM_OF_GRANADES;
	myPath = NULL;
	foundTarget = false;
	foundPath = false;


	//int a;
	//for (a = 0; a < NUM_OF_BULLETS; a++)
	//	bulletsStock[a] = new Bullet(x, y);

	//for (a = 0; a < NUM_OF_GRANADES; a++)
	//	granadesStock[a] = new Granade(x, y);

	// Parameter that keep the maze whole
	pre.setRow(-1); pre.setCol(-1); pre.setpreColor(-1);
	
}

void Player::info()
{
	printf("\n**************************************************\n");
	printf("\n                Player Info\n\n");
	printf("Location: [%d, %d]\n", getLocation().getRow(), getLocation().getCol());
	printf("Team: %d\n", team);
	// 0-Fight 1-Survive 2-Emmu Filling 3-Health Improving 4-Dead
	switch (this->mode)
	{
	case Fight: printf("Mode: Fight \n");
		break;
	case Survive: printf("Mode: Survive \n");
		break;
	case AmmuFilling: printf("Mode: AmmuFilling \n");
		break;
	case HealthImproving: printf("Mode: Health Improving \n");
		break;
	case Dead: printf("Mode: Dead \n");
		break;
	}
	printf("Health: %d\n", health);
	printf("Rest bullets: %d\n", restBullets);
	printf("Rest granades: %d\n", restGranades);
	printf("\n**************************************************\n");
	
}

double Player::getX()
{
	return x;
}

double Player::getY()
{
	return y;
}

Mode Player::getMode()
{
	return mode;
}

void Player::setX(double x)
{
	this->x = x;
}

void Player::setY(double y)
{
	this->y = y;
}

void Player::setMode(Mode mode)
{
	this->mode = mode;
}

int Player::getHealth()
{
	return health;
}

void Player::sethealth(int health)
{
	this->health = health;
}

int Player::getTeam()
{
	return team;
}

void Player::setTeam(int team_number)
{
	// could be one of {1, 2}
	this->team = team_number;
}

void Player::setFoundTarget(bool flag)
{
	this->foundTarget = flag;
}

void Player::setFoundPath(bool flag)
{
	this->foundPath = flag;
}

bool Player::IsTargetFound()
{
	return foundTarget;
}

bool Player::IsPathFound()
{
	return foundPath;
}

// check if the node at row,col is white or gray that is better then the previous one
// and if so add it to pq
void Player::AddNode(Node maze[MSZ][MSZ], int row, int col, Node * pn,
	vector<Node>& gray, vector<Node>& black, priority_queue<Node*, vector<Node*>, CompareNodes>& pq)
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
	{
		cost = 3;
		// Do not add it to pq
		return;
	}
	// cost depends on is it a wall or a space
	pn1 = new Node(pt, pn->getTarget(), maze[pt.getRow()][pt.getCol()].GetValue(), pn->getG() + cost, pn);

	black_it = find(black.begin(), black.end(), *pn1);
	gray_it = find(gray.begin(), gray.end(), *pn1);
	if (black_it == black.end() && gray_it == gray.end()) // it is not black and not gray!
	{// i.e. it is white
		pq.push(pn1);
		gray.push_back(*pn1);
	}
}

void Player::AddNeighbours(Node maze[MSZ][MSZ], Node * pn, vector<Node>& gray,
	vector<Node>& black, priority_queue<Node*, vector<Node*>, CompareNodes>& pq)
{
	// try down
	if (pn->getPoint().getRow() < MSZ - 1) 
		AddNode(maze, pn->getPoint().getRow() + 1, pn->getPoint().getCol(), pn, gray, black, pq);
	// try up
	if (pn->getPoint().getRow() > 0)
		AddNode(maze, pn->getPoint().getRow() - 1, pn->getPoint().getCol(), pn, gray, black, pq);
	// try left
	if (pn->getPoint().getCol() > 0)
		AddNode(maze, pn->getPoint().getRow(), pn->getPoint().getCol() - 1, pn, gray, black, pq);
	// try right
	if (pn->getPoint().getCol() < MSZ - 1)
		AddNode(maze, pn->getPoint().getRow(), pn->getPoint().getCol() + 1, pn, gray, black, pq);

	if (maze[pn->getPoint().getRow() + 1, pn->getPoint().getCol()]->GetValue() + 1 != WALL &&
		maze[pn->getPoint().getRow() - 1, pn->getPoint().getCol()]->GetValue() + 1 != WALL &&
		maze[pn->getPoint().getRow() - 1, pn->getPoint().getCol()]->GetValue() - 1 != WALL &&
		maze[pn->getPoint().getRow() + 1, pn->getPoint().getCol()]->GetValue() - 1 != WALL)
	{
		AddNode(maze, pn->getPoint().getRow() + 1, pn->getPoint().getCol() + 1, pn, gray, black, pq);
		AddNode(maze, pn->getPoint().getRow() - 1, pn->getPoint().getCol() + 1, pn, gray, black, pq);
		AddNode(maze, pn->getPoint().getRow() - 1, pn->getPoint().getCol() - 1, pn, gray, black, pq);
		AddNode(maze, pn->getPoint().getRow() + 1, pn->getPoint().getCol() - 1, pn, gray, black, pq);
	}
}

Point2D Player::getLocation()
{
	// current Location: x, y
	int i, j;
	i = MSZ * (y + 1) / 2;
	j = MSZ * (x + 1) / 2;
	
	Point2D p;
	p.setRow(i); p.setCol(j);
	return p;
}

void Player::findPath(Node maze[MSZ][MSZ], Point2D target)
{
	Point2D start = getLocation();
	this->target = target;

	//// Now Start & Target are set
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
	while (!pq.empty() && !stop)
	{
		// take the best node from pq
		pn = pq.top();
		// remove top Node from pq
		pq.pop();
		if (pn->getPoint() == target) // the path has been found
		{
			printf("Target found\n");
			stop = true;
			// restore path to dig tunnels
			// set SPACE instead of WALL on the path

			// pn - Node target
			Node* current = pn->getParent();
			current->setChild(pn);
			while (!(current->getPoint() == start))
			{
				Node* temp = current->getParent();
				//maze[current->getPoint().getRow()][current->getPoint().getCol()].SetValue(path);
				temp->setChild(current);
				current = temp;
			}
			myPath = current;
			setFoundPath(true);

			// Draw the way
			/*while (!(current->getPoint() == target))
			{
				maze[current->getPoint().getRow()][current->getPoint().getCol()].SetValue(path);
				current = current->getChild();
			}*/
			return;
		}
		else // pn is not target
		{
			// remove Node from gray and add it to black
			gray_it = find(gray.begin(), gray.end(), *pn);
			if (gray_it != gray.end())
				gray.erase(gray_it);
			black.push_back(*pn);
			// check the neighbours
			AddNeighbours(maze, pn, gray, black, pq);
		}
	}
	if (!stop)
	{
		// target is not exist
		printf("Target not exist\n");
	}

}

double Player::getAngle(Node maze[MSZ][MSZ])
{
	double angle = 0;
	int i, j;

	// Calculate the direction according to the next step in the way
	Node* next_Node = myPath->getChild();
	if (next_Node != NULL && next_Node->getPoint().getRow() == getLocation().getRow() && next_Node->getPoint().getCol() == getLocation().getCol())
	{
		myPath = myPath->getChild(); next_Node = myPath->getChild();
		if (next_Node == NULL)
			return -1;
	}
	if (myPath->getH() < 5 && mode == 0) // Fight mode
	{
		return -1;
	}
	// We have not reach target yet
	if (!(next_Node == NULL))
	{
		if (next_Node->getPoint().getRow() < getLocation().getRow() && next_Node->getPoint().getCol() == getLocation().getCol())
		{
			//printf("DOWN\n");
			angle = 4.71;
		}
		else if (next_Node->getPoint().getRow() > getLocation().getRow() && next_Node->getPoint().getCol() == getLocation().getCol())
		{
			//printf("UP\n");
			angle = 1.57;
		}
		else if (next_Node->getPoint().getRow() == getLocation().getRow() && next_Node->getPoint().getCol() < getLocation().getCol())
		{
			//printf("Left\n");
			angle = 3.14;
		}
		else if (next_Node->getPoint().getRow() == getLocation().getRow() && next_Node->getPoint().getCol() > getLocation().getCol())
		{
			//printf("RIGHT\n");
			angle = 0.0;
		}
		else if(next_Node->getPoint().getRow() < getLocation().getRow() && next_Node->getPoint().getCol() < getLocation().getCol())
		{
			//printf("DOWN LEFT\n");
			angle = 3.925;
		}
		else if (next_Node->getPoint().getRow() < getLocation().getRow() && next_Node->getPoint().getCol() > getLocation().getCol())
		{
			//rintf("DOWN RIGHT\n");
			angle = 5.495;
		}
		else if (next_Node->getPoint().getRow() > getLocation().getRow() && next_Node->getPoint().getCol() < getLocation().getCol())
		{
			//printf("UP LEFT\n");
			angle = 2.355;
		}
		else if (next_Node->getPoint().getRow() > getLocation().getRow() && next_Node->getPoint().getCol() > getLocation().getCol())
		{
			//printf("UP RIGHT\n");
			angle = 0.785;
		}
	}
	this->dir = angle;
	return angle;
}

void Player::stockRenewalAmmu()
{
	// Bullets refill
	if (restBullets + 20 > NUM_OF_BULLETS)
	{
		restBullets = NUM_OF_BULLETS;
		int i;
		for (i = 0; i < NUM_OF_BULLETS; i++)
			bulletsStock[i] = new Bullet(x, y);
	}
	else
	{
		restBullets += 20;
		int i;
		for (i = 0; i < restBullets; i++)
			bulletsStock[i] = new Bullet(x, y);
	}

	// Granade refill
	if (restGranades + 5 > NUM_OF_GRANADES)
	{
		restGranades = NUM_OF_GRANADES;
		int i;
		for (i = 0; i < NUM_OF_GRANADES; i++)
			granadesStock[i] = new Granade(x, y);
	}
	else
	{
		restGranades += 5;
		int i;
		for (i = 0; i < restGranades; i++)
			granadesStock[i] = new Granade(x, y);
	}
}

void Player::fillHealth()
{
	// add 20
	if (getHealth() + 20 > 100)
		sethealth(100);
	else sethealth(getHealth() + 20);
}

void Player::loseHealth(int loss)
{
	if (this->health - loss >= 0)
		this->health -= loss;
	else
		mode = Dead;
}

void Player::move(Node maze[MSZ][MSZ], Point2D target)
{
	//targetNearBy(maze);
	if (mode == Fight)
	{
		//int i = rand() % 20;
		//if (i < 10) // refresh the path
		findPath(maze, target);

	}
	double dirx, diry;
	double angle = getAngle(maze);
	if (angle == -1)
	{
		printf("Hit target\n");
		switch (mode)
		{
		case Fight:
		{
			// What to do?
			fight();
			// close enough to the enemy
			
			setFoundTarget(false);
			setFoundPath(false);
		}
		break;
		case Survive: 
		{
			setFoundTarget(false);
			setFoundPath(false);
		}
			break;
		case AmmuFilling:
		{
			stockRenewalAmmu();
			// clear the maze
			maze[myPath->getPoint().getRow()][myPath->getPoint().getCol()].SetValue(SPACE);
			// set Flags
			setFoundTarget(false);
			setFoundPath(false);
			break;
		}
		case HealthImproving:
		{
			fillHealth();
			// clear the maze
			maze[myPath->getPoint().getRow()][myPath->getPoint().getCol()].SetValue(SPACE);
			// set flags
			setFoundTarget(false);
			setFoundPath(false);
		}
		break;
		default:
			break;
		}
		return;
	}

	dirx = cos(angle);
	diry = sin(angle);

	if (maze[getLocation().getRow()][getLocation().getCol()].GetValue() == SPACE || maze[getLocation().getRow()][getLocation().getCol()].GetValue() == WALL
		||maze[getLocation().getRow()][getLocation().getCol()].GetValue() == BULLET )
	{
		x += 0.002*dirx;
		y += 0.002*diry;
		if (this->pre.getRow() == -1)
		{
			// Save the current values for later
			pre.setRow(getLocation().getRow());
			pre.setCol(getLocation().getCol());
			pre.setpreColor(maze[getLocation().getRow()][getLocation().getCol()].GetValue());
			// Draw the new point int player color
			maze[getLocation().getRow()][getLocation().getCol()].SetValue(team == 1 ? PLAYER_1 : PLAYER_2);
		}
		else
		{
			// Draw the previous point in the original color
			maze[pre.getRow()][pre.getCol()].SetValue(pre.getpreColor());
			// Save the current values for later
			pre.setRow(getLocation().getRow());
			pre.setCol(getLocation().getCol());
			pre.setpreColor(maze[getLocation().getRow()][getLocation().getCol()].GetValue());
			// Draw the new point int player color
			maze[getLocation().getRow()][getLocation().getCol()].SetValue(team == 1 ? PLAYER_1 : PLAYER_2);
		}
	}
	/*else if (maze[getLocation().getRow()][getLocation().getCol()].GetValue() == (team == 1 ? PLAYER_1 : PLAYER_2) || 
		maze[getLocation().getRow()][getLocation().getCol()].GetValue() == (team == 2 ? PLAYER_2 : PLAYER_1))*/
	else if(maze[getLocation().getRow()][getLocation().getCol()].GetValue() == PLAYER_1 || maze[getLocation().getRow()][getLocation().getCol()].GetValue() == PLAYER_2)
	{
		x += 0.002*dirx;
		y += 0.002*diry;
	}
	// Found Ammu by eccident, player will take it
	else if (maze[getLocation().getRow()][getLocation().getCol()].GetValue() == AMMU)
	{
		maze[getLocation().getRow()][getLocation().getCol()].SetValue(team == 1 ? PLAYER_1 : PLAYER_2);
		stockRenewalAmmu();
	}
	// Found Med by eccident, player will take it
	else if (maze[getLocation().getRow()][getLocation().getCol()].GetValue() == MED)
	{
		maze[getLocation().getRow()][getLocation().getCol()].SetValue(team == 1 ? PLAYER_1 : PLAYER_2);
		fillHealth();
	}

}

void Player::shoot(double dir)
{
	if (restBullets - 1 >= 0) {
		restBullets--;
		bulletsStock[restBullets] = new Bullet(x, y);
		bulletsStock[restBullets]->setOriginTeam(team);
		bulletsStock[restBullets]->SetDir(dir);
		bulletsStock[restBullets]->SetIsMoving(true);
	}//else : out of bullets..
}

void Player::moveBullets(Node maze[MSZ][MSZ])
{
	for (int i = restBullets; i < NUM_OF_BULLETS; i++)
		bulletsStock[i]->move(maze);
}

void Player::showBullets()
{
	for (int i = restBullets; i < NUM_OF_BULLETS; i++)
	{
		if(bulletsStock[i]->GetIsMoving())
			bulletsStock[i]->showMe();
	}
}

void Player::throwGranade()
{
	if (restGranades - 1 >= 0) {
		restGranades--;
		granadesStock[restGranades] = new Granade(x, y);
		granadesStock[restGranades]->setOriginTeam(team);
		granadesStock[restGranades]->explode();
	}//else : out of granades..
}

void Player::moveGranadesBullets(Node maze[MSZ][MSZ])
{
	for (int i = restGranades; i < NUM_OF_GRANADES; i++)
		granadesStock[i]->moveBullets(maze);
}

void Player::showGranades()
{
	for (int i = restGranades; i < NUM_OF_GRANADES; i++)
		granadesStock[i]->showMe();
}

void Player::fight()
{
	if (restGranades == 0 && restBullets > 0) {
		shoot(this->dir);
		return;
	}
	if (restGranades > 0 && restBullets == 0) {
		throwGranade();
		return;
	}

	int ch = rand() % 2;
	if (ch == 0) {
		throwGranade();
	}
	else {
		shoot(this->dir);
	}

}

void Player::targetNearBy(Node maze[MSZ][MSZ])
{
	int i, j;
	for(i=0; i<MSZ; i++)
		for (j = 0; j < MSZ; j++)
		{
			double distance = sqrt(((getLocation().getRow() - i)*(getLocation().getRow() - i)) - ((getLocation().getCol() - j)*(getLocation().getCol() - j)));
			if ( (maze[i][j].GetValue() == PLAYER_1 && this->team == 2 && distance<0.05) || (maze[i][j].GetValue() == PLAYER_2 && this->team == 1 && distance < 0.05))
			{
				throwGranade();
			}
		}
}

void Player::updateMode(Node maze[MSZ][MSZ])
{	
	int pre = getMode();
	
	if (health < 30)
		this->mode = HealthImproving;
	else if (health <= 10)
		this->mode = Survive;
	else if (restBullets + restGranades < 20)
		this->mode = AmmuFilling;
	else if(pre == Fight)
	{
		// something different then a Fight
		int randomMode = rand() % 4;
		switch (randomMode)
		{
		case 0: this->mode = Fight;
			break;
		case 1: this->mode = Survive;
			break;
		case 2: this->mode = AmmuFilling;
			break;
		case 3: this->mode = HealthImproving;
			break;
		default: this->mode = Fight;
			break;
		}
	}
	else
		this->mode = Fight;
	printf("new mode: %d\n", this->mode);

}

void Player::showMe()
{
	if (team == 1)
		glColor3d(1, 0, 0); // red
	else if (team == 2)
		glColor3d(0, 0.5, 0); // green
	else if (team == 0)
		glColor3d(0, 0, 0); // black
	
	glBegin(GL_POLYGON);
	glVertex2d(x - 0.015, y);
	glVertex2d(x, y + 0.015);
	glVertex2d(x + 0.015, y);
	glVertex2d(x, y - 0.015);
	glEnd();

	showBullets();
	showGranades();
}

void Player::AmmuMotion(Node maze[MSZ][MSZ])
{
	moveBullets(maze); 
	moveGranadesBullets(maze); 
}

Player::~Player()
{
}
