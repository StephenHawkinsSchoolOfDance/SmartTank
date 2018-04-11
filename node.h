#pragma once
#include <SFML/Graphics.hpp>
#include "defs.h"

class Node {
private:
	// standard variables 
	int xPos;
	int yPos;
	int height;
	int width;
	sf::RectangleShape sfNode; // this will be the physical node // drawn in debug mode
public:
	int row;
	int column;
	// node info
	bool notPath = false;
	bool Path = true;
	bool current = true; // will be changed by setCurrent method, depends on what's passed
	Node(int xPosIn, int yPosIn, int heightIn, int widthIn, int rowIn, int columnIn);
	void DrawNode(sf::RenderTarget &target); // draw node to screen, depneding if debug bool is true
	float f; //total cost of the node
	float g; //distance of current to start
	float h; //distance of current to goal 
	int parent;
	// Setters
	void setCurrent(bool c); // set node object to current depending on bool
	// node colour setter for debug aid // will change to one method if time
	void setWall();
	void setPath();
	void setGoal();
	void setStart();
	void setOpen();
	void setClosed();
	// Getters
	int getX();
	int getY();
	// lower outcome score to be added
	// A* Euclidean distance calc
	float scoreCalc(float prevG, int xPos, int yPos, int goalX, int goalY);
	int lowerOutcome(const Node other) const; // used for sorting smallest f value between nodes
};	