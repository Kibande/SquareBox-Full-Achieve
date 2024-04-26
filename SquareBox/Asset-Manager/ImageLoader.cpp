#include "ImageLoader.h"
#include <SquareBox-Logger/Logger.h>
#include "IOManager.h"
#include <SDL.h>
#include <fstream>
#include <stdio.h>
#include "stb_image.h"
#include "stb_image_resize.h"

/*
	we should add the ability to resize images if that specific resolution is not found 
	we can base on the size thats there , this will enusre best resluytion even on unique devices
*/


namespace SquareBox {
	namespace AssetManager {
		GLTexture ImageLoader::getTextureFromImageFile(std::string file_path_)
		{
			std::pair<char *, int> file_buffer_info = SquareBox::AssetManager::IOManager::getRawDataFromFile(file_path_);
			
			return createGLTexture(file_buffer_info, file_path_);
			
		}

		GLTexture ImageLoader::getTextureFromImageBuffer(std::pair<float*, int> file_buffer_info_, int width_, int height_)
		{
			GLTexture texture;
			SBX_GLCall(glGenTextures(1, &(texture.id)));

			SBX_GLCall(glBindTexture(GL_TEXTURE_2D, texture.id));

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_FLOAT,(void*)file_buffer_info_.first);

			//unbind the texture
			SBX_GLCall(glBindTexture(GL_TEXTURE_2D, 0));

			return texture;
		}

		GLTexture ImageLoader::createGLTexture(std::pair<char*, int> file_buffer_info_,const std::string file_path_)
		{
			GLTexture texture;
			unsigned char* localBuffer = nullptr;
			int bpp;//Bits per pixel
			/*
			Flip our texture vertically because opengl expects our texture pixels
			to start from the bottom left(0,0) not top left, this is for pngs only !!!
			*/
			stbi_set_unpremultiply_on_load(1);

			localBuffer = stbi_load_from_memory((const stbi_uc*)file_buffer_info_.first, file_buffer_info_.second, &texture.width, &texture.height, &bpp, 4);

			if (file_buffer_info_.second > 0) {

				static GLint max_texture_size = 0;//In pixels
				glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
				//creating our Open-g l texture
				SBX_GLCall(glGenTextures(1, &(texture.id)));

				SBX_GLCall(glBindTexture(GL_TEXTURE_2D, texture.id));

				//set up some texture parameters
				SBX_GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));//liner sampling
				SBX_GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));//magnification
				SBX_GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));//yes tiling in y
				SBX_GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));//yes tiling  in x
				SBX_GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texture.width, texture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer));
				if (texture.width > max_texture_size || texture.height > max_texture_size)
				{
					SBX_CORE_ERROR("{} of {}X{} was to big for {}X{} max texture size", file_path_, texture.width, texture.height, max_texture_size, max_texture_size);
				}

				SBX_GLCall(glGenerateMipmap(GL_TEXTURE_2D));
				//unbind the texture
				SBX_GLCall(glBindTexture(GL_TEXTURE_2D, 0));

				if (localBuffer != nullptr)
				{
					stbi_image_free(localBuffer);
				}
				else
				{
					SBX_CORE_ERROR("localBuffer for {} was null , reason {} ", file_path_, stbi_failure_reason());
				}
			}
			else {
				SBX_CORE_ERROR("Failed to load {} ", file_path_);

			}
			texture.asset_file_path = file_path_;
			if (!file_path_.empty()) {
				std::string sub_str;
				size_t npos = file_path_.find_last_of('/');
				if (npos != std::string::npos) {
					texture.display_name = file_path_.substr(npos + 1);
				}
			}
			
			return texture;
		}


	}
}