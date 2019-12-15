#pragma once
#include "Map.h"
#include "Magician.h"
#include "Wizzard.h"
#include "Message.h"
#include "Monster.h"

class GameOfMazes : public olcConsoleGameEngine
{
	Map maps[1024]; //holds the data of all maps
	size_t mapsCnt; //holds the count of the maps
	size_t level; //the current level of the player
	Player *player; //ptr to the player class (Magician or Wizard)
	pair<size_t, size_t> cursor; //cursor for putting walls
	string state; //what the main game thread loop should do
	string commandCycle; //command for after the infinite cycle
	float toWait; //variable, used to track time
	list<Message> messages; //interactive list of messages
	vector<Monster> monsters; //data for all the monsters of the current map

	void loadMaps(); //loads maps from a file DIR_MAPS
	void sortMaps(); //sorts them by level

	void initMonsters(size_t level); //sets initial random positions of the monsters
	void updateMonsters(); //all monsters move if possible
	void printMonsters(); //prints all monsters to the console
	bool checkLose() const; //returns true if the player and monster overlap

	void cycle(); //"pauses" the main game loop untill a button is pressed
	bool play(float fElapsedTime); //the process of "walking" through the maze
	void mapManagement(); //the process of putting walls
	void printMessages(size_t x, size_t y, float fElapsedTime); //prints the messages and if needed, deletes them


public:
	GameOfMazes() {}
	virtual bool OnUserCreate() override; //initializes the class
	virtual bool OnUserUpdate(float fElapsedTime) override; //main game thread loop
	virtual bool OnUserDestroy() override;
};

bool GameOfMazes::OnUserCreate()
{
	loadMaps();
	level = 0;
	cursor = { 0,0 };
	state = "mapManagement";
	toWait = 0.0f;

	return true;
}

bool GameOfMazes::OnUserUpdate(float fElapsedTime)
{
	if (state == "exit") return false;
	else if (state == "cycle") cycle();
	else if (state == "play") return play(fElapsedTime);
	else if (state == "mapManagement") mapManagement();
	printMessages(maps[level].getHeight() + 1, 0, fElapsedTime);
	return true;
}

bool GameOfMazes::OnUserDestroy()
{
	delete player;
	return true;
}

void GameOfMazes::loadMaps()
{
	mapsCnt = 0;
	ifstream iFile(DIR_MAPS);
	if (!iFile)
	{
		cerr << "File couldn't be opened!\n";
		system("pause");
		return;
	}
	// read utill eof (Map(ifstream&) throws exception when eof reached)
	try {
		while (true)
		{
			Map t(iFile);
			if (t.isValid()) maps[mapsCnt++] = move(t);
		}
	}
	catch (...)//(const exception &e)
	{
		//cout << e.what() << endl;
	}
	sortMaps();
}

void GameOfMazes::sortMaps()
{
	sort(maps, maps+mapsCnt);
}

void GameOfMazes::initMonsters(size_t level)
{
	maps[level].shuffle();
	monsters.clear();
	size_t j = 0;
	for (size_t i = 0;i < maps[level].getMonstersCnt();i++)
	{
		while (maps[level][maps[level].getNthFreeCell(j).first][maps[level].getNthFreeCell(j).second] != '.')j++;
		monsters.emplace_back(maps[level].getNthFreeCell(j).first, maps[level].getNthFreeCell(j).second);
		j++;;
	}
}

void GameOfMazes::updateMonsters()
{
	for (size_t i = 0;i < maps[level].getMonstersCnt(); i++)
	{
		monsters[i].move(maps[level], monsters);
	}
}

void GameOfMazes::printMonsters()
{
	for (size_t i = 0;i < maps[level].getMonstersCnt();i++)
	{
		Draw(monsters[i].getY(), monsters[i].getX(), 'M', FG_RED);
	}
}

bool GameOfMazes::checkLose() const
{
	for (size_t i = 0;i < maps[level].getMonstersCnt();i++)
	{
		if (monsters[i].getX() == player->getX() && monsters[i].getY() == player->getY()) return true;
	}
	return false;
}

void GameOfMazes::cycle()
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

bool GameOfMazes::play(float fElapsedTime)
{
	if (m_keys[VK_ESCAPE].bHeld) toWait = 0.0f; // skip via escape key
	toWait -= fElapsedTime;

	maps[level].print(this);
	printMonsters();
	Draw(player->getY(), player->getX(), L'P', FG_GREEN);
	
	if (checkLose())
	{
		Draw(player->getY(), player->getX(), L'P', FG_DARK_RED);
		maps[level].reset();
		state = "cycle";
		cursor = { 0,0 };
		delete player;
		DrawString(0, maps[level].getHeight() + 1, L"You lose!");
		commandCycle = "mapManagement";
		level = 0;
		return true;
	}

	if (toWait <= 0.0f)
	{
		if (player->getCntMoves()) pair<size_t, size_t> playerMove = player->move();
		updateMonsters();

		if (player->getX() == maps[level].getHeight() - 1 && player->getY() == maps[level].getWidth() - 1)
		{
			maps[level].print(this);
			printMonsters();
			Draw(maps[level].getWidth() - 1, maps[level].getHeight() - 1, L'P', FG_GREEN);
			maps[level].reset(); // if player loses in the future, we need the map reseted and ready to go
			state = "cycle";
			cursor = { 0,0 };
			delete player;
			level++;

			if (level >= mapsCnt)
			{
				DrawString(0, maps[level - 1].getHeight() + 1, L"You win!");
				commandCycle = "exit";
				return true;
			}
			commandCycle = "mapManagement";
			DrawString(0, maps[level - 1].getHeight() + 1, L"Level "s + to_wstring(level) + L" complete!"s);
		}
		toWait += 1.0f;
	}
	return true;
}

void GameOfMazes::mapManagement()
{
	maps[level].print(this);
	Draw(cursor.second, cursor.first, L'X', FG_CYAN);
	if (m_keys[VK_LEFT].bPressed && cursor.second > 0) cursor.second--;
	if (m_keys[VK_UP].bPressed && cursor.first > 0) cursor.first--;
	if (m_keys[VK_RIGHT].bPressed && cursor.second < maps[level].getWidth() - 1) cursor.second++;
	if (m_keys[VK_DOWN].bPressed && cursor.first < maps[level].getHeight() - 1) cursor.first++;
	if (m_keys[VK_SPACE].bPressed)
	{
		if (maps[level][cursor.first][cursor.second] == '#')
		{
			messages.emplace_back(L"The cell is already a wall!"s, 2);
			return;
		}
		else if (cursor.first == 0 && cursor.second == 0)
		{
			messages.emplace_back(L"Cannot put walls on starting position!"s, 2);
			return;
		}
		else if (cursor.first == maps[level].getHeight() - 1 && cursor.second == maps[level].getWidth() - 1)
		{
			messages.emplace_back(L"Cannot put walls on ending position!"s, 2);
			return;
		}
		else if (maps[level][cursor.first][cursor.second] == '.')
		{
			if (maps[level].F)
			{
				maps[level][cursor.first][cursor.second] = 'X';
				maps[level].F--;
			}
			else
			{
				messages.emplace_back(L"No more aviable wall slots!"s, 2);
				return;
			}
		}
		else if (maps[level][cursor.first][cursor.second] == 'X')
		{
			maps[level][cursor.first][cursor.second] = '.';
			maps[level].F++;
		}
	}
	if (m_keys['m'].bPressed || m_keys['M'].bPressed) // Magician
	{
		toWait = 1.0f;
		try
		{
			player = new Magician(maps[level]);
			player->move();
		}
		catch (const exception &e)
		{
			messages.emplace_back(e.what(), 2);

			return;
		}
		messages.clear();
		initMonsters(level);
		state = "play";
	}
	if (m_keys['w'].bPressed || m_keys['W'].bPressed) // Wizard
	{
		toWait = 1.0f;
		try
		{
			player = new Wizzard(maps[level]);
			player->move();
		}
		catch (const exception &e)
		{
			messages.emplace_back(e.what(), 2);
			return;
		}
		messages.clear();
		initMonsters(level);
		state = "play";
	}
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
