#pragma once

#include <SquareBox-Core/SquareBoxGameEngine.h>
#include <string>
#include "CustomAppConsole.h"
namespace SquareBoxExample {
		class IExample
		{
		public:
			IExample() {};
			virtual ~IExample() {};
			virtual void onEntry(SquareBox::RenderEngine::Window* _window) = 0;
			virtual void onUpdate(float delta_time_,SquareBox::IMainGame * m_game_ptr_,SquareBox::Camera::ParallelCamera & m_parallel_camera) = 0;
			virtual void onDraw(SquareBox::RenderEngine::SpriteBatch* _spriteBatch, SquareBox::RenderEngine::SpriteFont * _spriteFont, SquareBox::RenderEngine::DebugRenderer * _debugRenderer) = 0;
			virtual void imGuiControls(SquareBoxExample::CustomConsole * _console) = 0;
			virtual void onExit() = 0;
			virtual void reset() = 0;
			bool isExampleInitialised = false;
			std::string exampleTitle = "No Name";
		};
	}

