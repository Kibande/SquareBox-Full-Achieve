#pragma once
#include <string>
#include<Asset-Manager/Asset-Manager.h>
namespace SquareBox {
	namespace MediaPlayer {
		class IPlayer
		{
		public:

			virtual ~IPlayer() {};

			virtual int init() = 0;
			virtual void play() = 0;
			virtual int setMediaSource(std::string _filePath, SquareBox::AssetManager::GLTexture * _texture, std::string _chromaFormat, bool _isRemoteVideo = false) = 0;

			virtual void decode() = 0;
			virtual void pause() = 0;
			virtual void stop() = 0;
			virtual void setVolumeLevel(int _level) = 0;
			virtual int getVolumeLevel() = 0;
			virtual void dispose() = 0;
		};
	};
}
