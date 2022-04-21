#pragma once
#include<vector>
#include<string>

namespace SquareBox {
	namespace AssetManager {
		struct DirEntry {
			std::string path;
			bool isDirectory;
		};

		class IOManager
		{
		public:
			class PixelData {
			public:
				unsigned char* pixels = nullptr;
				int width = 0;
				int height = 0;
				int bpp;
			};
			static std::pair<char *, int> getRawDataFromFile(std::string file_path_);
			//Remember to call free pixels after this 
			static  PixelData getPixelDataFromImageFile(std::string file_path_);
			static  void freePixelData(PixelData& pixel_data_);
			static bool copyFileFromLocationAtoLocationB(std::string location_a_, std::string location_b_);
			static bool saveRawDataToFile(const char * data_, int len_, std::string file_path_);
		};
	}
}
