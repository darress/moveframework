#pragma once
#include "StdAfx.h"

class UIManager
{
	MyGUI::Gui* mGUI;
	float infoTimer;

public:
	UIManager(MyGUI::Gui* mGUI);
	virtual ~UIManager();

	void update(float deltaT);
	void showInfo(const MyGUI::UString& value, float time);
	void updateScore(int score1, int score2);
	void showGameType(const MyGUI::UString& value);
};

