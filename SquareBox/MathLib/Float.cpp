#include "Float.h"
#include "Float.h"

float SquareBox::MathLib::Float::divideAndGetFloat(float a_, float b_)
{
	return areFloatsEqual(b_, 0) ? 0 : a_ / b_;
}

bool SquareBox::MathLib::Float::areFloatsEqual(float a_, float b_, float torrelence_)
{
	return  (std::fabs(a_ - b_)) < torrelence_;
}