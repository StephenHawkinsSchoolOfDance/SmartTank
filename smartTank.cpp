#include "smartTank.h"

smartTank::smartTank()
{
	map = new Map();
	clearMovement();
}

smartTank::~smartTank()
{
	// clean up please!!
	//map = nullptr;
	//delete map;
	currentNode = nullptr;
	delete currentNode;
	//targetNode = nullptr;
	//delete targetNode;
}

void smartTank::move()
{
	tankRect.setSize(sf::Vector2f(200, 10)); // set size of tank
	if (friendlyBaseSpotted == true)tankRect.setFillColor(sf::Color(255, 0, 0, 50));
	tankRect.setRotation(turretTh); // set tank rotation to turret direction
									// updating here / states machines handled here 
	setCurrentNode(); // finds wherte the player tank is
	turretHandler();
	aimingHandler();
	navigationHandler(); // find next move
	movementHandler();
}

void smartTank::reset()
{
	path.clear(); //clear the old path 
	runFlag = false;
}

void smartTank::collided()
{
}

void smartTank::markTarget(Position p)
{
	//std::cout << "Target spotted at (" << p.getX() << ", " << p.getY() << ")\n";
	enemyBaseSpotted = true;
	sf::Vector2f delta(getX() - p.getX(), getY() - p.getY());

	angle2Degree = atan2(delta.y, delta.x) * 180 / PI;
	turretAngle = angle2Degree + 180;
}

void smartTank::markEnemy(Position p)
{
	// because enemy tank is dynamic i will do some calcs here as they will be constanly updated
	std::cout << "Target spotted at (" << p.getX() << ", " << p.getY() << ")\n"; // debug / testing
	enemySpotted = true;
	// find tank point and enemey points
	sf::Vector2i delta(getX() - p.getX(), getY() - p.getY());
	angle2Degree = atan2(delta.y, delta.x) * 180 / PI; // find angle between the two point
													// set angle to variable as it's used as dynmaic 
	turretAngle = angle2Degree + 180; // add 180 else turret is backwards
							   // next work out distance to offset aiming or states
	sf::Vector2f dist(p.getX() - getX(), p.getY() - getY());
	// now we have dynmic updated range finder, used in aiming or state machines 
	enemyDistance = sqrt((dist.x * dist.x) + (dist.y * dist.y));
}

void smartTank::markBase(Position p)
{
	sf::Vector2f delta (getX() - p.getX(), getY() - p.getY()) ;
	sf::Vector2f vec1((p.getX() - getX()) * (p.getX() - getX()), (p.getY() - getY()) * (p.getY() - getY())); //check brackets
	
	baseAngle2Degree = atan2(delta.y, delta.x) * 180 / PI;
	baseAngle = baseAngle2Degree + 180;

	float distance = sqrt(vec1.x + vec1.y);
	float range = 0;

	if (distance <= 50) range = 50;
	if (distance > 50 && distance <= 100) range = 25;
	if (distance > 100) range = 12.5;
}

void smartTank::markShell(Position p)
{
}

void smartTank::setCurrentNode()
{
	// find player pos
	sf::Vector2i pPos((getX() - (map->getNodeX() / 2)) / map->getNodeX(),
		(getY() - (map->getNodeY() / 2)) / map->getNodeY());
	// clean up / set every node current to false
	for (int i = 0; i < map->getRows(); i++)
		for (int j = 0; j < map->getColumns(); j++)
			map->mapArray[i][j]->setCurrent(false);
	// set node player is in to current
	map->mapArray[pPos.y][pPos.x]->setCurrent(true);
	currentNode = map->mapArray[pPos.y][pPos.x]; // add reference to our new current node
}

void smartTank::turretHandler()
{
	const int IDLE = 0, FIRING = 1;
	int state = IDLE;
	if ((enemySpotted != false || enemyBaseSpotted != false) && hasAmmo() && lineOfSight != false)
	{
		state = FIRING;
	}
	else {
		firing = false; state = IDLE;
	}
	switch (state) {
	case IDLE: return; break;
	case FIRING: 
		scanCount = 0; firing = true; enemySpotted = false;
	enemyBaseSpotted = false; lineOfSight = false; break;
	}
}

void smartTank::aimingHandler()
{
	const int SCAN = 0, AIM = 1;
	int scanState = SCAN;
	if (scanState == SCAN && friendlyBaseSpotted != true)
		if (enemySpotted == true) scanState = AIM;
		else if (enemyBaseSpotted == true) scanState = AIM;
		if (scanState == AIM && (enemySpotted != true || enemyBaseSpotted != true) && friendlyBaseSpotted != false)
			scanState == SCAN;
		if (scanState == AIM && friendlyBaseSpotted != false) scanState == SCAN;
		//State Switch
		switch (scanState) {
		case SCAN: turretGoRight(); std::cout << "SCAN" << std::endl; break;
		case AIM:
			std::cout << "AIM" << std::endl;
			if (friendlyBaseSpotted != true) {
				float deltaR = turretTh - turretAngle;
				if (deltaR > 1 && deltaR < 180) {
					turretGoLeft();
				}
				else if (deltaR < -1 && deltaR > -180) {
					turretGoRight();
				}
				else if (deltaR > 180) {
					turretGoRight();
				}
				else if (deltaR < -180) {
					turretGoLeft();
				}
				else {
					lineOfSight = true; stopTurret(); clearMovement();
				}
			} break;
		}
}

void smartTank::movementHandler()
{
	// setting states
	const int IDLE = 0, MOVE = 1, ROTATE = 2;
	int state = IDLE;
	std::string states[3] = { "IDLE", "MOVE", "ROTATE" };
	// 
	if (path.size() != 0) {
		targetNode = &path.front();
		if (targetNode->column == currentNode->column)
			if (targetNode->row == currentNode->row)
				path.pop_front(); if (path.size() != 0)
				targetNode = &path.front();
	}
	// the distance needed to move
	sf::Vector2f delta(getX() - targetNode->getX(), getY() - targetNode->getY());
	angle2Degree = atan2(delta.y, delta.x) * 180 / PI; // rad to deg calc
													   // final angle calc + current AI tank rotation + final angle
	float finalAngle = angle2Degree + 180, rotation = pos.getTh(), deltaR = rotation - finalAngle;
	//local vars
	int distanceThreshold = 5, directionalThreshold = 5;
	bool facingDirection = (deltaR < directionalThreshold && deltaR > -directionalThreshold);
	bool atLocation = (path.size() == 0);
	int direction;
	// State Transitions
	if (state == IDLE)
		if (facingDirection && !atLocation) {
			state = MOVE;
		}
		else if (!facingDirection && !atLocation) {
			state = ROTATE;
		}
		else
		{
			scanCount++;
		}
	else if (state == MOVE)
		if (atLocation)	state = IDLE;
		else if (state == ROTATE)
			if (facingDirection) state = MOVE;
	//State Switch 
	switch (state) {
	case IDLE: stop(); std::cout << state << std::endl;  break;
	case MOVE: goForward(); std::cout << state << std::endl; break;
	case ROTATE:
		std::cout << state << std::endl;
		if (deltaR > 1 && deltaR < 180) goLeft();
		else if (deltaR < -1 && deltaR > -180) goRight();
		else if (deltaR > 180) goRight();
		else if (deltaR < -180) goLeft(); break;
	}
}

void smartTank::navigationHandler()
{
	// first call of method is a map discovery method // will add second search here
	while (!runFlag) {
		// pick random location to navigate to
		int randXLoc = rand() % 13 + 2, randYLoc = rand() % 5 + 2;
		// do search // use A*
		if (map->mapArray[randXLoc][randYLoc]->Path != false)
			map->aStar(path, *currentNode, *map->mapArray[randXLoc][randYLoc]); runFlag = true;
		// turn off flag / will be in try catch to ensure tank moves first time
	}
	// setting up states // will change syntax
	const int IDLE = 0, MOVE_RAND = 1, MOVE_AWAY = 2, MOVE_CENTRE = 3;
	int state = IDLE;
	// limits
	int enemyRangeLimit = 100, wallRangeLimit = 10;
	// next part is transition conditionals
	if (state == IDLE) {
		// if enemy is close or spotted move away // probably should add shell amount check
		if (enemyDistance < enemyRangeLimit || (enemySpotted == true && numberOfShells > enemyBasesCount)) {
			state = MOVE_AWAY;
			//std::cout << "State Changed -> MOVE_AWAY \n";
		}
		// check if enemy base has been spotted or not enough info has been gathered 
		else if (scanCount > 300 && enemyBaseSpotted == false) {
			runFlag = false; // turn off flag
			state = MOVE_RAND;
			//std::cout << "State Changed -> MOVE_RAND \n";
		}
	}
	else if (state == MOVE_AWAY) {
		// when distance between players is too much then AFK
		if (enemyDistance < enemyRangeLimit) {
			state = IDLE;
			//std::cout << "State Changed -> IDLE \n";
		}
		// unless tank is at the boarder
		else if (getX() < map->mapBuff || getX() > (map->getMapX() - map->mapBuff) ||
			getY() < map->mapBuff || getY() > (map->getMapY() - map->mapBuff)) {
			runFlag = false;
			state = MOVE_CENTRE;
			//std::cout << "State Changed -> MOVE_CENTRE \n";
			
		}
	}
	else if (state == MOVE_RAND) {
		// let tank roam until he sees target / stop and ready fire / if ammo
		if (enemyDistance < enemyRangeLimit || enemySpotted == true && numberOfShells > enemyBasesCount)
			clearMovement();
		else {
			state = IDLE;
			//std::cout << "State Changed -> IDLE \n";
		}
	}
	// state machine
	switch (state) {
		// if IDLE clear movements
	case IDLE: {
		clearMovement();
	} break;
		// if tank is close then move away
	case MOVE_AWAY: {
		sf::Vector2i move(currentNode->row, currentNode->column); // needs to be populated by set current node method
																  // while run not true / depending on turrent angle change to new direction
		while (!runFlag) {
			// add check for out of bounds mapArray[16][22]
			if (turretAngle >= 180) { // more than 180 // unsure if i should limit turret angle to 360
				if (move.x <= 15)
					move.x = move.x + 1;
				if (move.y >= 1)
					move.y = move.y - 1;
			}
			else { // less than 180
				if (move.x >= 1)
					move.x = move.x - 1;
				if (move.y <= 21)
					move.y = move.y + 1;
			} // might add more so it can go to each 4 courners if time depending on turret angle
			if (map->mapArray[move.x][move.y]->Path != false)
				map->aStar(path, *currentNode, *map->mapArray[move.x][move.y]);	runFlag = true; // turn off if we create a path else loop again
																								// create path here with A*
		}
	} break;
		// mainly for when tank collides with walls or tank gets lost
	case MOVE_CENTRE: {
		sf::Vector2i center(map->getRows() / 2, (map->getColumns() / 2) - 1); // take 1 for sfml shapes
		while (!runFlag) {
			// do A* create a path
			if (map->mapArray[center.x][center.y]->Path != false)
				map->aStar(path, *currentNode, *map->mapArray[center.x][center.y]); runFlag = true;
			// added so if middle is occup
			center.x++;
			center.y++;
		}
	} break;
	case MOVE_RAND: {
		scanCount = 0;
		sf::Vector2i randLoc(0, 0);
		while (!runFlag)
		{
			// if past half way top, check if left or right / else in bottom 
			if (currentNode->row <= 8 && currentNode->column <= 11 || currentNode->row <= 8 && currentNode->column >= 12) {
				if (currentNode->column <= 11) { //check for if the current node is past half the map size
					randLoc.x = rand() % 14 + 8;
					randLoc.y = rand() % 20 + 11;
				}
				else {
					randLoc.x = rand() % 14 + 8;
					randLoc.y = rand() % 11 + 2;
				}

			}
			else { //could cause problem (add else if)
				if (currentNode->column <= map->getMapY() / 2) { //check for if the current node is past half the map size
					randLoc.x = rand() % 9 + 2;
					randLoc.y = rand() % 11 + 2;
				}
				else {
					randLoc.x = rand() % 9 + 2;
					randLoc.y = rand() % 11 + 2;
				}
			}
			if (map->mapArray[randLoc.x][randLoc.y]->Path == true) {
				map->aStar(path, *currentNode, *map->mapArray[randLoc.x][randLoc.y]);
				runFlag = true;
			}
		}
	} break;
	}
}

bool smartTank::isFiring()
{
	return firing;
}

void smartTank::score(int thisScore, int enemyScore)
{
}
