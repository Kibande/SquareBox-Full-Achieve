#include "Thrid_Screen.h"

Thrid_Screen::Thrid_Screen()
{
}



int Thrid_Screen::getNextScreenIndex() const
{
    return 0;
}

int Thrid_Screen::getPreviousScreenIndex() const
{
    return 0;
}

void Thrid_Screen::build()
{
}

void Thrid_Screen::onEntry()
{
	//m_sprite_font.init("Assets/Fonts/Comfortaa-Bold.ttf", 32);
	//m_utilities.init();
	//m_sprite_batch.init();
	//m_debug_renderer.init();

	////Texture Program
	//m_texture_program.compileShaders("Assets/Shaders/colorShading.vert", "Assets/Shaders/colorShading.frag");
	//m_texture_program.addAttribute("vertexPosition");//since we are using sprite batch we have to always set these attributes
	//m_texture_program.addAttribute("vertexColor");
	//m_texture_program.addAttribute("vertexUV");
	//m_texture_program.linkShaders();

	////the debugProgram
	//m_debug_program.compileShaders("Assets/Shaders/debugShading.vert", "Assets/Shaders/debugShading.frag");
	//m_debug_program.addAttribute("vertexPosition");
	//m_debug_program.addAttribute("vertexColor");
	//m_debug_program.linkShaders();

	////this is where the screens default parameters are set
	//m_layer.camera.init(m_window->getScreenWidth(), m_window->getScreenHeight());
	//m_layer.camera.setScale(162.0f);//The Zoom of the Camera
	//m_layer.camera.setPosition(glm::vec2(0, 0));//Center the camera
	//m_game_ptr->setProcessingInput(true);
	//m_current_shape_ptr = new SquareBox::Shapes::BoxShape();

	////first world cluster
	//m_world_cluster.emplace_back();

	//m_placing_object.shape = SquareBox::BodyShapeEnum::Box;
	//m_placing_object.is_alive = true;
	//m_placing_object.angle = 0.0f;
	//m_placing_object.width = 0.5f;
	//m_placing_object.height = 0.5f;
	//m_placing_object.index = 0;
	//m_placing_object.cluster_index = 0;

	////setting up the texture
	//m_placing_object.texture = SquareBox::AssetManager::TextureManager::getTextureByFilePath("Assets/Textures/isotiles.png");
	//m_placing_object.texture.tiling.x = 5;
	//m_placing_object.texture.tiling.y = 1;
	//SquareBox::AssetManager::TileSheet tileSheet;
	//tileSheet.readGrid(m_placing_object.texture);
	//m_placing_object.uv_rect = tileSheet.getUVCords(2);

	//m_hovering_object = m_placing_object;
	//m_hovering_object.uv_rect = tileSheet.getUVCords(0);
	////Setting up our layers grid
	//m_layer.tiling = SquareBox::LayerTilingEnum::FlatTiling;

	//m_layer.layer_grid.init(-5, -5, 10, 10, 0.5f);

	//m_utilities.init();

}

void Thrid_Screen::update(const float& deltaTime_)
{
	//Called once every game loop , and updates what will be drawn
	//m_window->update();
	//m_layer.camera.update(m_window->getScreenWidth(), m_window->getScreenHeight());
	//m_editor_assitant.cameraControls(m_layer.camera, m_game_ptr);
	//if (m_game_ptr->getInputDevice()->isInputIdReceived((int)SquareBox::MouseEnum::LEFT_CLICK)) {
	//	//place the object in the world
	//	m_world_cluster.back().cluster_objects.push_back(m_placing_object);
	//	m_placing_object.index = m_world_cluster.back().cluster_objects.size() - 1;
	//	m_placing_object.cluster_index = m_world_cluster.size()-1;
	//	m_layer.layer_grid.addObject(m_placing_object);
	//}
}
void Convert2dToIso(glm::vec2 & point)
{
	float tmpX = point.x - point.y;
	float tmpY = (point.x + point.y) * 0.5;
	point.x = tmpX;
	point.y = tmpY;
}

void ConvertIsoTo2D(glm::vec2& point)
{
	float tmpX = (2 * point.y + point.x) * 0.5;
	float tmpY = (2 * point.y - point.x) * 0.5;
	point.x = tmpX;
	point.y = tmpY;
}
void Thrid_Screen::draw()
{
	//////Draw call
	//m_texture_program.use();

	//IGameScreen::preUpdateShader(&m_texture_program, "mySampler");

	//IGameScreen::uploadCameraInfo(&m_texture_program, &m_layer.camera, "P");

	//m_sprite_batch.begin(SquareBox::RenderEngine::GlyphSortType::FRONT_TO_BACK);

	//if (m_layer.layer_grid.isInitialised() && m_layer.tiling == SquareBox::LayerTilingEnum::FlatTiling) {
	//	glm::vec2 camera_center = m_layer.camera.getPosition();
	//	float camera_width = m_layer.camera.getCameraDimensions().x;
	//	float camera_height = m_layer.camera.getCameraDimensions().y;
	//	glm::vec4 camera_destRect(camera_center - glm::vec2(camera_width, camera_height), glm::vec2(camera_width, camera_height) * 2.0f);

	//	auto cells_in_camera_view = m_layer.layer_grid.getPopulatedCellsInBox(camera_destRect);
	//	for (auto it = cells_in_camera_view.rbegin(); it != cells_in_camera_view.rend(); ++it)
	//	{

	//		for (unsigned int i = 0; i < (*it).second->member_cordinates.size(); i++)
	//		{
	//		SquareBox::GWOM::ClusterObject& ccoj = m_world_cluster[(*it).second->member_cordinates[i].first].cluster_objects[(*it).second->member_cordinates[i].second];
	//		m_utilities.setCurrentShapePointer(ccoj.shape, &m_current_shape_ptr);
	//		m_current_shape_ptr->draw(ccoj, m_sprite_batch);

	//		//draw the index
	//		char buffer[256];
	//		sprintf(buffer, "%d :: %d", (*it).first,(*it).second->index);
	//		m_sprite_font.draw(m_sprite_batch, buffer, ccoj.position, glm::vec2(0.004f), 0.0f, SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white));
	//		}
	//	}

	//}

	////draw the hovering object
	//m_current_shape_ptr->draw(m_hovering_object, m_sprite_batch);

	////Display the current FPS
	////	char buffer[256];
	////	m_sprite_font or m_sprite_batch draw
	////	sprintf(buffer, "FPS %d", m_game_ptr->getFps());
	////	m_sprite_font.draw(m_sprite_batch, buffer, m_layer.camera.getPosition(), glm::vec2(0.01f), 0.0f, SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white));
	//m_sprite_batch.end();
	//m_sprite_batch.renderBatch();
	//m_texture_program.unuse();

	//m_debug_program.use();
	//m_debug_renderer.begin();
	////draw grid
	//if (m_layer.layer_grid.isInitialised() && m_layer.tiling == SquareBox::LayerTilingEnum::FlatTiling) {
	//	glm::vec2 camera_center = m_layer.camera.getPosition();
	//	float camera_width = m_layer.camera.getCameraDimensions().x;
	//	float camera_height = m_layer.camera.getCameraDimensions().y;
	//	glm::vec4 camera_destRect(camera_center - glm::vec2(camera_width, camera_height), glm::vec2(camera_width, camera_height)*2.0f);
	//	
	//	m_debug_renderer.drawBox(camera_destRect, SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::pink), 0.0f);

	//	auto cells_in_camera_view = m_layer.layer_grid.getAllCellsInDestRect(camera_destRect);

	//	for (auto it = cells_in_camera_view.begin(); it != cells_in_camera_view.end(); it++)
	//	{
	//		glm::vec2 mouse_in_world = m_layer.camera.convertScreenToWorld(m_game_ptr->getInputDevice()->getScreenLocations()[0].coordinates);

	//		ConvertIsoTo2D(mouse_in_world);

	//		//the isometric view
	//		glm::vec2 position = (*it).second->position;
	//		
	//		float width = m_layer.layer_grid.getCellSize();
	//		float height = m_layer.layer_grid.getCellSize();
	//	
	//		 bool is_being_box_being_hovered= m_utilities.isInBox(mouse_in_world, glm::vec4(position,width,height));

	//		glm::vec2 point_a = position;
	//		glm::vec2 point_b = glm::vec2(position.x + width, position.y);
	//		glm::vec2 point_c = glm::vec2(position.x + width, position.y+ height);
	//		glm::vec2 point_d = glm::vec2(position.x, position.y + height);

	//		//convert points to isometric ones
	//		Convert2dToIso(point_a);
	//		Convert2dToIso(point_b);
	//		Convert2dToIso(point_c);
	//		Convert2dToIso(point_d);

	//		if (is_being_box_being_hovered || (*it).second->index==0) {// are always highlighting the m_origin cell
	//			m_hovering_object.position = (*it).second->position+glm::vec2(m_layer.layer_grid.getCellSize())*0.5f;
	//			Convert2dToIso(m_hovering_object.position);
	//			//copy the converted position
	//			m_placing_object.position = m_hovering_object.position;
	//			m_debug_renderer.drawLine(point_a, point_b, SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::yellow));
	//			m_debug_renderer.drawLine(point_b, point_c, SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::yellow));
	//			m_debug_renderer.drawLine(point_c, point_d, SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::yellow));
	//			m_debug_renderer.drawLine(point_d, point_a, SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::yellow));

	//		}
	//		else {
	//			m_debug_renderer.drawLine(point_a, point_b, SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white));
	//			m_debug_renderer.drawLine(point_b, point_c, SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white));
	//			m_debug_renderer.drawLine(point_c, point_d, SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white));
	//			m_debug_renderer.drawLine(point_d, point_a, SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white));

	//		}
	//
	//	}


	//}


	////drawing the scaling area
	//m_debug_renderer.end();
	//uploadCameraInfo(&m_debug_program, &m_layer.camera, "P");
	//m_debug_renderer.render();
	//m_debug_program.unuse();
}

void Thrid_Screen::onExit()
{
	//m_utilities.dispose();
 //   m_sprite_batch.dispose();
 //   m_sprite_font.dispose();
 //   m_texture_program.dispose();
	//delete m_current_shape_ptr;
	//exit(0);//this is not advisable but it gets the job done
}

void Thrid_Screen::destroy()
{
}

Thrid_Screen::~Thrid_Screen()
{
}