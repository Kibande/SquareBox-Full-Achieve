#pragma once
#include"IInputDevice.h"
#include <SDL/include/SDL.h>

namespace SquareBox {
	namespace InputManager {
		class Keyboard :public SquareBox::InputManager::IInputDevice
		{
		public:
			Keyboard();
			~Keyboard();

			// Inherited via IInputDevice
			virtual void init() override;

			// Inherited via IInputDevice
			virtual void onSDLEvent(SDL_Event & evnt_) override;

			virtual void dispose() override;

			// Inherited via IInputDevice
			virtual void update(int screen_width_, int screen_height_) override;
		};
	}
}
