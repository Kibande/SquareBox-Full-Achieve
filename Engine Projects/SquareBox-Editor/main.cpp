#pragma once
#include "App.h"
int main(int argc, char** argv) {
	App app;
	unsigned windowFlags = 0;
	//windowFlags |= SquareBox::RenderEngine::WindowFlags::WINDOW_FULLSCREEN;
	windowFlags |= SquareBox::RenderEngine::WindowFlags::WINDOW_RESIZABLE;
	windowFlags |= SquareBox::RenderEngine::WindowFlags::WINDOW_ALLOW_HIGHDPI;
	windowFlags |= SquareBox::RenderEngine::WindowFlags::WINDOW_MAXIMIZED;
	app.run("SquareBox Editor", 600, 600, windowFlags,60,0.8f, SquareBox::InputDevicesEnum::KeyBoardAndMouse, SquareBox::VideoDecorderEnum::None, SquareBox::RenderEngine::ColorRGBA8(43, 43, 48, 255),"test/SquareBox-Editor.txt",true);
	     
	exit(0);
}



