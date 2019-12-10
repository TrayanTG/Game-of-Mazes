#include <iostream>
#include "olcConsoleGameEngine.h"
using namespace std;

class Demo : public olcConsoleGameEngine
{
	float m_fPlayerX;
	float m_fPlayerY;
public:
	Demo()
	{

	}
protected:
	virtual bool OnUserCreate()
	{
		m_fPlayerX = 10.0f;
		m_fPlayerY = 10.0f;
		return true;
	}
	virtual bool OnUserUpdate(float fElapsedTime)
	{
		if (m_keys[VK_LEFT].bHeld)
			m_fPlayerX -= 15.0f * fElapsedTime;
		if (m_keys[VK_RIGHT].bHeld)
			m_fPlayerX += 15.0f * fElapsedTime;
		if (m_keys[VK_UP].bHeld)
			m_fPlayerY -= 15.0f * fElapsedTime;
		if (m_keys[VK_DOWN].bHeld)
			m_fPlayerY += 15.0f * fElapsedTime;


		Fill(0, 0, m_nScreenWidth, m_nScreenHeight, L' ', 0);
		Fill((int)m_fPlayerX, (int)m_fPlayerY, (int)m_fPlayerX + 5, (int)m_fPlayerY + 5, L'█', 15);
		return true;
	}
};

int main()
{
	Demo game;
	game.ConstructConsole(160, 80, 8, 8);
	game.Start();
	
	return 0;
}