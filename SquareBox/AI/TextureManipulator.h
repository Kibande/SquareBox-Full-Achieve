#pragma once
#include "IManipulator.h"
#include "MathLib/MathLib.h"
namespace SquareBox {
	namespace AI {
		class TextureManipulator : public SquareBox::AI::IManipultor
		{
		public:
			TextureManipulator(SquareBox::GWOM::ClusterObject & cluster_object_, int start_tile_, int num_tiles_, float duration_);
			// Inherited via IManipultor
			virtual bool update(float delta_time_, SquareBox::GWOM::ClusterObject & cluster_object) override;

		private:
			int m_num_tiles = 0;
			int m_start_tile = 0;
			float m_gradual_tile_gain = 0.0f;
			float m_accumulated_tile_gain = 0.0f;
		};
	};
}
