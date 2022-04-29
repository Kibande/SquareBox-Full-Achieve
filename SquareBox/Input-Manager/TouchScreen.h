#pragma once
#include"IInputDevice.h"
#include <SDL/include/SDL.h>
#include<vector>
namespace SquareBox {
	namespace InputManager {
		class TouchScreen :public SquareBox::InputManager::IInputDevice
		{
		public:
			TouchScreen();
			~TouchScreen();
			// Inherited via IInputDevice
			virtual void init() override;

			// Inherited via IInputDevice
			virtual void onSDLEvent(SDL_Event & evnt_) override;

			virtual void dispose() override;

			// Inherited via IInputDevice
			virtual void update() override;
		};
	};
};