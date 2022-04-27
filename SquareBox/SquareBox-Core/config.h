#pragma once

#include <Animation-System/AnimationSystem.h>
#include <Audio-System/AudioSystem.h>
#include <Camera/Camera.h>
#include <GWOM/GWOM.h>
#include <Input-Manager/InputManager.h>
#ifndef SQB_PLATFORM_ANDROID
//#include <Network-Manager/NetworkManager.h>
#include <Media-Player/MediaPlayer.h>
#endif // !SQB_PLATFORM_ANDROID

#include <Physics-Collision-Engine/PhysicsCollisionEngine.h>
#include <Render-Engine/RenderingEngine.h>
#include <SquareBox-Logger/Logger.h>
#include <Shapes/Shapes.h>
#include <AI/AI.h>
#include <Asset-Manager/Asset-Manager.h>
#include "IMainGame.h"
#include "IGameScreen.h"
#include "IShape.h"
#include "ScreenList.h"
#include "Essentials.h"
#include "Utilities.h"