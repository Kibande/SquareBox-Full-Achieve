#pragma once
#include <string>
namespace SquareBox {
	//Game World Object Model
	namespace GWOM
	{
		class StringReaderWriter
		{
		public:
			bool saveStringDataAsBinary(const std::string& file_path_, const std::string& data_);
			bool saveLoadDataAsBinary(const std::string& file_path_, std::string& data_);
		};
	};
};

