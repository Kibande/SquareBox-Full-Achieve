#pragma once
#include <math.h>
#include <algorithm>
#include <cmath>
namespace SquareBox {
	namespace MathLib
	{
		class Float
		{
		public:
			static float divideAndGetFloat(float a_, float b_);
			static  bool areFloatsEqual(float a_, float b_, float torrelence_ = 0.01);
		};
	};
};
