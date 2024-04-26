#pragma once
#include <SDL.h>
namespace SquareBox {
	namespace RenderEngine {
		class FpsLimiter
		{
		public:
			FpsLimiter();
			~FpsLimiter();
			void init(Uint32 maxFPS);
			void setMaxFPS(Uint32 maxFPS);
			void begin();
			Uint32 end();
		private:
			void calculateFPS();
			Uint32 _maxFPS = 0;
			Uint32 _startTicks = 0;
			Uint32 _fps = 0;
			Uint32 _frameTime = 0;
		};
	}
}