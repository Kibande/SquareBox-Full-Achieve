#pragma once

#include <Render-Engine/Window.h>
#include <Input-Manager/InputManager.h>
#include "SquareBoxGameEngine.h"
#include <string>
#include <memory>

namespace SquareBox {
	class ScreenList;
	class IGameScreen;
	class IMainGame
	{
	public:
		IMainGame();
		virtual ~IMainGame();
		void run(std::string windowName, int windowWidth, int windowHeight, int windowsFlags, int desiredFPS_, float maxDeltaTime, SquareBox::InputDevicesEnum inputDevice, SquareBox::VideoDecorderEnum videoPlayer, SquareBox::RenderEngine::ColorRGBA8 windowBGColor, std::string log_file_name_, bool write_to_log_file_);
		void exitGame();
		//All our custom stuff after the window is all set up

		const Uint32 getFps() const {
			return m_fps;
		}
		const int getGameLoopElapsedSeconds() const {
			return m_game_loop_elapsed_seconds;
		}
		
		SquareBox::InputManager::IInputDevice * getInputDevice() { return m_input_manager; }
		SquareBox::AudioSystem::AudioEngine* getAudioEngine() { return m_audio_engine_ptr; }
		SquareBox::RenderEngine::Window* getWindow() { return &m_window; }
#ifndef SQB_PLATFORM_ANDROID

		SquareBox::MediaPlayer::IPlayer * getVideoPlayer(unsigned int index = 0) {
			if (index >= 0 && index < m_vec_of_media_players.size()) {
				return m_vec_of_media_players[index];
			}
			else {
				return nullptr;
			}
		}
#endif // !SQB_PLATFORM_ANDROID

		void createNewMediaPlayer() {
#ifndef SQB_PLATFORM_ANDROID
			if (m_video_decorder == SquareBox::VideoDecorderEnum::Vlc && m_vec_of_media_players[0] != nullptr) {
				SquareBox::MediaPlayer::VLCplayer * tempPlayer = new SquareBox::MediaPlayer::VLCplayer;
				tempPlayer->copyVLcInstance(static_cast<SquareBox::MediaPlayer::VLCplayer *>(m_vec_of_media_players[0])->getVlcInstance());
				m_vec_of_media_players.push_back(tempPlayer);
			}
			else {
				SBX_CORE_ERROR("Can only create multiple  VLCPlayers \n");
			}
#endif // !SQB_PLATFORM_ANDROID
		}
		void abortExit() {
			m_can_game_exit = false;
		}
		void initiateExit() {
			m_can_game_exit = true;
		}
		void setProcessingInput(bool status) {
			m_use_get_input = status;
		}
		bool isProcessingInput()const {
			return m_use_get_input;
		}
		std::string getLogsFileName() {
			return m_logs_file;
		}

	protected:
		virtual void onInit() = 0;
		virtual void addScreens() = 0;
		virtual void onExit() = 0;
		//Custom update function
		virtual void update(const float & deltaTime_);
		//Custom render function
		virtual void draw();

		bool init();
		bool initSystems();
		std::unique_ptr <ScreenList> m_screen_list = nullptr;
		IGameScreen* m_current_screen_ptr = nullptr;
		bool m_is_running = false;
		Uint32 m_fps = 60;
		SquareBox::RenderEngine::Window m_window;
		SquareBox::InputManager::IInputDevice *m_input_manager = nullptr;
		SquareBox::AudioSystem::AudioEngine* m_audio_engine_ptr = nullptr;
#ifndef SQB_PLATFORM_ANDROID
		std::vector<SquareBox::MediaPlayer::IPlayer *>m_vec_of_media_players;
#endif
		int m_window_height = 100;
		int m_window_width = 100;
		int m_window_flags = 0;
		std::string m_window_name;
		SquareBox::InputDevicesEnum m_input_device = SquareBox::InputDevicesEnum::KeyBoardAndMouse;
		SquareBox::VideoDecorderEnum m_video_decorder = SquareBox::VideoDecorderEnum::Vlc;
		SquareBox::RenderEngine::ColorRGBA8 m_windowBGColor = SquareBox::RenderEngine::ColorRGBA8(255, 255, 255, 255);
		bool m_use_get_input = true;
		bool m_can_game_exit = false;
		int m_current_frame_count = 0;
		int m_game_loop_elapsed_seconds = 0;
		float m_max_delta_time = 0.8f;
		std::string  m_logs_file;
	};
}
