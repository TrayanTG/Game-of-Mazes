#include <iostream>
#include <string>
#include "GameOfMazes.h"
using namespace std;

int main()
{
	GameOfMazes game;
	game.ConstructConsole(80, 40, 16, 16);
	game.Start();
	/*maps[mapCnt - 1].print();
	Player *player = new Magician(maps[mapCnt - 1]);
	while (player->getCntMoves())
	{
		maps[mapCnt - 1].print();
		auto m = player->move();
		maps[mapCnt - 1][m.first][m.second] = 'P';
		system("pause");
	}
	system("pause");*/
	return 0;
}
