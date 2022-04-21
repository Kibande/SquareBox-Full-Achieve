#pragma once
#include"IInput.h"
#include <SDL/include/SDL.h>

namespace SquareBox {
	namespace InputManager {
		class Keyboard :public SquareBox::InputManager::IInput
		{
		public:
			Keyboard();
			~Keyboard();

			// Inherited via IInput
			virtual void init() override;

			// Inherited via IInput
			virtual void onSDLEvent(SDL_Event & evnt_) override;

			virtual void dispose() override;

			// Inherited via IInput
			virtual void update() override;
		};
	}
}
