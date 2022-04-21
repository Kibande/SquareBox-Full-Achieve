#pragma once
#include "IExample.h"
#include <SquareBox-Core/SquareBoxGameEngine.h>
namespace SquareBox {
	namespace Example {
		class LoggerExample :public SquareBox::Example::IExample
		{
			// Inherited via IExample
			virtual void init() override;
			virtual void onUpdate() override;
			virtual void dispose() override;
		};
	}
}
