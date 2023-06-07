#include <SquareBox-Core/SquareBoxGameEngine.h>
#include "App.h"

int main(int argc, char ** argv) {
	int window_flags = 0;
	App app;
	SquareBox::InputDevicesEnum input_device_type;
#ifdef SQB_PLATFORM_ANDROID
	input_device_type = SquareBox::InputDevicesEnum::TouchScreen;
	window_flags |= SquareBox::RenderEngine::WindowFlags::WINDOW_FULLSCREEN;

#else
	input_device_type = SquareBox::InputDevicesEnum::KeyBoardAndMouse;
	window_flags |= SquareBox::RenderEngine::WindowFlags::WINDOW_RESIZABLE;
#endif
	app.run("Ball Game", 1000, 600, window_flags,60,0.8f,input_device_type,SquareBox::VideoDecorderEnum::None,SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::grey),"logFile.txt",false);
	return 0;
	//exit(1);
}