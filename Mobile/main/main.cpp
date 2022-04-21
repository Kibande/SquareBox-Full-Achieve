#include "App.h"
int main(int argc, char** argv) {
	App app;

	int windowFlags = 0;// Bitwise representation of the window attributes
	//windowFlags += SquareBox::RenderEngine::WindowFlags::WINDOW_BORDERLESS;
	//windowFlags += SquareBox::RenderEngine::WindowFlags::WINDOW_ALWAYS_ON_TOP;
	windowFlags += SquareBox::RenderEngine::WindowFlags::WINDOW_RESIZABLE;
	SDL_DisplayMode mode;
	SDL_GetDisplayMode(0, 0, &mode);
	int width = mode.w;
	int height = mode.h;

	app.run("SquareBox Example", width, height,false, windowFlags, SquareBox::InputDevicesEnum::KeyBoardAndMouse, SquareBox::VideoDecorderEnum::None, SquareBox::RenderEngine::ColorRGBA8(43, 43, 48, 255));
	/* Argumets
		The Windows Title
		Window Dimensions
		Window flags
		InputDevice type
		Background Color of The window

*/
//	the int argc, char** argv  main commandline agrugments are required by sdl 
//  so they are must have 
}