#pragma once
#include "Player.h"

class Magician : public Player
{
	queue<pair<size_t, size_t> > moves; //holds the path of the player
	bool dfs(Map &m, size_t x = 0, size_t y = 0); //initializes moves and returns true if path found
public:
	Magician(const Map &m); //initializes and throws an exception if path not found
	virtual pair<size_t, size_t> move() override; //returns the next move of the player + repositioning
	virtual size_t getCntMoves()const override { return moves.size(); }
};

Magician::Magician(const Map &m)
{
	Map t(m);
	if(!dfs(t)) throw runtime_error("No path aviable!");
	//moves.pop(); // removing the initial value (0, 0)
}

bool Magician::dfs(Map &m, size_t x, size_t y)
{
	moves.emplace(x, y);
	if (x + 1 == m.getHeight() && y + 1 == m.getWidth()) return true;
	m[x][y] = 'T';
	if (x > 0 && m[x - 1][y] == '.' && dfs(m, x - 1, y)) return true;
	if (y > 0 && m[x][y - 1] == '.' && dfs(m, x, y - 1)) return true;
	if (x < m.getHeight() - 1 && m[x + 1][y] == '.' && dfs(m, x + 1, y)) return true;
	if (y < m.getWidth() - 1 && m[x][y + 1] == '.' && dfs(m, x, y + 1)) return true;
	return false;
}

pair<size_t, size_t> Magician::move()
{
	pair<size_t, size_t> t = std::move(moves.front());
	moves.pop();
	x = t.first;
	y = t.second;
	return t;
}
