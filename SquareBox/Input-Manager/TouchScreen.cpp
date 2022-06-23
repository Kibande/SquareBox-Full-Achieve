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

void SquareBox::InputManager::TouchScreen::onSDLEvent(SDL_Event& evnt_)
{
	switch (evnt_.type)
	{
	case SDL_FINGERDOWN:
		m_screen_location_details_vec.emplace_back(
			static_cast<int>(evnt_.tfinger.fingerId),
			static_cast<float>(evnt_.tfinger.pressure),
			glm::vec2(evnt_.tfinger.x * m_screen_width, evnt_.tfinger.y * m_screen_height));
		/* for mobile we have to multiply with the screen dimensions*/
		break;

	case SDL_FINGERUP:
		for (auto it = m_screen_location_details_vec.begin(); it != m_screen_location_details_vec.end();)
		{
			bool erased = false;
			if ((*it).id == static_cast<int>(evnt_.tfinger.fingerId)) {
				it = m_screen_location_details_vec.erase(it);
				erased = true;
			}
			if (!erased) {
				++it;
			}
		}
		break;
	case SDL_FINGERMOTION:
		/*

		using a an iterator loop that simply replaces the
		target location details freezes the program
		*/
		//for (auto it = m_screen_location_details_vec.begin(); it != m_screen_location_details_vec.end();)
		//{
		//	bool erased = false;
		//	if ((*it).id == static_cast<int>(evnt_.tfinger.fingerId)) {
		//		(*it).pressure = static_cast<float>(evnt_.tfinger.pressure);
		//		(*it).coordinates = glm::vec2(evnt_.tfinger.x, evnt_.tfinger.y);
		//	}
		//}

		for (auto it = m_screen_location_details_vec.begin(); it != m_screen_location_details_vec.end();)
		{
			bool erased = false;
			if ((*it).id == static_cast<int>(evnt_.tfinger.fingerId)) {
				it = m_screen_location_details_vec.erase(it);
				erased = true;
			}
			if (!erased) {
				++it;
			}
		}
		m_screen_location_details_vec.emplace_back(
			static_cast<int>(evnt_.tfinger.fingerId),
			static_cast<float>(evnt_.tfinger.pressure),
			glm::vec2(evnt_.tfinger.x * m_screen_width, evnt_.tfinger.y * m_screen_height));
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

void SquareBox::InputManager::TouchScreen::update(int screen_width_, int screen_height_)
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
	m_screen_width = screen_width_;
	m_screen_height = screen_height_;
}