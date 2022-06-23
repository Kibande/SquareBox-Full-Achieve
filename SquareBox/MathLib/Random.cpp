#include "Random.h"
std::mt19937 SquareBox::MathLib::Random::m_random_engine;
std::uniform_int_distribution<std::mt19937::result_type> SquareBox::MathLib::Random::m_distribution;