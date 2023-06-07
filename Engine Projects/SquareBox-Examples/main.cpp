#include "App.h"
int main(int argc, char** argv) {
	App app;

	int windowFlags = 0;// Bitwise representation of the window attributes
	//windowFlags += SquareBox::RenderEngine::WindowFlags::WINDOW_BORDERLESS;
	//windowFlags += SquareBox::RenderEngine::WindowFlags::WINDOW_ALWAYS_ON_TOP;
	windowFlags += SquareBox::RenderEngine::WindowFlags::WINDOW_RESIZABLE;

	app.run("Early Stroke Detection", 900, 600,windowFlags,60,0.8f,SquareBox::InputDevicesEnum::KeyBoardAndMouse,SquareBox::VideoDecorderEnum::None,SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::grey),"Square_box_examples.log",true);
	//	the int argc, char** argv  main commandline agrugments are required by sdl
	//  so they are a must have
}


