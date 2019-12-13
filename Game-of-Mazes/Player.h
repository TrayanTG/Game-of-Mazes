#pragma once
#include "Map.h"

class Player
{
protected:
	queue<pair<size_t, size_t> > moves;
	size_t x, y;
public:
	Player() : x(0), y(0) {}
	virtual ~Player() {}
	virtual pair<size_t, size_t> move() = 0;

	size_t getX()const { return x; }
	size_t getY()const { return y; }
	virtual size_t getCntMoves()const = 0;
};