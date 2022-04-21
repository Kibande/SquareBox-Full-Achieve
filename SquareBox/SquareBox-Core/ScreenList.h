#pragma once
#include<vector>
#include <SquareBox-Logger/Logger.h>
namespace SquareBox {
	class IMainGame;
	class IGameScreen;
	class ScreenList {
		//this class has list of screens
		//and also helps us to move between the different screens
	public:
		ScreenList(IMainGame* game);
		~ScreenList();
		IGameScreen* moveNext();
		IGameScreen* movePrevious();

		void setScreen(unsigned int nextScreen);
		void addScreen(IGameScreen* newScreen);

		void destroy();

		IGameScreen* getCurrent();
	protected:
		IMainGame* m_game = nullptr;
		std::vector<IGameScreen*>m_screens;
		int m_currentScreenIndex = -1;
	};
}