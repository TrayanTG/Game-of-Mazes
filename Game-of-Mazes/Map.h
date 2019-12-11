#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include <exception>
#include <queue>
#include <utility>
using namespace std;

const string DIR_MAPS = "maps";

class Map;

void loadMaps(Map *maps, size_t &mapCnt);
void sortMaps(Map *maps, size_t &mapCnt);

class Map
{
	vector<vector<char> > map;
	size_t monsters;
public:
	Map() {}
	Map(ifstream &iFile);
	
	bool isValid();
	void print()const;

	bool operator<(const Map &other)const;
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
			}
		}
		iFile >> monsters;
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

void Map::print()const
{
	for (size_t i = 0; i < map.size();i++)
	{
		for (size_t j = 0; j < map[i].size();j++)
		{
			cout << map[i][j];
		}
		cout << endl;
	}
	cout << endl;
}

bool Map::operator<(const Map &other)const
{
	size_t s1 = map.size()*map[0].size();
	size_t s2 = other.map.size()*other.map[0].size();
	if (s1 < s2) return true;
	if (s1 == s2 && monsters < other.monsters) return true;
	return false;
}

// --------------------------------------------------------

void loadMaps(Map *maps, size_t &mapCnt)
{
	ifstream iFile(DIR_MAPS);
	if (!iFile)
	{
		cerr << "File couldn't be opened!\n";
		system("pause");
		return;
	}
	try {
		while (true)
		{
			Map t(iFile);
			if (t.isValid()) maps[mapCnt++] = move(t);
		}
	}
	catch (const exception &e)
	{
		//cout << e.what() << endl;
	}
	sortMaps(maps, mapCnt);
}

void sortMaps(Map *maps, size_t &mapCnt)
{
	sort(maps, maps + mapCnt);
}
