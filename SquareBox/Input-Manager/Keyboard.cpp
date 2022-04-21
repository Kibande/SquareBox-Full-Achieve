#include "Keyboard.h"
#include <SquareBox-Logger/Logger.h>
SquareBox::InputManager::Keyboard::Keyboard()
{
}

SquareBox::InputManager::Keyboard::~Keyboard()
{
}

void SquareBox::InputManager::Keyboard::init()
{
	m_screen_location_details_vec.reserve(2);
	m_screen_location_details_vec.emplace_back();//for the window coordinates
	m_screen_location_details_vec.emplace_back();//for the movement in the relative x and y directions
}

void SquareBox::InputManager::Keyboard::onSDLEvent(SDL_Event & evnt_)
{
	switch (evnt_.type)
	{
	case SDL_KEYDOWN:
		receivingId(evnt_.key.keysym.sym);
		break;
	case SDL_KEYUP:
		notReceivingId(evnt_.key.keysym.sym);
		break;
	case SDL_MOUSEBUTTONDOWN:
		receivingId(evnt_.button.button);
		break;
	case SDL_MOUSEBUTTONUP:
		notReceivingId(evnt_.button.button);
		break;
	case SDL_MOUSEMOTION:
		m_screen_location_details_vec[0] = LocationDetails(1, 1, glm::vec2(evnt_.motion.x, evnt_.motion.y));
		m_screen_location_details_vec[1] = LocationDetails(2, 1, glm::vec2(evnt_.motion.xrel, evnt_.motion.yrel));
		break;
	case SDL_MOUSEWHEEL:
		m_pivot_motion.x = static_cast<float>(evnt_.wheel.x);
		m_pivot_motion.y = static_cast<float>(evnt_.wheel.y);
		break;
	case SDL_QUIT:
		m_exit = true;
		break;
	default:
		//this is only hit when we are not recieving game input (mouse not in world)
		break;
		
	}
}

void SquareBox::InputManager::Keyboard::dispose()
{
}

void SquareBox::InputManager::Keyboard::update()
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
	m_screen_location_details_vec[1] = LocationDetails(2, 1, glm::vec2(0, 0));
}