#include "ImageLoader.h"
#include <SquareBox-Logger/Logger.h>
#include "IOManager.h"
#include <SDL/include/SDL.h>
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
			GLTexture texture;
			unsigned char * localBuffer = nullptr;
			int bpp;//Bits per pixel
			/*
			Flip our texture vertically because opengl expects our texture pixels
			to start from the bottom left(0,0) not top left, this is for pngs only !!!
			*/
			stbi_set_unpremultiply_on_load(1);

			std::pair<char *, int> fileBufferInfo = SquareBox::AssetManager::IOManager::getRawDataFromFile(file_path_);
			localBuffer = stbi_load_from_memory((const stbi_uc*)fileBufferInfo.first, fileBufferInfo.second, &texture.width, &texture.height, &bpp, 4);
            
			if (fileBufferInfo.second > 0) {

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
					SBX_CORE_ERROR("{} of {}X{} was to big for {}X{} max texture size",file_path_, texture.width, texture.height, max_texture_size,max_texture_size);
				}
				//send the data to Open-g-l
				//SBX_GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texture.width, texture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));
				//const int chunck_size = 1024;
				////upload the image data in parts
				//SBX_CORE_INFO("Orignal width {} height {}  ", texture.width, texture.height);
				//int remaining_width = texture.width;
				//int remaining_height = texture.height;
				//int width_offset = 0;
				//int height_offset = 0;
				//while (remaining_width > 0)
				//{
				//	remaining_height = texture.height;//renew height
				//	height_offset = 0;
				//	if (remaining_width > chunck_size) {
				//		//go through the y

				//		while (remaining_height > 0) {

				//			if (remaining_height > chunck_size)
				//			{
				//				//SBX_GLCall(glTexSubImage2D(GL_TEXTURE_2D, 0, width_offset, height_offset, chunck_size, chunck_size, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer));
				//				SBX_GLCall(glTexSubImage2D(GL_TEXTURE_2D, 0, width_offset, height_offset, texture.width, texture.height, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer));

				//				height_offset += chunck_size;
				//				remaining_height -= chunck_size;
				//			}
				//			else {
				//				//send it all in 
				//				//SBX_GLCall(glTexSubImage2D(GL_TEXTURE_2D, 0, width_offset, height_offset, chunck_size, remaining_height, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer));
				//				SBX_GLCall(glTexSubImage2D(GL_TEXTURE_2D, 0, width_offset, height_offset, texture.width, texture.height, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer));
				//				remaining_height = 0;

				//			}
				//		}
				//		width_offset += chunck_size;
				//		remaining_width -= chunck_size;
				//	}
				//	else
				//	{
				//		//loop through the remaining widths height
				//		while (remaining_height > 0) {

				//			if (remaining_height > chunck_size)
				//			{
				//				//SBX_GLCall(glTexSubImage2D(GL_TEXTURE_2D, 0, width_offset, height_offset, remaining_width, chunck_size, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer));
				//				SBX_GLCall(glTexSubImage2D(GL_TEXTURE_2D, 0, width_offset, height_offset, texture.width, texture.height, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer));
				//				remaining_width = 0;
				//				height_offset += chunck_size;
				//				remaining_height -= chunck_size;
				//			}
				//			else {
				//				//send it all in 
				//				//SBX_GLCall(glTexSubImage2D(GL_TEXTURE_2D, 0, width_offset, height_offset, remaining_width, remaining_height, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer));
				//				SBX_GLCall(glTexSubImage2D(GL_TEXTURE_2D, 0, width_offset, height_offset, texture.width, texture.height, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer));
				//				remaining_width = 0;
				//				remaining_height = 0;
				//			}
				//		}

				//	}
				//}
				

				

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
			std::string sub_str;
			size_t npos = file_path_.find_last_of('/');
			if (npos != std::string::npos) {
				texture.display_name = file_path_.substr(npos + 1);
			}
			return texture;
		}


	}
}