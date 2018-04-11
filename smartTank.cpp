#include "smartTank.h"

smartTank::smartTank()
{
	map = new Map();
	clearMovement();
}

smartTank::~smartTank()
{
	// clean up please!!
	map = nullptr;
	delete map;
	currentNode = nullptr; 
	delete currentNode;
	targetNode = nullptr;
	delete targetNode;
}

void smartTank::move()
{
	tankRect.setSize(sf::Vector2f(200, 10)); // set size of tank
	//if (friendlyBaseSpotted == true)tankRect.setFillColor(sf::Color(255, 0, 0, 50));
	tankRect.setRotation(turretTh); // set tank rotation to turret direction
	// updating here / states machines handled here 
	setCurrentNode(); // finds wherte the player tank is
	navigationHandler(); // find next move
	//movementHandler();
}

void smartTank::reset()
{
}

void smartTank::collided()
{
}

void smartTank::markTarget(Position p)
{
	//std::cout << "Target spotted at (" << p.getX() << ", " << p.getY() << ")\n";
	
}

void smartTank::markEnemy(Position p)
{
	// because enemy tank is dynamic i will do some calcs here as they will be constanly updated
	std::cout << "Target spotted at (" << p.getX() << ", " << p.getY() << ")\n"; // debug / testing
	enemySpotted = true;
	// find tank point and enemey points
	sf::Vector2i delta(getX() - p.getX(), getY() - p.getY());
	int angle = atan2(delta.y, delta.x) * 180 / PI; // find angle between the two point
	// set angle to variable as it's used as dynmaic 
	turretAngle = angle + 180; // add 180 else turret is backwards
	// next work out distance to offset aiming or states
	sf::Vector2f dist(p.getX() - getX(), p.getY() - getY());
	// now we have dynmic updated range finder, used in aiming or state machines 
	enemyDistance = sqrt((dist.x * dist.x) + (dist.y * dist.y)); 
}

void smartTank::markBase(Position p)
{
}

void smartTank::markShell(Position p)
{
}

void smartTank::setCurrentNode()
{
	// find player pos
	int x = (getX() - (map->getNodeX() / 2)) / map->getNodeX();
	int y = (getY() - (map->getNodeY() / 2)) / map->getNodeY();
	// clean up / set every node current to false
	for (int i = 0; i < map->getRows(); i++)
		for (int j = 0; j < map->getColumns(); j++)
			map->mapArray[i][j]->setCurrent(false);
	// set node player is in to current
	map->mapArray[y][x]->setCurrent(true);
	currentNode = map->mapArray[y][x]; // add reference to our new current node
}

void smartTank::movementHandler()
{
}

void smartTank::navigationHandler()
{
	// first call of method is a map discovery method // will add second search here
	while (!runFlag) {
		// pick random location to navigate to
		int randXLoc = rand() % 13 + 2;
		int randYLoc = rand() % 5 + 2;
		// do search // use A*
		runFlag = true; // turn off flag / will be in try catch to ensure tank moves first time
	}
	// setting up states // will change syntax
	const int IDLE = 0;
	const int MOVE_RAND = 1;
	const int MOVE_AWAY = 2;
	const int MOVE_CENTRE = 3;
	int state = IDLE;
	// limits
	int enemyRangeLimit = 100;
	int wallRangeLimit = 10;
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
			state = MOVE_CENTRE;
			//std::cout << "State Changed -> MOVE_CENTRE \n";
			runFlag = false;
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
				runFlag = false; // turn off if we create a path else loop again
				// create path here with A*
			}			
		} break;
		// mainly for when tank collides with walls or tank gets lost
		case MOVE_CENTRE: {
			sf::Vector2i center(map->getRows() / 2, (map->getColumns() / 2) - 1); // take 1 for sfml shapes
			while (!runFlag) {
				// do A* create a path
				runFlag = false;
				// added so if middle is occup
				center.x++;
				center.y++;
			}			 
		} break;
		case MOVE_RAND: {

		} break;
	}
}

bool smartTank::isFiring()
{
	return false;
}

void smartTank::score(int thisScore, int enemyScore)
{
}
