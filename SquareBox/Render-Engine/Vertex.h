#pragma once
#include "ColorRGBA8.h"
namespace SquareBox {
	namespace RenderEngine {
		struct Position
		{
			float x;	//	4 bytes
			float y;	//	4 bytes
		};

		struct UV {
			float u;
			float v;
		};

		struct Vertex
		{
			Position position;  //	8 bytes

			ColorRGBA8 color;	//	same as unsigned char

			UV uv; //UV texture coordinates

			void setPosition(float x, float y) {
				position.x = x;
				position.y = y;
			}

			void setColor(GLubyte r, GLubyte g, GLubyte b, GLubyte a) {
				color = ColorRGBA8(r, g, b, a);
			}

			void setUV(float u, float v) {
				uv.u = u;
				uv.v = v;
			}

			//make sure that our vertex is a multiple of 4 to avoid alignment issues
		};
	}
}
