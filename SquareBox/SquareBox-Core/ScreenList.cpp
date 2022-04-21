#include "ScreenList.h"
#include "IGameScreen.h"
#include "IMainGame.h"

namespace SquareBox {
	ScreenList::ScreenList(IMainGame * game)
		:m_game(game) {
		//Empty
	}

	IGameScreen* SquareBox::ScreenList::moveNext()
	{
		IGameScreen* currentScreen = getCurrent();
		if (currentScreen->getNextScreenIndex() != SCREEN_INDEX_NO_SCREEN)
		{
			m_currentScreenIndex = currentScreen->getNextScreenIndex();
		}
		return getCurrent(); //return the new current screen
	}

	IGameScreen * ScreenList::movePrevious()
	{
		IGameScreen* currentScreen = getCurrent();
		if (currentScreen->getPreviousScreenIndex() != SCREEN_INDEX_NO_SCREEN)
		{
			m_currentScreenIndex = currentScreen->getPreviousScreenIndex();
		}
		return getCurrent(); //return the new current screen
	}

	void ScreenList::setScreen(unsigned int nextScreen)
	{
		if (nextScreen >= 0 && nextScreen < m_screens.size()) {
			m_currentScreenIndex = nextScreen;
		}
		else {
			SBX_CORE_ERROR("Invalid Screen index -> {} ", nextScreen);
		}
	}

	void ScreenList::addScreen(IGameScreen* newScreen)
	{
		newScreen->m_screen_index = m_screens.size();
		m_screens.push_back(newScreen);
		newScreen->build();
		newScreen->setParentGame(m_game);
	}

	void ScreenList::destroy()
	{
		for (unsigned i = 0; i < m_screens.size(); i++)
		{
			m_screens[i]->destroy();
		}

		m_screens.resize(0);
		m_currentScreenIndex = SCREEN_INDEX_NO_SCREEN;
	}

	ScreenList::~ScreenList()
	{
		destroy();
	}

	IGameScreen * ScreenList::getCurrent()
	{
		if (m_currentScreenIndex == SCREEN_INDEX_NO_SCREEN)
		{
			return nullptr;
		}
		return m_screens[m_currentScreenIndex];
	}
}