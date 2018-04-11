#include "map.h"

Map::Map()
{
	// create a new node object in each space of the array
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++) {
			node = new Node(15 + width * j, 10 + height * i, width, height, i, j);
			mapArray[i][j] = node;
		}
	}
}

Map::~Map()
{

}

void Map::DrawMap(sf::RenderTarget & target)
{
	// used for drawing nodes to screen // used for debug and testing
	for (int i = 0; i < 22; i++) {
		for (int j = 0; j < 16; j++) {
			mapArray[j][i]->DrawNode(target);
		}
	}
}

int Map::getRows()
{
	return rows;
}

int Map::getColumns()
{
	return columns;
}

int Map::getMapX()
{
	return Width;
}

int Map::getMapY()
{
	return Height;
}

int Map::getNodeX()
{
	return width;
}

int Map::getNodeY()
{
	return height;
}

void Map::removeBase(int x, int y)
{
	// checks what inputted base is in what node and if so remove them from being a path
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++) {
			// loop through nodes
			if (x < (mapArray[i][j]->getX() - width / 2) + width)
				if (x + 50 > mapArray[i][j]->getX() - width / 2)
					if (y < (mapArray[i][j]->getY() - height / 2) + height)
						if (y + 50 > mapArray[i][j]->getY() - height / 2)
							mapArray[i][j]->setWall(); 
			// set node as wall / method changes path bool to true
		}
	}
}

bool Map::aStar(std::list<Node>& path, Node start, Node finish) // don't return path just change referenced one
{
	// based off example code as i can't get mine working solo 
	// Throw away vars
	Node currentNode = start;
	Node finalNode = finish;
	// storage
	std::list<Node> open;
	std::list<Node> closed;
	closed.clear(); // should already be empty
	// clear node vars incase conflict
	currentNode.parent = -1;
	currentNode.g = 0; // clear g score
	// get score of first node
		// f = g + h
	currentNode.f = currentNode.g +
		currentNode.scoreCalc(currentNode.g, currentNode.getX(), currentNode.getY(),
			finish.getX(), finish.getY());
	// now score has been calc add to list
	open.push_back(currentNode);
	// check if nodes are in open list // if empty break method
	while (!open.empty()) {
		// stable sort -> orders list in smallest firsat
			// mem_fun_ref -> takes a pointer to a member function which should put each node through the method
		std::stable_sort(open.begin(), open.end(), std::mem_fun_ref(&Node::lowerOutcome)); // will be using this again very nice way of sorting
		// now we check if current node has reached target node
			// if so then we return path
		if (currentNode.row == finish.row && currentNode.column == finish.column)
			path = createPath(path, closed, &finish); return true;
		// get best scoring node from the open list
		currentNode = open.front();
		open.pop_front(); // take off open list
		closed.push_back(currentNode); // added to closed list
		mapArray[currentNode.row][currentNode.column]->setGoal(); // for debug reasons
	}
	return false;
}

std::list<Node> Map::createPath(std::list<Node>& path, std::list<Node>& closed, Node * node)
{
	Node* currentNode = node; // set goal node as current from construc
	path.push_front(*currentNode); // give path currentnnode as starting point
	// remove node from closed list
	closed.pop_back();
	// work back through the closed list to create path
	std::list<Node>::iterator iterator; // because we are using lists / need iterator 
	for (iterator = closed.end(), iterator--; iterator != closed.begin(); iterator--) {
		// keep looping untill nothing left in list
		path.push_front(*iterator); // add it to front as it's backwards
		closed.erase(iterator); // remove node
		iterator = closed.end(); // work back through list again from the end as it's backwards
	}

	return path;
}