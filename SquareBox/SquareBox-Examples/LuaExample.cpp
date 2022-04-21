#include "LuaExample.h"

void SquareBox::Example::LuaExample::init()
{
	cmd = "a = 7 +11";
	L = luaL_newstate();
}

void SquareBox::Example::LuaExample::onUpdate()
{
	if (L != nullptr) {
		int r = luaL_dostring(L, cmd.c_str());

		if (r == LUA_OK) {
			lua_getglobal(L, "a");
			if (lua_isnumber(L, -1))
			{
				float a_in_cpp = (float)lua_tonumber(L, -1);
				std::cout << "a_in_cpp " << a_in_cpp << std::endl;
			}
		}
		else {
			std::string erromsg = lua_tostring(L, -1);
			std::cout << "erromsg " << erromsg << std::endl;
		}
	}
	else {
		std::cout << "Lua State was null" << std::endl;
	}
	system("pause");
}

void SquareBox::Example::LuaExample::dispose()
{
	//clean up
	lua_close(L);
}