#pragma once

#define NOMINMAX

#include <iostream>
#include <vector>
#include <fstream>
#include <exception>
#include <queue>
#include <utility>
#include <stack>
#include <limits>
#include <algorithm>
#include <random>

#include "olcConsoleGameEngine.h"
using namespace std;

const string DIR_MAPS = "maps";

class Map
{
	vector<vector<char> > map;
	vector<pair<size_t, size_t> > freeCells; //a vector, containing all cells of type '.'
	size_t monsters; //count of monsters
public:
	size_t F; // F = freeCells.size() - monsters - 2

	Map() {}
	Map(ifstream &iFile); //loads a map from a file and if failed to import, throws an exception
	
	bool isValid(); //checks if the map is valid (contains only '.' and '#' and a path from (0, 0) to (m-1, n-1))
	void print(olcConsoleGameEngine *cge) const; //prints the map to the console ptr the modified (with 'X'-es)

	void reset(); //undo all 'X' to '.'
	void shuffle(); //shuffle freeCells to later place monsters freeCells[0], [1], [2], ...

	size_t getHeight() const { return map.size(); }
	size_t getWidth() const { return map[0].size(); }
	size_t getMonstersCnt() const { return monsters; }
	pair<size_t, size_t> getNthFreeCell(size_t n) const { return freeCells[n]; }

	bool operator<(const Map &other) const; //predefined operator<. Returns true if *this is lower level than other
	vector<char>& operator[](size_t pos); //predefined operator[] for non-const
	const vector<char>& operator[](size_t pos) const; //predefined operator[] for const
};

Map::Map(ifstream &iFile)
{
	size_t m, n;
	iFile >> m >> n;
	if (iFile)
	{
		map.resize(m);
		for (size_t i = 0;i < m;i++)
		{
			map[i].resize(n);
			for (size_t j = 0;j < n;j++)
			{
				iFile >> map[i][j];
				if ((i || j) && (i!=getHeight() - 1 || j!=getWidth() - 1) && map[i][j] == '.')
				{
					freeCells.emplace_back(i, j);
				}
			}
		}
		iFile >> monsters;
		F = freeCells.size() - monsters;
	}
	else
	{
		throw runtime_error("No more maps to be loaded!");
	}
}

bool Map::isValid()
{
	size_t m = map.size(), n = map[0].size();
	if (map[0][0] == '#' || map[m-1][n-1] == '#') return false;
	for (size_t i = 0;i < m;i++) for (size_t j = 0;j < n;j++) if (map[i][j] != '.' && map[i][j] != '#') return false;
	queue<pair<size_t, size_t> > q, fix;
	q.emplace(0, 0);
	map[0][0] = '#'; fix.emplace(0, 0);
	while (!q.empty())
	{
		pair<size_t, size_t> p = q.front(); q.pop();
		if (p.first < m - 1 && map[p.first + 1][p.second] == '.')
		{
			if (p.first + 1 == m - 1 && p.second == n - 1)
			{
				while (!fix.empty())
				{
					p = fix.front(); fix.pop();
					map[p.first][p.second] = '.';
				}
				return true;
			}
			q.emplace(p.first + 1, p.second);
			map[p.first + 1][p.second] = '#';
			fix.emplace(p.first + 1, p.second);
		}
		if (p.second < n - 1 && map[p.first][p.second + 1] == '.')
		{
			if (p.first == m - 1 && p.second + 1 == n - 1)
			{
				while (!fix.empty())
				{
					p = fix.front(); fix.pop();
					map[p.first][p.second] = '.';
				}
				return true;
			}
			q.emplace(p.first, p.second + 1);
			map[p.first][p.second + 1] = '#';
			fix.emplace(p.first, p.second + 1);
		}
		if (p.first > 0 && map[p.first - 1][p.second] == '.')
		{
			q.emplace(p.first - 1, p.second);
			map[p.first - 1][p.second] = '#';
			fix.emplace(p.first - 1, p.second);
		}
		if (p.second > 0 && map[p.first][p.second - 1] == '.')
		{
			q.emplace(p.first, p.second - 1);
			map[p.first][p.second - 1] = '#';
			fix.emplace(p.first, p.second - 1);
		}
	}
	while (!fix.empty())
	{
		pair<size_t, size_t> p = fix.front(); fix.pop();
		map[p.first][p.second] = '.';
	}
	return false;
}

void Map::print(olcConsoleGameEngine *cge)const
{
	cge->Fill(0, 0, cge->ScreenWidth(), cge->ScreenHeight(), L' ', 0);
	for (size_t i = 0; i < map.size();i++)
		for (size_t j = 0; j < map[i].size();j++)
		{
			if (map[i][j] == '.')cge->Draw(j, i, map[i][j], FG_YELLOW);
			else if (map[i][j] == 'X')cge->Draw(j, i, map[i][j], FG_CYAN);
			else if (map[i][j] == '#')cge->Draw(j, i, map[i][j], FG_DARK_CYAN);

		}
}

void Map::reset()
{
	for (size_t i = 0; i < map.size();i++)
		for (size_t j = 0; j < map[i].size();j++)
			if (map[i][j] != '#')
				map[i][j] = '.';
	F = freeCells.size() - monsters;
}

void Map::shuffle()
{
	unsigned seed = chrono::system_clock::now().time_since_epoch().count();
	default_random_engine e(seed);
	std::shuffle(freeCells.begin(), freeCells.end(), e);
}

bool Map::operator<(const Map &other)const
{
	size_t s1 = map.size()*map[0].size();
	size_t s2 = other.map.size()*other.map[0].size();
	if (s1 < s2) return true;
	if (s1 == s2 && monsters < other.monsters) return true;
	return false;
}

vector<char>& Map::operator[](size_t pos)
{
	return map[pos];
}

const vector<char>& Map::operator[](size_t pos) const
{
	return map[pos];
}
