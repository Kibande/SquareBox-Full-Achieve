#pragma once
#include "App.h"
unsigned windowFlags = 0;
int main(int argc, char** argv) {
	App app;
	SquareBox::InputDevicesEnum input_device;
#ifdef SQB_PLATFORM_ANDROID
	input_device = SquareBox::InputDevicesEnum::TouchScreen;
	windowFlags |= SquareBox::RenderEngine::WindowFlags::WINDOW_FULLSCREEN;
	//windowFlags |= SquareBox::RenderEngine::WindowFlags::WINDOW_BORDERLESS;
	//windowFlags |= SquareBox::RenderEngine::WindowFlags::WINDOW_FULLSCREEN_DESKTOP;
	//windowFlags |= SquareBox::RenderEngine::WindowFlags::WINDOW_MAXIMIZED;

#else
	input_device = SquareBox::InputDevicesEnum::KeyBoardAndMouse;
	windowFlags |= SquareBox::RenderEngine::WindowFlags::WINDOW_RESIZABLE;
#endif // SQB_PLATFORM_ANDROID
	
	app.run("Zombie Game",800, 600, windowFlags, 60, 0.8f, input_device, SquareBox::VideoDecorderEnum::None, SquareBox::RenderEngine::ColorRGBA8(43, 43, 48, 255), "test/SquareBox-Editor.txt", true);
}



//#define SERVER = 1;
//#ifdef SERVER
//#include <SquareBox-Core/SquareBoxGameEngine.h>
//
//enum class CustomMsgTypes : uint32_t
//{
//	ServerAccept,
//	ServerDeny,
//	ServerPing,
//	MessageAll,
//	ServerMessage,
//};
//
//
//
//class CustomServer : public SquareBox::net::server_interface<CustomMsgTypes>
//{
//public:
//	CustomServer(uint16_t nPort) : SquareBox::net::server_interface<CustomMsgTypes>(nPort)
//	{
//
//	}
//
//protected:
//	virtual bool OnClientConnect(std::shared_ptr<SquareBox::net::connection<CustomMsgTypes>> client)
//	{
//		SquareBox::net::message<CustomMsgTypes> msg;
//		msg.header.id = CustomMsgTypes::ServerAccept;
//		client->Send(msg);
//		return true;
//	}
//
//	// Called when a client appears to have disconnected
//	virtual void OnClientDisconnect(std::shared_ptr<SquareBox::net::connection<CustomMsgTypes>> client)
//	{
//		std::cout << "Removing client [" << client->GetID() << "]\n";
//	}
//
//	// Called when a message arrives
//	virtual void OnMessage(std::shared_ptr<SquareBox::net::connection<CustomMsgTypes>> client, SquareBox::net::message<CustomMsgTypes>& msg)
//	{
//		switch (msg.header.id)
//		{
//		case CustomMsgTypes::ServerPing:
//		{
//			std::cout << "[" << client->GetID() << "]: Server Ping\n";
//
//			// Simply bounce message back to client
//			client->Send(msg);
//		}
//		break;
//
//		case CustomMsgTypes::MessageAll:
//		{
//			std::cout << "[" << client->GetID() << "]: Message All\n";
//
//			// Construct a new message and send it to all clients
//			SquareBox::net::message<CustomMsgTypes> msg;
//			msg.header.id = CustomMsgTypes::ServerMessage;
//			msg << client->GetID();
//			MessageAllClients(msg, client);
//
//		}
//		break;
//		}
//	}
//};
//
//int main()
//{
//	CustomServer server(60000);
//	server.Start();
//
//	while (1)
//	{
//		server.Update(-1, true);
//	}
//
//
//
//	return 0;
//}
//
//#endif
