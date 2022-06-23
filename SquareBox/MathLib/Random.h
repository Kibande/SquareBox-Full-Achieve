#pragma once
#include <random>
namespace SquareBox {
	namespace MathLib {
		class Random
		{
		public:
			static void Init()
			{
				m_random_engine.seed(std::random_device()());
			}

			static float Float() {
				return (float)m_distribution(m_random_engine) / (float)std::numeric_limits<uint32_t>::max();
			}
		private:
			static std::mt19937 m_random_engine;
			static std::uniform_int_distribution<std::mt19937::result_type> m_distribution;
		};

	}
}

