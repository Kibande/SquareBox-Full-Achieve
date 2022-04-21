#include "Timing.h"
namespace SquareBox {
	namespace RenderEngine {
		FpsLimiter::FpsLimiter()
		{
		}

		FpsLimiter::~FpsLimiter()
		{
		}

		void FpsLimiter::init(Uint32 maxFPS)
		{
			setMaxFPS(maxFPS);
		}

		void FpsLimiter::setMaxFPS(Uint32 maxFPS)
		{
			_maxFPS = maxFPS;
		}

		void FpsLimiter::begin()
		{
			_startTicks = SDL_GetTicks();
		}

		Uint32 FpsLimiter::end()
		{
			calculateFPS();
			//get the ticks from the ending of the game loop
			Uint32 endTicks = SDL_GetTicks();
			Uint32 frameTime = endTicks - _startTicks;
			//limit the FPS TO the maxFPS

			//check if we need to limit the FPS
			if (1000.0f / _maxFPS > frameTime) {
				//delay the programe
				SDL_Delay((Uint32)(1000.0f / _maxFPS - frameTime));
			}
			return _fps;
		}

		void FpsLimiter::calculateFPS()
		{
			static const Uint32 NUM_SAMPLES = 10;
			//The array we shall used to average the FPS
			static Uint32 frameTimes[NUM_SAMPLES];

			static Uint32 currentFrame = 0;
			//Get previous milliseconds since SDL was first initialised
			static Uint32 prevTicks = SDL_GetTicks();
			Uint32 currentTicks;
			//Get current milliseconds since SDL was first initialised
			currentTicks = SDL_GetTicks();
			//the time this frame took to be drawn
			_frameTime = currentTicks - prevTicks;
			//add it to the array of Frametimes , at a specific index
			frameTimes[currentFrame%NUM_SAMPLES] = _frameTime;
			prevTicks = currentTicks;
			Uint32 count;
			currentFrame++;
			if (currentFrame < NUM_SAMPLES) {
				count = currentFrame;
			}
			else {
				count = NUM_SAMPLES;
			}
			//moving average
			Uint32 frameTimeAverage = 0;
			for (Uint32 i = 0; i < count; i++)
			{
				frameTimeAverage += frameTimes[i];
			}

			frameTimeAverage /= count;

			if (frameTimeAverage > 0) {
				//calcuctating the FPS
				_fps = 1000.0f / frameTimeAverage;
			}
			else
			{
				_fps = 60.0f;
			}
		}
	}
}