#include "TouchScreen.h"

SquareBox::InputManager::TouchScreen::TouchScreen()
{
}

SquareBox::InputManager::TouchScreen::~TouchScreen()
{
}

void SquareBox::InputManager::TouchScreen::init()
{
	m_screen_location_details_vec.reserve(4);
#ifdef SDL_HINT_ANDROID_TRAP_BACK_BUTTON  
	#undef SDL_HINT_ANDROID_TRAP_BACK_BUTTON //some sdl_hints already defines it 
#endif
#define	SDL_HINT_ANDROID_TRAP_BACK_BUTTON ="1";//allow us to trap the back key on android
}

void SquareBox::InputManager::TouchScreen::onSDLEvent(SDL_Event & evnt_)
{
	switch (evnt_.type)
	{
	case SDL_FINGERMOTION:
	case SDL_FINGERDOWN:
	case SDL_FINGERUP:

		for (int i = 0; i < SDL_GetNumTouchFingers(evnt_.tfinger.touchId); i++)
		{
			SDL_Finger *currentfinger = SDL_GetTouchFinger(evnt_.tfinger.touchId, i);
			m_screen_location_details_vec.emplace_back(
				static_cast<int>(currentfinger->id),
				static_cast<float>(currentfinger->pressure),
				glm::vec2(currentfinger->x, currentfinger->y));
		}
		break;
	case SDL_KEYDOWN:
		receivingId(evnt_.key.keysym.sym);
		break;
	case SDL_KEYUP:
		notReceivingId(evnt_.key.keysym.sym);
		break;
	case SDL_QUIT:
		m_screen_location_details_vec.clear();
		m_exit = true;
		break;
	}
}

void SquareBox::InputManager::TouchScreen::dispose()
{
}

void SquareBox::InputManager::TouchScreen::update()
{
	//loop through the previous keys and set it to the current key
	//goo through each of the key states in our key map

	for (auto it = m_key_map.begin(); it != m_key_map.end(); it++)
	{
		//set that on previous key map
		m_previous_key_map[it->first] = it->second;
	}
	//reset the defaults
	m_pivot_motion.x = 0;
	m_pivot_motion.y = 0;
	m_screen_location_details_vec.clear();

}