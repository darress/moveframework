#include "StdAfx.h"
#include "UIManager.h"


UIManager::UIManager(MyGUI::Gui* mGUI)
	:mGUI(mGUI)
{
}

UIManager::~UIManager()
{
}

void UIManager::update(float deltaT)
{
	if (infoTimer > 0.0f)
	{
		infoTimer -= deltaT;
		if (infoTimer <= 0.0f)
		{
			infoTimer = 0.0f;
			mGUI->findWidget<MyGUI::TextBox>("Info")->setCaption("");
		}
	}
}

void UIManager::showInfo(const MyGUI::UString& value, float time)
{
	mGUI->findWidget<MyGUI::TextBox>("Info")->setCaption(value);
	infoTimer = time;
}

void UIManager::updateScore(int score1, int score2)
{
	char tmp[128];
	sprintf_s(tmp, sizeof(tmp), "Score: %d : %d", score1, score2);
	mGUI->findWidget<MyGUI::TextBox>("Score")->setCaption(tmp);
}

void UIManager::showGameType(const MyGUI::UString& value)
{
	mGUI->findWidget<MyGUI::TextBox>("Game Type")->setCaption(value);
}