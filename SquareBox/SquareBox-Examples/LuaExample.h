#pragma once
#include "IExample.h"
#include <string>
#include <iostream>
#include "Lua/lua.hpp"
namespace SquareBox {
	namespace Example {
		class LuaExample :public SquareBox::Example::IExample
		{
		public:
			// Inherited via IExample
			virtual void init() override;
			virtual void onUpdate() override;
			virtual void dispose() override;

		private:
			std::string cmd;//our test commande
			lua_State *L = nullptr;//pointer to lua state machine
		};
	};
};
