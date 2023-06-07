#include "GamePlayeScreen.h"


//we need a spriteBatch
SquareBox::RenderEngine::SpriteBatch m_sprite_batch;


// our default texture
SquareBox::AssetManager::GLTexture m_texture;

void luavec4_to_cpp(lua_State* L,glm::vec4& target_, int arg_index) {
	lua_getfield(L, arg_index, "x");
	target_.x = static_cast<float>(lua_tonumber(L, -1));
	lua_pop(L, 1);

	lua_getfield(L, arg_index, "y");
	target_.y = static_cast<float>(lua_tonumber(L, -1));
	lua_pop(L, 1);

	lua_getfield(L, arg_index, "z");
	target_.z = static_cast<float>(lua_tonumber(L, -1));
	lua_pop(L, 1);

	lua_getfield(L, arg_index, "w");
	target_.w = static_cast<float>(lua_tonumber(L, -1));
	lua_pop(L, 1);
}

void luacolor_to_cpp(lua_State* L, SquareBox::RenderEngine::ColorRGBA8& color_, int arg_index) {
	lua_getfield(L, arg_index, "r");
	color_.r = static_cast<float>(lua_tonumber(L, -1));
	lua_pop(L, 1);

	lua_getfield(L, arg_index, "g");
	color_.g = static_cast<float>(lua_tonumber(L, -1));
	lua_pop(L, 1);

	lua_getfield(L, arg_index, "b");
	color_.b = static_cast<float>(lua_tonumber(L, -1));
	lua_pop(L, 1);

	lua_getfield(L, arg_index, "a");
	color_.a = static_cast<float>(lua_tonumber(L, -1));
	lua_pop(L, 1);
}

int lua_SpriteBatchDraw(lua_State* L) {
	float depth = 1.0f;
	SquareBox::RenderEngine::ColorRGBA8 color;
	
	glm::vec4 destRect;
	luavec4_to_cpp(L, destRect, 1);
	glm::vec4 uvRect;
	luavec4_to_cpp(L, uvRect, 2);

	luacolor_to_cpp(L, color, 3);

	m_sprite_batch.draw(destRect,uvRect,m_texture.id,depth,color);

	return 1;
}

GamePlayeScreen::GamePlayeScreen()
{
	

}

int GamePlayeScreen::getNextScreenIndex() const
{
	return 0;
}

int GamePlayeScreen::getPreviousScreenIndex() const
{
	return 0;
}

void GamePlayeScreen::build()
{
	m_camera.init(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight());
	m_camera.setScale(1.0f);
	m_camera.setPosition(glm::vec2(m_game_ptr->getWindow()->getScreenWidth() *0.5f, m_game_ptr->getWindow()->getScreenHeight()*0.5f));

	//Initialize our texture program
	m_texture_program.init();
	m_texture_program.compileDefaultTextureShaders();
	m_texture_program.addDefaultTextureAttributes();
	m_texture_program.linkShaders();

	m_sprite_batch.init();

	const int width = 1000;
	const int height = 1000;
	const int length = (width * height) * 4;

	float* data = new float[length];
	int i = 0;
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			data[i] = 1.0f;
			data[i + 1] = 1.0f;
			data[i + 2] = 1.0f;
			data[i + 3] = 1.0f;
			i += 4;
		}
	}

	auto  fileBuffer = std::pair<float*, int>(data, length);
	m_texture = SquareBox::AssetManager::TextureManager::createTextureFromFileBuffer(fileBuffer, width, height);

	delete[] data; // since the data is stored on the CPU

	L = luaL_newstate();
	luaL_openlibs(L);
	lua_register(L, "SpriteBatchDraw", lua_SpriteBatchDraw);

	CheckLua(L,luaL_dofile(L, "Assets/Animations/animations_file.lua"));

	lua_getglobal(L, "build");

	if (lua_isfunction(L, -1)) {
		lua_pcall(L, 0, 0, 0);	 
	}
}

void GamePlayeScreen::onEntry()
{
	lua_getglobal(L, "onEntry");
	if (lua_isfunction(L, -1)) {
		lua_pcall(L, 0, 0, 0);
		lua_pop(L, 1);
	}
}



void GamePlayeScreen::update(const float & deltaTime_)
{

	//figure out why calling update on the camera is making everything else disappear from the screen
	//m_camera.update(m_game_ptr->getWindow()->getScreenWidth(),m_game_ptr->getWindow()->getScreenHeight());
	//m_camera.setPosition(glm::vec2(m_game_ptr->getWindow()->getScreenWidth() * 0.5f, m_game_ptr->getWindow()->getScreenHeight() * 0.5f));

	lua_getglobal(L, "update");
	if (lua_isfunction(L, -1)) {
		lua_pushnumber(L, deltaTime_);
		lua_pcall(L, 1, 0, 0);
	}
}

void GamePlayeScreen::draw()
{
	m_texture_program.use();
	preUpdateShader(&m_texture_program, "mySampler");
	uploadCameraInfo(&m_texture_program, &m_camera, "P");

	m_sprite_batch.begin();

	lua_getglobal(L, "draw");

	if (lua_isfunction(L, -1)) {
		lua_pcall(L, 0, 0, 0);
	}

	m_sprite_batch.end();
	m_sprite_batch.renderBatch();

	m_texture_program.unuse();

}

void GamePlayeScreen::onExit()
{
	lua_getglobal(L, "onExit");

	if (lua_isfunction(L, -1)) {
		lua_pcall(L, 0, 0, 0);
	}
}

void GamePlayeScreen::destroy()
{
	m_texture_program.dispose();
	m_camera.dispose();
	m_sprite_batch.dispose();
	lua_getglobal(L, "destroy");

	if (lua_isfunction(L, -1)) {
		lua_pcall(L, 0, 0, 0);
	}
	lua_close(L);
	
}


bool GamePlayeScreen::CheckLua(lua_State* L, int r)
{
	//since every lua Command return a number
//this should be an effective error checker
	if (r != LUA_OK) {
		std::string errormsg = lua_tostring(L, -1);
		SBX_CORE_INFO(errormsg.c_str());
		return false;
	}
	return true;
}