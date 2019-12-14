#pragma once
#include "Map.h"
#include "Magician.h"
#include "Wizzard.h"
#include "Message.h"

class GameOfMazes : public olcConsoleGameEngine
{
	Map maps[1024];
	size_t mapCnt;
	size_t level;
	Player *player;
	pair<size_t, size_t> cursor;
	string state;
	string commandCycle; // command for after the infinite cycle
	float toWait;
	list<Message> messages;
	void loadMaps(Map *maps, size_t &mapCnt);
	void sortMaps(Map *maps, size_t &mapCnt);

	void printMessages(size_t x, size_t y, float fElapsedTime);

public:
	GameOfMazes() {}
	virtual bool OnUserCreate() override;
	virtual bool OnUserUpdate(float fElapsedTime) override;
	virtual bool OnUserDestroy() override;
};

bool GameOfMazes::OnUserCreate()
{
	mapCnt = 0;
	loadMaps(maps, mapCnt);
	level = 0;
	cursor = { 0,0 };
	state = "mapManagement";
	toWait = 0.0f;
	return true;
}

bool GameOfMazes::OnUserUpdate(float fElapsedTime)
{
	if (state == "exit") return false;
	if (state == "cycle")
	{
		for (size_t i = 0; i < 256; i++)
		{
			if (m_keys[i].bPressed)
			{
				state = commandCycle;
				m_keys[i].bPressed = false;
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}
		}
	}
	if (state == "play")
	{
		if (m_keys[VK_ESCAPE].bHeld) toWait = 0.0f; // scip via escape key
		toWait -= fElapsedTime;
		maps[level].print(this);
		if (player->getCntMoves()) Draw(player->getY(), player->getX(), L'P');
		if (toWait <= 0.0f)
		{
			if (player->getCntMoves()) pair<size_t, size_t> playerMove = player->move();
			toWait += 1.0f;
		}
		if (player->getX() == maps[level].getHeight() - 1 && player->getY() == maps[level].getWidth() - 1)
		{
			maps[level].print(this);
			Draw(maps[level].getWidth() - 1, maps[level].getHeight() - 1, L'P');
			maps[level].reset(); // not needed, but if to make some upgrades in the future, it will be necessary
			level++;
			state = "cycle";
			cursor = { 0,0 };
			delete player;

			if (level >= mapCnt)
			{
				DrawString(0, maps[level - 1].getHeight() + 1, L"You win!");
				commandCycle = "exit";
				return true;
			}
			commandCycle = "mapManagement";
			DrawString(0, maps[level - 1].getHeight() + 1, L"Level "s + to_wstring(level) + L" complete!"s);
		}
	}
	else if (state == "mapManagement")
	{
		maps[level].print(this);
		Draw(cursor.second, cursor.first, L'X', FG_YELLOW);
		if (m_keys[VK_LEFT].bPressed && cursor.second > 0) cursor.second--;
		if (m_keys[VK_UP].bPressed && cursor.first > 0) cursor.first--;
		if (m_keys[VK_RIGHT].bPressed && cursor.second < maps[level].getWidth() - 1) cursor.second++;
		if (m_keys[VK_DOWN].bPressed && cursor.first < maps[level].getHeight() - 1) cursor.first++;
		if (m_keys[VK_SPACE].bPressed)
		{
			if (maps[level][cursor.first][cursor.second] == '#')
			{
				messages.emplace_back(L"The cell is already a wall!"s, 2);
			}
			else if (cursor.first == 0 && cursor.second == 0)
			{
				messages.emplace_back(L"Cannot put walls on starting position!"s, 2);
			}
			else if (cursor.first == maps[level].getHeight() - 1 && cursor.second == maps[level].getWidth() - 1)
			{
				messages.emplace_back(L"Cannot put walls on ending position!"s, 2);
			}
			else if (maps[level][cursor.first][cursor.second] == '.')
			{
				if (maps[level].F)
				{
					maps[level][cursor.first][cursor.second] = 'X';
					maps[level].F--;
				}
				else messages.emplace_back(L"No more aviable wall slots!"s, 2);
			}
			else if (maps[level][cursor.first][cursor.second] == 'X')
			{
				maps[level][cursor.first][cursor.second] = '.';
				maps[level].F++;
			}
		}
		if (m_keys['m'].bPressed || m_keys['M'].bPressed)
		{
			toWait = 0.0f;
			try
			{
				player = new Magician(maps[level]);
			}
			catch (const exception &e)
			{
				messages.emplace_back(e.what(), 2);

				return true;
			}
			messages.clear();
			state = "play";
		}
		if (m_keys['w'].bPressed || m_keys['W'].bPressed)
		{
			toWait = 0.0f;
			try
			{
				player = new Wizzard(maps[level]);
			}
			catch (const exception &e)
			{
				messages.emplace_back(e.what(), 2);
				return true;
			}
			messages.clear();
			state = "play";
		}

	}
	printMessages(maps[level].getHeight()+1, 0, fElapsedTime);
	return true;
}

bool GameOfMazes::OnUserDestroy()
{
	delete player;
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

void GameOfMazes::printMessages(size_t x, size_t y, float fElapsedTime)
{
	for (auto it = messages.begin(); it != messages.end(); ++it)
	{
		it->durotation -= fElapsedTime;
		if (it->durotation <= 0.0f)
		{
			//Fill(it->y, it->x, it->y + it->text.size(), it->x, L' ');
			it = messages.erase(it);
			if (it == messages.begin() && it != messages.end())
			{
				DrawString(y, x++, it->text);
			}
			else if (it == messages.end())break;
			else --it;
		}
		else DrawString(y, x++, it->text);
	}
}