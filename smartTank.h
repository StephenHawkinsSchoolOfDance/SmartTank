#pragma once
#include "aitank.h"
#include "node.h"
#include "map.h"
#include "defs.h"
#include <iostream>

class smartTank : public AITank
{
private:
	sf::RectangleShape tankRect; // smart tank shape
	Node* currentNode; // will be current location node
	Node* targetNode; // target location node
	std::list<Node> path; //list containing node objects / used with A* 
						  // bool checks
	bool runFlag = false;
	bool crashed;
	bool enemySpotted = false;
	bool enemyBaseSpotted = false;
	bool friendlyBaseSpotted = false;
	bool lineOfSight = false;
	bool firing = false;
	// counters
	float enemyDistance = 1000;
	int enemyBasesCount = 10;
	int friendlyBaseCount = 10;
	int scanCount = 0;
	int turretAngle = 0;
	int angle2Degree = 0;
	int baseAngle = 0;
	int baseAngle2Degree = 0;
public:
	smartTank();
	~smartTank();
	//Node* node; // might be important?!?!
	Map* map;
	// same methods as dumb tank
	void move();
	void reset();
	void collided();
	void markTarget(Position p);
	void markEnemy(Position p);
	void markBase(Position p);
	void markShell(Position p);
	bool isFiring();
	void score(int thisScore, int enemyScore);
	// added states
	void setCurrentNode();
	void movementHandler();
	void navigationHandler();
	void turretHandler();
	void aimingHandler();
};