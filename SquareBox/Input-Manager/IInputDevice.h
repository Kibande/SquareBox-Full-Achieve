#pragma once
#include<unordered_map>
#include <utility>
#include<glm/glm.hpp>
#include <SDL/include/SDL.h>
#include<vector>
namespace SquareBox {
	namespace InputManager {
		class Keyboard;
		class TouchScreen;
		class LocationDetails
		{
		public:
			LocationDetails() {};

			LocationDetails(int id_, float pressure_, const glm::vec2 & coordinates_):id(id_),pressure(pressure_),coordinates(coordinates_) {};

			int id = 0;
			float pressure = 0.0f;
			glm::vec2 coordinates = {0,0};
		};
		//this is the input interface
		class IInputDevice
		{
		public:
			friend Keyboard;
			friend TouchScreen;

			virtual void init() = 0;

			//	return true if the key is held down , or touch screen pressed
			bool isInputIdBeingReceived(unsigned int key_id_);

			//	return true if the key was just pressed , or touch screen touched
			bool isInputIdReceived(unsigned int key_id_);
			
			//	shows where our input is being recieved in our world
			const std::vector<LocationDetails> & getScreenLocations() { return m_screen_location_details_vec; }
			
			//  returns the devices tilt angle
			const glm::vec3 & getTiltAngels(); 

			//	return true if the input manager registered an exit command
			bool getExitStatus() const { return m_exit; }

			//	cancels out any exit commands that the input manager might have recieved
			void abortExit() { m_exit = false; }

			glm::vec2  getPivotMotion() const & { return m_pivot_motion; }
			void processInput();
			virtual void update(int screen_width_,int screen_height_) = 0;
			virtual void dispose() = 0;

		private:
			bool wasReceivingId(unsigned int key_id_);
			virtual void onSDLEvent(SDL_Event & evnt_) = 0;

			void receivingId(unsigned int key_id_);
			void notReceivingId(unsigned int key_id_);

			std::unordered_map<unsigned int, bool> m_key_map;
			std::unordered_map<unsigned int, bool> m_previous_key_map;
			std::vector<LocationDetails> m_screen_location_details_vec;
			glm::vec3 m_device_tilt;
			bool m_exit = false;
			glm::vec2 m_pivot_motion = glm::vec2(0.0f);
		};
	};
};
