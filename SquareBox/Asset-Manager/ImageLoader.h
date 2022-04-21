#pragma once
#include<string>
#include "GLTexture.h"
#include  <SquareBox-Logger/Logger.h>
namespace SquareBox {
	namespace AssetManager {
		class ImageLoader
		{
		public:

			static GLTexture getTextureFromImageFile(std::string file_path_);

		};
	}
}
