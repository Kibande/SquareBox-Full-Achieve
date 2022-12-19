#include "SoundEffect.h"
namespace SquareBox {
	namespace AudioSystem {
		SoundEffect::SoundEffect()
		{
		}

		SoundEffect::~SoundEffect()
		{
		}

		SoundEffect::SoundEffect(std::string unique_name_, std::string  file_path_, char * data_, int data_length_) {
			m_file_path = file_path_;
			m_data = data_;
			m_data_length = data_length_;
			m_unique_name = unique_name_;

		}
	};
};