#pragma once
#include "Player.h"

class Wizzard : public Player
{
	stack<pair<size_t, size_t> > moves;
	bool bfs(const Map &m, size_t x = 0, size_t y = 0);
public:
	Wizzard(const Map &m);
	virtual pair<size_t, size_t> move() override;
	virtual size_t getCntMoves()const override { return moves.size(); }
};

Wizzard::Wizzard(const Map &m)
{
	if(!bfs(m)) throw runtime_error("No path aviable!");
	//moves.pop(); // removing the initial value (0, 0)
}

bool Wizzard::bfs(const Map &m, size_t x, size_t y)
{
	// if the maze is 1x1, which is theoretically impossible because of F = |free spaces| - |monsters| - 2 = -1 - |monsters|, which is <0 for any non-negative monster amount
	if (x + 1 == m.getHeight() && y + 1 == m.getWidth())
	{
		moves.emplace(x, y);
		return true;
	}

	vector<vector<pair<size_t, size_t> > > parent;
	parent.resize(m.getHeight());
	for (size_t i = 0;i < m.getHeight();i++)
	{
		parent[i].resize(m.getWidth(), { numeric_limits<size_t>::max(),numeric_limits<size_t>::max() });
	}

	queue<pair<size_t, size_t> >q; // que for the dfs
	q.emplace(x, y);
	parent[0][0] = { 0,0 }; // (0, 0) comes from itself

	while (!q.empty())
	{
		x = q.front().first;
		y = q.front().second;
		q.pop();
		if (x == m.getHeight() && y == m.getWidth())break; // a little slower, but cleaner
		if (x > 0 && m[x - 1][y] == '.' && parent[x - 1][y].first == numeric_limits<size_t>::max() && parent[x - 1][y].second == numeric_limits<size_t>::max())
		{
			parent[x - 1][y] = make_pair(x, y);
			q.emplace(x - 1, y);
		}
		if (y > 0 && m[x][y - 1] == '.' && parent[x][y - 1].first == numeric_limits<size_t>::max()  && parent[x][y - 1].second == numeric_limits<size_t>::max())
		{
			parent[x][y - 1] = make_pair(x, y);
			q.emplace(x, y - 1);
		}
		if (x < m.getHeight() - 1 && m[x + 1][y] == '.' && parent[x + 1][y].first == numeric_limits<size_t>::max() && parent[x + 1][y].second == numeric_limits<size_t>::max())
		{
			parent[x + 1][y] = make_pair(x, y);
			q.emplace(x + 1, y);
		}
		if (y < m.getWidth() - 1 && m[x][y + 1] == '.' && parent[x][y + 1].first == numeric_limits<size_t>::max() && parent[x][y + 1].second == numeric_limits<size_t>::max())
		{
			parent[x][y + 1] = make_pair(x, y);
			q.emplace(x, y + 1);
		}
	}

	// if no path aviable return false
	if (parent[m.getHeight() - 1][m.getWidth() - 1] == make_pair(numeric_limits<size_t>::max(), numeric_limits<size_t>::max())) return false;

	// otherwise, trace the path which led us to (m-1, n-1)
	pair<size_t, size_t> p = { m.getHeight() - 1, m.getWidth() - 1 };
	while (p.first || p.second)
	{
		moves.push(p);
		p = parent[p.first][p.second];
	}
	moves.emplace(0, 0);
	return true;
}

pair<size_t, size_t> Wizzard::move()
{
	pair<size_t, size_t> t = std::move(moves.top());
	moves.pop();
	x = t.first;
	y = t.second;
	return t;
}
