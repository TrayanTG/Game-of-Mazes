#pragma once
#include "Map.h"
#include "Magician.h"
#include "Wizzard.h"

class GameOfMazes : public olcConsoleGameEngine
{
	Map maps[1024];
	size_t mapCnt;
	Player *player;
	Player *player2; // only for tests
	float toSecond;
	void loadMaps(Map *maps, size_t &mapCnt);
	void sortMaps(Map *maps, size_t &mapCnt);

public:
	GameOfMazes() {}
	virtual bool OnUserCreate() override;
	virtual bool OnUserUpdate(float fElapsedTime) override;
	virtual bool OnUserDestroy() override;
};

bool GameOfMazes::OnUserCreate()
{
	mapCnt = 0;
	toSecond = 1.0f;
	loadMaps(maps, mapCnt);
	player = new Magician(maps[mapCnt - 1]);
	player2 = new Wizzard(maps[mapCnt - 1]);
	return true;
}

bool GameOfMazes::OnUserUpdate(float fElapsedTime)
{
	toSecond -= fElapsedTime;
	if (toSecond <= 0.0f)
	{
		maps[mapCnt - 1].print(this);
		if (player->getCntMoves())
		{
			pair<size_t, size_t> playerMove = player->move();
			Draw(player->getY(), player->getX(), L'M');
		}
		if (player2->getCntMoves())
		{
			pair<size_t, size_t> playerMove = player2->move();
			Draw(player2->getY(), player2->getX(), L'W');
		}

		toSecond += 1.0f;
	}
	return true;
}

bool GameOfMazes::OnUserDestroy()
{
	delete player;
	delete player2;
	return true;
}

void GameOfMazes::loadMaps(Map *maps, size_t &mapCnt)
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
	catch (...)//(const exception &e)
	{
		//cout << e.what() << endl;
	}
	sortMaps(maps, mapCnt);
}

void GameOfMazes::sortMaps(Map *maps, size_t &mapCnt)
{
	sort(maps, maps + mapCnt);
}