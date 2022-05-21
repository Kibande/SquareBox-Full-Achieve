#include "IMainGame.h"
#include <iostream>
#include <memory>


namespace SquareBox {
	IMainGame::IMainGame()
	{
		m_screen_list = std::make_unique<ScreenList>(this);
	}

	IMainGame::~IMainGame()
	{
		//empty
	}

	void IMainGame::run(std::string windowName, int windowWidth, int windowHeight, int windowsFlags, int desiredFPS_, float maxDeltaTime, SquareBox::InputDevicesEnum inputDevice, SquareBox::VideoDecorderEnum videoDecorder, SquareBox::RenderEngine::ColorRGBA8 windowBGColor, std::string log_file_name_, bool write_to_log_file_)
	{
		m_windowBGColor = windowBGColor;
		m_window_name = windowName;
		m_window_height = windowHeight;
		m_max_delta_time = maxDeltaTime;
		m_window_width = windowWidth;
		m_input_device = inputDevice;
		m_video_decorder = videoDecorder;
		m_window_flags = windowsFlags;
		m_fps = desiredFPS_;
		m_logs_file = log_file_name_;
		SquareBox::Log::Init(log_file_name_, write_to_log_file_);// this should be intialised first
		if (!init())
		{
			return;
		}



		// Some helpful constants.
		const float DESRED_FPS = m_fps; // FPS the game was designed to run at 
		const int & MAX_UPATE_LOOPS = 8; // Max number of physics steps per frame
		const float & MS_PER_SECOND = 1000; // Number of milliseconds in a second
		const float & DESIRED_FRAMETIME = MS_PER_SECOND / DESRED_FPS; // The desired frame time per frame
		
		SquareBox::RenderEngine::FpsLimiter limiter;
		limiter.setMaxFPS(DESRED_FPS);
		
		// Start our previousTicks variable
		Uint32 previousTicks = SDL_GetTicks();
		//Game loop
		m_is_running = true;
		while (m_is_running)
		{
			if (m_can_game_exit || m_input_manager->getExitStatus()) {
				//Exit the game here!
				m_can_game_exit = true;
				exitGame();
				continue;
			}
			else {
				m_window.update();
				m_window_width = m_window.getScreenWidth();
				m_window_height = m_window.getScreenHeight();

				// Calculate the frameTime in milliseconds
				limiter.begin();
				const Uint32 & newTicks = SDL_GetTicks();
				const Uint32 & frameTime = newTicks - previousTicks;
				previousTicks = newTicks; // Store newTicks in previousTicks so we can use it next frame
				// Get the total delta time
				float  totalDeltaTime = frameTime / DESIRED_FRAMETIME;

				int i = 0; // This counter makes sure we don't spiral to death!
				// Loop while we still have steps to process.
				while (totalDeltaTime > 0.0f && i < MAX_UPATE_LOOPS) {
					//printf("DELTA TIME %f \n", totalDeltaTime);
					// The deltaTime should be the the smaller of the totalDeltaTime and MAX_DELTA_TIME
					const float & deltaTime = std::min(totalDeltaTime, m_max_delta_time);
					// Update all physics here and pass in deltaTime

					update(deltaTime);
					/*Since the update function sometimes gets info from the inputManager ,
					its important to have them update ant the same time */
					if (m_use_get_input)
					{
						// update the input manager if we are processing input
						m_input_manager->update();
						m_input_manager->processInput();
					}
					// Since we just took a step that is length deltaTime, subtract from totalDeltaTime
					totalDeltaTime -= deltaTime;
					// Increment our frame counter so we can limit steps to MAX_PHYSICS_STEPS
					i++;
				}

				draw();
				m_fps = limiter.end();
				m_current_frame_count++;
				if (std::fabs(m_fps - m_current_frame_count) <= 2) {
					m_game_loop_elapsed_seconds++;
					m_current_frame_count = 0;
				}
				m_window.swapBuffer();
			}
		}
	}

	void IMainGame::exitGame()
	{
		m_current_screen_ptr->onExit();//this is the point where the game is permitted to actually exit or not

		//the current screens' OnExit Function has a chance to stop the games exit
		if (m_can_game_exit) {
			if (m_screen_list) {
				m_screen_list->destroy();
				m_screen_list.reset(); // frees all the smart pointer memory for us
				m_window.destory();//sdl clean up
				m_input_manager->dispose(); //dispose the input manager and free up memory
				delete m_input_manager;//release inputManager Memory
#ifndef SQB_PLATFORM_ANDROID

				for (unsigned int i = 0; i < m_vec_of_media_players.size(); i++)
				{
					if (m_vec_of_media_players[i] != nullptr) {
						m_vec_of_media_players[i]->dispose();
						delete m_vec_of_media_players[i];//release MediaPlayer Memory
					}
				}
#endif // !SQB_PLATFORM_ANDROID
			}
			m_is_running = false;
			//Do SDL clean up 
			  SDL_Quit();
			// this is where i think we shall even do the android clean up
			return;
		}
		else {
			//Change the input mangers mind, just incase it was the one responsible
			m_input_manager->abortExit();
		}
	}

	bool IMainGame::init()
	{
		SquareBox::init();
		if (m_input_device == SquareBox::InputDevicesEnum::KeyBoardAndMouse) {
			m_input_manager = new SquareBox::InputManager::Keyboard();
			m_input_manager->init();
		}
		else if (m_input_device == SquareBox::InputDevicesEnum::TouchScreen) {
			m_input_manager = new SquareBox::InputManager::TouchScreen();
			m_input_manager->init();
		}
		else {
			SBX_CORE_CRITICAL("Unsupported Input Device");
			return false;
		}
#ifndef SQB_PLATFORM_ANDROID
		if (m_video_decorder == SquareBox::VideoDecorderEnum::Vlc) {
			SquareBox::MediaPlayer::VLCplayer*tempPlayer = new SquareBox::MediaPlayer::VLCplayer();
			tempPlayer->init();
			m_vec_of_media_players.push_back(tempPlayer);
		}
		else if (m_video_decorder == SquareBox::VideoDecorderEnum::None) {
			//do nothing
		}
		else {
			SBX_CORE_CRITICAL("Unsupported Input Device");
			return false;
		}
#endif
		if (!initSystems()) return false;
		onInit();
		addScreens();
		m_current_screen_ptr = m_screen_list->getCurrent();
		if (m_current_screen_ptr == nullptr) {
			SBX_CORE_CRITICAL("No Game Screen Initialized");
		}
		m_current_screen_ptr->onEntry();
		m_current_screen_ptr->setRunning();

		return true;
	}

	bool IMainGame::initSystems()
	{
		m_window.create(m_window_name, m_window_width, m_window_height, m_window_flags, m_windowBGColor);
		return true;
	}

	void IMainGame::update(const float & deltaTime_)
	{
			switch (m_current_screen_ptr->getState())
			{
			case ScreenState::RUNNING:

				m_current_screen_ptr->update(deltaTime_);
				break;
			case ScreenState::CHANGE_NEXT:
				m_current_screen_ptr->onExit(); //Exit the current screen
				m_current_screen_ptr = m_screen_list->moveNext();//Move to next screen
				if (m_current_screen_ptr)
				{
					m_current_screen_ptr->setRunning();
					m_current_screen_ptr->onEntry();
				}
				break;
			case ScreenState::CHANGE_PREVIOUS:
				m_current_screen_ptr->onExit(); //Exit the current screen
				m_current_screen_ptr = m_screen_list->movePrevious();//Move to next screen
				if (m_current_screen_ptr)
				{
					m_current_screen_ptr->setRunning();
					m_current_screen_ptr->onEntry();
				}
				break;
			case ScreenState::EXIT_APPLICATION:
				exitGame();
				break;
			case ScreenState::NONE:
				//this is not a common use case 
				// when is it used ??
				break;
			default:
				SBX_CORE_CRITICAL("Unknown Screen State");
				break;
			}
	
	}

	void IMainGame::draw()
	{
		//glViewport(0, 0, m_window.getScreenWidth(), m_window.getScreenHeight());//this is just a safety percaution
		if (m_current_screen_ptr&& m_current_screen_ptr->getState() == ScreenState::RUNNING) {
			m_current_screen_ptr->draw();
		}
	}
}