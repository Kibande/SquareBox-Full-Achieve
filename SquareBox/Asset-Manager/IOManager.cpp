#include "IOManager.h"
#include <SquareBox-Logger/Logger.h>
#include "stb_image.h"
#include <SDL/include/SDL.h>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
///returns nullptr when we failed to read in the file
	///remember to call delete after use

namespace SquareBox {
	namespace AssetManager {
		std::pair<char *, int> IOManager::getRawDataFromFile(std::string file_path_)
		{
			int total_buffer_size = 0;
			SDL_RWops *src = SDL_RWFromFile(file_path_.c_str(), "rb");
			if (src == NULL) {
				SBX_CORE_ERROR("SDL Failed to load file {} ", file_path_);
				return std::pair<char *, int>(nullptr, 0);
			}

			auto position = SDL_RWtell(src);
			if (position < 0) {
				SDL_RWclose(src);
				SBX_CORE_ERROR("Can't seek in stream");
			}
			const int calculated_size = static_cast<unsigned int>(SDL_RWsize(src));
			char * data_buffer = new char[calculated_size];
			total_buffer_size = calculated_size;

			if (!(src->read(src, data_buffer, calculated_size, 1) > 0))
			{
				delete[] data_buffer;
				data_buffer = nullptr;
				total_buffer_size = 0;
				SBX_CORE_ERROR("Failed to get {} from storage", file_path_);
			}
			SDL_RWclose(src);
			return std::pair<char *, int>(data_buffer, total_buffer_size);
		}
		IOManager::PixelData IOManager::getPixelDataFromImageFile(std::string file_path_)
		{
			/*
		Flip our texture vertically because opengl expects our texture pixels
		to start from the bottom left(0,0) not top left, this is for pngs only !!!
		*/
			PixelData pixel_data;
			stbi_set_unpremultiply_on_load(1);

			std::pair<char*, int> fileBufferInfo = SquareBox::AssetManager::IOManager::getRawDataFromFile(file_path_);
			pixel_data.pixels = stbi_load_from_memory((const stbi_uc*)fileBufferInfo.first, fileBufferInfo.second, &pixel_data.width, &pixel_data.height, &pixel_data.bpp, 4);


			return pixel_data;
		}

		void IOManager::freePixelData(PixelData& pixel_data_)
		{
			if (pixel_data_.pixels != nullptr)
			{
				stbi_image_free(pixel_data_.pixels);
			}
		}
		///this does not over write existing information
		bool IOManager::copyFileFromLocationAtoLocationB(std::string locationA, std::string locationB)
		{
			std::pair<char *, int> fileinfo = getRawDataFromFile(locationA);
			if (fileinfo.first != nullptr && fileinfo.second > 0)
			{
				SDL_RWops *dest = SDL_RWFromFile(locationB.c_str(), "wb");
				if (SDL_RWwrite(dest, fileinfo.first, 1, fileinfo.second) != fileinfo.second) {
					auto char_error_message = SDL_GetError();
					std::string  error_message(char_error_message, sizeof(char_error_message));
					SBX_CORE_ERROR("Failed to Copy {}  to {} error message : {}", locationA, locationB, error_message);
					return false;
				}
				SDL_RWclose(dest);
				return true;
			}
			else {
				SBX_CORE_ERROR("Failed to Copy {}  to {} , file not found!", locationA, locationB);
				return false;
			}
		}
		bool IOManager::saveRawDataToFile(const char * data_, int len_, std::string file_path_)
		{
			SDL_RWops *dest = SDL_RWFromFile(file_path_.c_str(), "wb");
			if (SDL_RWwrite(dest, data_, 1, len_) != len_) {
				auto char_error_message = SDL_GetError();
				std::string  error_message(char_error_message, sizeof(char_error_message));
				SBX_CORE_ERROR("Failed to save bufferObject of size {} to {}", len_, file_path_);
				return false;
			}
			SDL_RWclose(dest);
			return true;
		}
	}
}

/* if we ever need to manually access the Folder*/
			//#include <jni.h>
			//#include <android/asset_manager.h>
			//#include <android/asset_manager_jni.h>
			//initialize our assetManager
			//SBX_CORE_INFO("one");
			//JNIEnv * env = (JNIEnv*)SDL_AndroidGetJNIEnv();
			//jobject activity = (jobject)SDL_AndroidGetActivity();
			//jclass activity_class = env->GetObjectClass(activity);
			//jmethodID activity_class_getAssets = env->GetMethodID(activity_class, "getAssets", "()Landroid/content/res/AssetManager;");
			//jobject asset_manager = env->CallObjectMethod(activity, activity_class_getAssets);//activity.getAssets();
			//auto global_asset_manager = env->NewGlobalRef(asset_manager);
			//auto mgr = AAssetManager_fromJava(env, global_asset_manager);

			//AAssetDir* assetDir = AAssetManager_openDir(mgr, "");
			//const char* filename = (const char*)NULL;

			//while ((filename = AAssetDir_getNextFileName(assetDir)) != NULL) {
			//	SBX_CORE_INFO("we retrived a file {} ", std::string(filename));
			//
			//	if (std::string(filename)==m_file_path)
			//	{
			//		AAsset* asset = AAssetManager_open(mgr, filename, AASSET_MODE_BUFFER);
			//		//AAsset_getBuffer(asset);//point to the m_buffer with all the data
			//		char data_[BUFSIZ];
			//		int nb_read = 0;
			//		FILE* out = fopen(filename, "w");
			//		//fputs(data_, out);
			//		while ((nb_read = AAsset_read(asset, data_, BUFSIZ)) > 0)
			//		{
			//			//fputs(data_, out);
			//			//fwrite(data_, nb_read, 1, out);
			//			/*
			//			Writing to a file doesn't work
			//			i think android doesn't let us write to a FILE dynamically
			//			so this little function will only give you a m_buffer of the asset files data
			//			And its important to remember that the files you are accessing should be in the root of the
			//			assets folder
			//			Author Kibande Steven
			//			date : Thursday July 15 2021 12:45 pm
			//			*/
			//		}
			//		fclose(out);

			//		AAsset_close(asset);
			//		delete out;
			//	}

			//}
			//AAssetDir_close(assetDir);