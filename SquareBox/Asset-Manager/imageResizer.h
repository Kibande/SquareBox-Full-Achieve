#pragma once
#include<SquareBox-Logger/Logger.h>

namespace SquareBox {
	namespace AssetManager {
		class imageResizer
		{
		public:
			char* resizeImage(char* inputPixels_, int in_w_, int in_h_, int in_strid_ , int out_w_, int out_h_, int out_strid_);
		};
	};
	
};


