#pragma once
#include <string>
#include<Render-Engine/RenderingEngine.h>
#include<Camera/Camera.h>
#define SCREEN_INDEX_NO_SCREEN -1
namespace SquareBox {
	class IMainGame;
	enum class ScreenState
	{
		RUNNING,
		EXIT_APPLICATION,
		CHANGE_NEXT,
		CHANGE_PREVIOUS
	};

	class IGameScreen
	{
	public:
		friend class ScreenList;

		IGameScreen() {
		};
		~IGameScreen() {
		};
		//returns the index of next or previous screen when changing screens
		virtual int getNextScreenIndex() const = 0;
		virtual int getPreviousScreenIndex() const = 0;

		///Called when this screen is added to the list of screens
		///this happens at the start of the application
		virtual void build() = 0;
		///Called when this screen comes to focus
		virtual void onEntry() = 0;
		///Called once ever frame to update logic
		virtual void update(const float & deltaTime_) = 0;
		///Called once ever frame to draw to the screen
		virtual void draw() = 0;
		///These are called when a screen enters and leaves focus
		virtual void onExit() = 0;
		/// Called when this screen is removed from the list of screen 
		///this happens at the end of the application
		virtual void destroy() = 0;

		//getters
		int getScreenIndex() const {
			return m_screen_index;
		}
		void setRunning() {
			m_current_state = ScreenState::RUNNING;
		}

		void setScreenState(ScreenState new_state) {
			m_current_state = new_state;
		}

		//setters
		void setParentGame(IMainGame* game) { m_game_ptr = game; }

		ScreenState getState() const { return m_current_state; }
	protected:
		//Called in the main game loop to update our opengl variables
		void preUpdateShader(SquareBox::RenderEngine::GLSLProgram* program, std::string textureUnifromName);
		void uploadCameraInfo(SquareBox::RenderEngine::GLSLProgram * program, SquareBox::Camera::ParallelCamera* camera, std::string cameraUniformName);

		ScreenState m_current_state = ScreenState::RUNNING;
		IMainGame* m_game_ptr = nullptr;//A pointer to  the main game contoller
		int m_screen_index = -1;//-1 means we are not currently an active screen
	};
}
