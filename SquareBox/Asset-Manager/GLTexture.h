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
			glm::ivec2 tiling = glm::ivec2(0);

			//some texture helper functions
			glm::vec4 getUVReactAtIndex(int index_) {
				if (tiling.x > 0 && tiling.y > 0) {
					int xTile = index_ % tiling.x;
					int yTile = index_ / tiling.x;

					glm::vec4 uvs;
					uvs.x = xTile / (float)tiling.x;
					uvs.y = yTile / (float)tiling.y;
					uvs.z = 1.0f / tiling.x;
					uvs.w = 1.0f / tiling.y;
					return uvs;
				}
				else {
					return glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
				}
			}
			/*
			only keep texture dependant data here and nothing specific to any cluster object
			*/
		};
	}
}