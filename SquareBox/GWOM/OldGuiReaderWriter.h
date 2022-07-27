#pragma once
#include "GuiLayer.h"
namespace SquareBox {
	//Game World Object Model
	namespace GWOM {

		class OldGuiReaderWriter
		{
		public:
			bool saveGuiLayerDataAsBinary(const std::string& file_path_, const SquareBox::GWOM::GuiLayer & gui_layer_);
			bool loadGuiLayerDataAsBinary(const std::string& file_path_, SquareBox::GWOM::GuiLayer & gui_layer_);
		};
	};

};
