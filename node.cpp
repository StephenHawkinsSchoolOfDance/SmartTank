#include "node.h"

Node::Node(int xPosIn, int yPosIn, int heightIn, int widthIn, int rowIn, int columnIn)
{
	// receive pos of node from construc
	xPos = xPosIn;
	yPos = yPosIn;
	row = rowIn;
	column = columnIn;
	height = heightIn;
	width = widthIn;
	// set sfml shape to correct pos
	sfNode.setSize(sf::Vector2f(widthIn, heightIn));
	sfNode.setPosition(sf::Vector2f(xPosIn, yPosIn)); // possible loss of data but too lazy
	sfNode.setFillColor(sf::Color(255, 255, 255, 50)); // grey
}

void Node::DrawNode(sf::RenderTarget & target)
{
	target.draw(sfNode);
}

void Node::setCurrent(bool c)
{
	// sets current node colour
	if (!current && c) {
		sfNode.setFillColor(sf::Color(255, 0, 255, 150));
		current = true;
	}
	else if (current && !c) {
		sfNode.setFillColor(sf::Color(255, 255, 255, 50));
		current = false;
	}
}

void Node::setWall()
{
	Path = false;
	sfNode.setFillColor(sf::Color(0, 0, 0, 100));
}

void Node::setPath()
{
	Path = true;
	sfNode.setFillColor(sf::Color(255, 255, 255, 50));
}

void Node::setGoal()
{
	sfNode.setFillColor(sf::Color(255, 0, 0, 100));
}

void Node::setStart()
{
	sfNode.setFillColor(sf::Color(0, 255, 0, 100));
}

void Node::setOpen()
{
	sfNode.setFillColor(sf::Color(255, 255, 255, 100));
}

void Node::setClosed()
{
	sfNode.setFillColor(sf::Color(255, 255, 0, 100));
}

int Node::getX()
{
	return xPos + width / 2;
}

int Node::getY()
{
	return yPos + height / 2;
}

float Node::scoreCalc(float prevG, int x, int y, int goalX, int goalY)
{
	sf::Vector2f dVec ((float)(goalX - x), (float)(goalY - y)); // vars from copnstructor are ints // just cast
	// update vars in header here as lazy
	h = sqrt((dVec.x * dVec.x) + (dVec.y * dVec.y)); // h score is distance
	g = prevG + 1.f;
	return h;
}

int Node::lowerOutcome(const Node other) const
{
	return f < other.f;
}
