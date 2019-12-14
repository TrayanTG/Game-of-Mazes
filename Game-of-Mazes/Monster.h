#pragma once
#include "Map.h"

class Monster
{
	size_t x, y;
	string direction;
public:
	Monster(size_t x, size_t y);
	pair<size_t, size_t> move(const Map &map, const vector<Monster> &m);

	size_t getX() const { return x; }
	size_t getY() const { return y; }
};

Monster::Monster(size_t x, size_t y) : x(x), y(y), direction("up") {}

pair<size_t, size_t> Monster::move(const Map &map, const vector<Monster> &m)
{
	if (direction == "up")
	{
		if (x == 0 || (map[x - 1][y] != '.' && map[x - 1][y] != 'X'))
		{
			direction = "left";
			return { x,y };
		}
		for (size_t i = 0;i < m.size();i++)
		{
			if (x - 1 == m[i].x && y == m[i].y)
			{
				direction = "left";
				return { x,y };
			}
		}
		return { --x,y };
	}
	else if (direction == "left")
	{
		if (y == 0 || (map[x][y - 1] != '.' && map[x][y - 1] != 'X'))
		{
			direction = "down";
			return { x,y };
		}
		for (size_t i = 0;i < m.size();i++)
		{
			if (x == m[i].x && y - 1 == m[i].y)
			{
				direction = "down";
				return { x,y };
			}
		}
		return { x,--y };
	}
	else if (direction == "down")
	{
		if (x >= map.getHeight() - 1 || (map[x + 1][y] != '.' && map[x + 1][y] != 'X'))
		{
			direction = "right";
			return { x,y };
		}
		for (size_t i = 0;i < m.size();i++)
		{
			if (x + 1 == m[i].x && y == m[i].y)
			{
				direction = "right";
				return { x,y };
			}
		}
		return { ++x,y };
	}
	else if (direction == "right")
	{
		if (y >= map.getWidth() - 1 || (map[x][y + 1] != '.' && map[x][y+1] != 'X'))
		{
			direction = "up";
			return { x,y };
		}
		for (size_t i = 0;i < m.size();i++)
		{
			if (x == m[i].x && y + 1 == m[i].y)
			{
				direction = "up";
				return { x,y };
			}
		}
		return { x,++y };
	}
	throw runtime_error("Wrong monster direction!");
	return { 0,0 };
}