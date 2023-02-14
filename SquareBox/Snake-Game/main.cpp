#pragma once
#include "App.h"
int main(int argc, char** argv) {
	App app;
	unsigned windowFlags = 0;
	
	windowFlags |= SquareBox::RenderEngine::WindowFlags::WINDOW_ALLOW_HIGHDPI;
	//windowFlags |= SquareBox::RenderEngine::WindowFlags::WINDOW_MAXIMIZED;
	app.run("Snake Game",700, 400, windowFlags, 15, 0.8f, SquareBox::InputDevicesEnum::KeyBoardAndMouse, SquareBox::VideoDecorderEnum::None, SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::black), "test/SquareBox-Editor.txt", true);

	exit(0);
}



