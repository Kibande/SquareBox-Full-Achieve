#pragma once
#include"IInput.h"
#include <SDL/include/SDL.h>
#include<vector>
namespace SquareBox {
	namespace InputManager {
		class TouchScreen :public SquareBox::InputManager::IInput
		{
		public:
			TouchScreen();
			~TouchScreen();
			// Inherited via IInput
			virtual void init() override;

			// Inherited via IInput
			virtual void onSDLEvent(SDL_Event & evnt_) override;

			virtual void dispose() override;

			// Inherited via IInput
			virtual void update() override;
		};
	};
};