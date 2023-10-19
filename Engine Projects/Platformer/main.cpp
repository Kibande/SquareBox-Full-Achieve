#pragma once
#include "App.h"
#include<SquareBox-Core/SquareBoxGameEngine.h>
int main(int argc, char** argv) {
	App app;
	unsigned window_flags = SquareBox::RenderEngine::WindowFlags::WINDOW_RESIZABLE;
	app.run("Platformer", 800, 600, window_flags, 60, 0.8f,SquareBox::InputDevicesEnum::KeyBoardAndMouse,SquareBox::VideoDecorderEnum::None,SquareBox::RenderEngine::ColorRGBA8(43, 43, 48, 25),"platformer.logs",true);
}