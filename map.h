#pragma once
#include <SFML/Graphics.hpp>
#include "node.h"
#include <list>
#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>

class Map
{
private:
	Node* node; // pointer to a node	
	int width = 35; //width of the Nodes
	int height = 35; //height of the nodes
	static const int unsigned Width = 780; //width of the map
	static const int unsigned Height = 570; //height of the map
	static const int unsigned rows = 16;
	static const int unsigned columns = 22;
	
public:
	Node* mapArray[16][22]; // pointer to an array which contains node pointers
	Map();
	~Map();
	static const int mapBuff = 10;
	void DrawMap(sf::RenderTarget &target); //Draws the SFML shapes
	// getters
	int getRows();
	int getColumns();
	int getMapX();
	int getMapY();
	int getNodeX();
	int getNodeY();
	// setters
	void removeBase(int x, int y); // remove discovered bases from path
	bool aStar(std::list<Node>& path, Node start, Node finish); // deals with path finding / returns true or false so we know if succesful
	std::list<Node> Map::createPath(std::list<Node> & path, std::list<Node>& closed, Node* node);
	std::vector<Node*> Map::getNeigh(Node* node);
};