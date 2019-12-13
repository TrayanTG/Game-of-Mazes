#include <iostream>
#include <string>
#include "GameOfMazes.h"
using namespace std;

int main()
{
	GameOfMazes game;
	game.ConstructConsole(80, 40, 16, 16);
	game.Start();
	return 0;
}
