#pragma once
#include "App.h"
//#include <Windows.h>
int main(int argc, char** argv) {
	App app;
	int windowFlags = 0;
	SquareBox::InputDevicesEnum input_device;
#ifdef SQB_PLATFORM_ANDROID
	input_device = SquareBox::InputDevicesEnum::TouchScreen;
#else
	input_device = SquareBox::InputDevicesEnum::KeyBoardAndMouse;
	//FreeConsole();
#endif // SQB_PLATFORM_ANDROID
	windowFlags |= SquareBox::RenderEngine::WindowFlags::WINDOW_MAXIMIZED;
	windowFlags |= SquareBox::RenderEngine::WindowFlags::WINDOW_RESIZABLE;
	app.run("SquareBox-Core-Sandbox", 600, 600, windowFlags, 60, 0.8f, input_device, SquareBox::VideoDecorderEnum::None, SquareBox::RenderEngine::ColorRGBA8(43, 43, 48, 255),"tilesheet_visualizer_logs.logs",true);


	exit(0);
}

