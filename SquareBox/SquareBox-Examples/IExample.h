#pragma once

#include<Render-Engine/RenderingEngine.h>
#include <string>
#include "CustomAppConsole.h"
namespace SquareBox {
	namespace Example {
		class IExample
		{
		public:
			IExample() {};
			virtual ~IExample() {};
			virtual void onEntry(SquareBox::RenderEngine::Window* _window) = 0;
			virtual void onUpdate() = 0;
			virtual void onDraw(SquareBox::RenderEngine::SpriteBatch* _spriteBatch, SquareBox::RenderEngine::SpriteFont * _spriteFont, SquareBox::RenderEngine::DebugRenderer * _debugRenderer) = 0;
			virtual void imGuiControls(CustomAppConsole * _console) = 0;
			virtual void onExit() = 0;
			virtual void reset() = 0;
			bool isExampleInitialised = false;
			std::string exampleTitle = "No Name";
		};
	}
}
