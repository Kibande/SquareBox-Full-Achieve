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
			static GLTexture getTextureFromImageBuffer(std::pair<float*, int> file_buffer_info_, int width_, int height_);

		private:
			static GLTexture createGLTexture(std::pair<char*, int> file_buffer_info_,const std::string file_path_);

		};
	}
}
