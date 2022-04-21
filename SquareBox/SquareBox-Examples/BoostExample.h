#pragma once
#include "IExample.h"
namespace SquareBox {
	namespace Example {
		class BoostExample :public SquareBox::Example::IExample
		{
		public:
			// Inherited via IExample
			virtual void init() override;
			virtual void onUpdate() override;
			virtual void dispose() override;
		};
	}
}
