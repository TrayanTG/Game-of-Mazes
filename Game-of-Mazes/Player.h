#pragma once
#include "Map.h"

class Player
{
protected:
	size_t x;
	size_t y;
public:
	Player() : x(0), y(0) {}
	virtual ~Player() {} //needed so that x and y destructors are called
	virtual pair<size_t, size_t> move() = 0; //virtual func, which when inherited will return the next move of the player

	size_t getX()const { return x; }
	size_t getY()const { return y; }
	virtual size_t getCntMoves()const = 0;
};