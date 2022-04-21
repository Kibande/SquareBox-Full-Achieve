#pragma once
#ifdef SQB_PLATFORM_ANDROID
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#else
#include<GL/glew.h>
#endif
#include <glm/glm.hpp>
#include <string>
namespace SquareBox {
	namespace AssetManager {
		class GLTexture
		{
		public:
			GLTexture() {};
			std::string asset_file_path = "not set";
			std::string display_name = "not set";
			GLuint id = 0;
			int width = 0;
			int height = 0;
			glm::ivec2 tiling;
			/*
			only keep texture dependant data here and nothing specific to any cluster object
			*/
		};
	}
}