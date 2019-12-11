#include <iostream>
#include <string>
#include "Map.h"
using namespace std;

Map maps[1024];
size_t mapCnt = 0;

int main()
{
	loadMaps(maps, mapCnt);
	for (size_t i = 0;i < mapCnt;i++)
	{
		maps[i].print();
	}
	system("pause");
	return 0;
}
