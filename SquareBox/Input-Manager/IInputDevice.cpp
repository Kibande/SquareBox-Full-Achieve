#include "IInputDevice.h"
#include <SquareBox-Logger/Logger.h>
void SquareBox::InputManager::IInputDevice::processInput()
{
	SDL_Event evnt;
	while (SDL_PollEvent(&evnt))
	{
		SBX_SDLCall(onSDLEvent(evnt));//update the input manager for that event
	}
}



void SquareBox::InputManager::IInputDevice::receivingId(unsigned int key_id_)
{
	m_key_map[key_id_] = true;
}

void SquareBox::InputManager::IInputDevice::notReceivingId(unsigned int key_id_)
{
	m_key_map[key_id_] = false;
}

bool SquareBox::InputManager::IInputDevice::isInputIdBeingReceived(unsigned int key_id_)
{
	//we don't want want to use the associate array approach here
	//because we don't want to create a key if it doesn't exist.
	//so we do it manually.

	auto it = m_key_map.find(key_id_);
	if (it != m_key_map.end()) {
		return it->second;
	}
	else {
		return false;
	}
}

bool SquareBox::InputManager::IInputDevice::isInputIdReceived(unsigned int key_id_)
{
	//if the key is pressed this frame but it want pressed the previous frame
	if (isInputIdBeingReceived(key_id_) == true && wasReceivingId(key_id_) == false) {
		return true;
	}
	return false;
}

const glm::vec3 & SquareBox::InputManager::IInputDevice::getTiltAngels()
{
#ifdef SQB_PLATFORM_ANDROID

#endif // SQB_PLATFORM_ANDROID

	return m_device_tilt;
}

bool SquareBox::InputManager::IInputDevice::wasReceivingId(unsigned int key_id_)
{
	auto it = m_previous_key_map.find(key_id_);
	if (it != m_previous_key_map.end()) {
		return it->second;
	}
	else {
		//we didn't find the key
		return false;
	}
}