#pragma once
#include "App.h"
int main(int argc, char** argv) {
	App app;
	unsigned windowFlags = 0;
	
	windowFlags |= SquareBox::RenderEngine::WindowFlags::WINDOW_ALLOW_HIGHDPI;
	//windowFlags |= SquareBox::RenderEngine::WindowFlags::WINDOW_MAXIMIZED;
	SquareBox::InputDevicesEnum input_device;

#ifdef SQB_PLATFORM_ANDROID
	input_device = SquareBox::InputDevicesEnum::TouchScreen;
	//windowFlags |= SquareBox::RenderEngine::WindowFlags::WINDOW_FULLSCREEN;
	//windowFlags |= SquareBox::RenderEngine::WindowFlags::WINDOW_BORDERLESS;
	//windowFlags |= SquareBox::RenderEngine::WindowFlags::WINDOW_FULLSCREEN_DESKTOP;
	//windowFlags |= SquareBox::RenderEngine::WindowFlags::WINDOW_MAXIMIZED;

#else
	input_device = SquareBox::InputDevicesEnum::KeyBoardAndMouse;
	//windowFlags |= SquareBox::RenderEngine::WindowFlags::WINDOW_RESIZABLE;
#endif // SQB_PLATFORM_ANDROID
	app.run("Snake Game",400, 600, windowFlags, 15, 0.8f, input_device, SquareBox::VideoDecorderEnum::None, SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::black), "test/SquareBox-Editor.txt", true);

	exit(0);
}



