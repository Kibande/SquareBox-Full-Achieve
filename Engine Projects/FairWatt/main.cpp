#include "App.h"

int main(int argc, char** argv) {
	App app;
	unsigned windowsFlags = 0;
	windowsFlags |= SquareBox::RenderEngine::WindowFlags::WINDOW_RESIZABLE;

	SquareBox::InputDevicesEnum input_device = SquareBox::InputDevicesEnum::KeyBoardAndMouse;

	app.run("FairWatt",800,400,windowsFlags,60,0.8f,input_device,SquareBox::VideoDecorderEnum::None,SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::purple),"fairwatt.log",true);

	exit(0);
}