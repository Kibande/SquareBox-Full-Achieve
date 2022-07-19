#include "GameHUDandGUI.h"
#include "LayerIndicies.h"
void GameHUDandGUI::init(SquareBox::IMainGame * game_)
{
	//set our references
	m_game_ptr = game_;
	m_utilities.init();

	//buttons
	// fire button texture is being handled by a separate class because of it compound nature
	m_fire_button_texture.init(m_game_ptr->getWindow()->getDDPI(), SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::green), SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::yellow));
	m_move_button_texture.init(m_game_ptr->getWindow()->getDDPI(),SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::blue),SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::yellow));


	fire_button.is_alive = true;
	fire_button.shape = SquareBox::BodyShapeEnum::Circle;
	fire_button.is_hidden = false;
	move_button = fire_button;

	outer_direction_button.texture_info.texture_id = SquareBox::AssetManager::TextureManager::getTextureByName("movement button body.png", m_game_ptr->getWindow()->getDDPI()).id;
	outer_direction_button.texture_info.texture_type = SquareBox::TextureEnum::SINGLE;
	outer_direction_button.texture_info.uv_rect = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	
	outer_direction_button.is_alive = true;
	outer_direction_button.shape = SquareBox::BodyShapeEnum::Circle;
	outer_direction_button.is_hidden = false;
	

	outer_direction_button.texture_info.color = glm::vec4(255, 255, 255, 255);

	inner_direction_button = outer_direction_button;
	inner_direction_button.texture_info.texture_id = SquareBox::AssetManager::TextureManager::getTextureByName("movement button center.png", m_game_ptr->getWindow()->getDDPI()).id;

	m_avaliable_gui_space_object.texture_info.texture_id = SquareBox::AssetManager::TextureManager::getTextureByName("white background.png", m_game_ptr->getWindow()->getDDPI()).id;
	m_avaliable_gui_space_object.texture_info.texture_type = SquareBox::TextureEnum::SINGLE;
	m_avaliable_gui_space_object.texture_info.uv_rect = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

	m_avaliable_gui_space_object.shape = SquareBox::BodyShapeEnum:: Box;
	m_avaliable_gui_space_object.is_alive = true;
	m_avaliable_gui_space_object.texture_info.color = SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::black).getIVec4();

}

void GameHUDandGUI::update(SquareBox::Camera::ParallelCamera & game_camera_, glm::vec2 player_position_, bool & is_player_moving_, glm::vec2 & player_direction_, bool & is_firing_amo_)
{
	//used while we are rendering our fonts
	m_camera_scale = game_camera_.getScale();
	glm::vec2 camera_dimensions = game_camera_.getCameraDimensions();
	
	glm::vec2 gui_visible_dimenions = camera_dimensions * 0.9f;
	m_gui_space_dest_rect = glm::vec4(game_camera_.getPosition() - gui_visible_dimenions * 0.5f, gui_visible_dimenions);
	m_avaliable_gui_space_object.position = game_camera_.getPosition();
	m_avaliable_gui_space_object.height = m_gui_space_dest_rect.w;
	m_avaliable_gui_space_object.width = m_gui_space_dest_rect.z;

#ifndef SQB_PLATFORM_ANDROID
	//moving
	if (m_game_ptr->getInputDevice()->isInputIdBeingReceived(static_cast<int>(SquareBox::KeyBoardEnum::SPACE))) {
		is_player_moving_ = true;
	}
	
	//firing
	if (m_game_ptr->getInputDevice()->isInputIdBeingReceived(static_cast<int>(SquareBox::MouseEnum::LEFT_CLICK))) {
		is_firing_amo_ = true;
	}

	//direction
	glm::vec2 focus_screen_input_point = game_camera_.convertScreenToWorld(m_game_ptr->getInputDevice()->getScreenLocations()[0].coordinates);

	player_direction_ = glm::normalize(focus_screen_input_point - player_position_);
	if (std::isnan(player_direction_.x) || std::isnan(player_direction_.y)) {
		//player_direction_ = glm::vec2(0, 0);
		/*
			This should never happen , so if it does , i should cross check this
		*/
		
 		SBX_CRITICAL("Is {} {}  NAN {} {} ", player_direction_.x, player_direction_.y,__FILE__,__LINE__);
	}
#else
	const std::vector<SquareBox::InputManager::LocationDetails>& locations_of_fingers_in_world = m_game_ptr->getInputDevice()->getScreenLocations();
	const int number_of_fingers_on_screen = locations_of_fingers_in_world.size();

	//get the control buttons positioning according to the current camera dimensions


	fire_button.radius = m_gui_space_dest_rect.w * 0.06f;
	move_button.radius = fire_button.radius;

	move_button.position = glm::vec2(m_gui_space_dest_rect.x + m_gui_space_dest_rect.z - move_button.radius, m_gui_space_dest_rect.y+ move_button.radius);
	
	fire_button.position = glm::vec2(move_button.position.x, move_button.position.y + fire_button.radius * 4.0f);


	outer_direction_button.radius = m_gui_space_dest_rect.w * 0.17f;
	outer_direction_button.position = glm::vec2(m_gui_space_dest_rect.x + outer_direction_button.radius, m_gui_space_dest_rect.y + outer_direction_button.radius);

	
	inner_direction_button.radius = outer_direction_button.radius;

	is_firing_amo_ = false;
	is_player_moving_ = false;
	m_fire_button_texture.setIsActive(false);
	m_move_button_texture.setIsActive(false);

	inner_direction_button.position = outer_direction_button.position;
	if (number_of_fingers_on_screen > 0) {
	//if (number_of_fingers_on_screen > 0 && m_game_ptr->getInputDevice()->isInputIdBeingReceived(static_cast<int>(SquareBox::MouseEnum::LEFT_CLICK))) {
		for (size_t i = 0; i < number_of_fingers_on_screen; i++)
		{	
			glm::vec2 focus_screen_input_point = game_camera_.convertScreenToWorld(locations_of_fingers_in_world[i].coordinates);
			if (!is_firing_amo_) {
				m_utilities.setCurrentShapePointer(fire_button.shape, &m_current_shape_ptr);
				if (m_current_shape_ptr->containtsPoint(fire_button, focus_screen_input_point)) {
					is_firing_amo_ = true;
					m_fire_button_texture.setIsActive(true);
				}
			}
			
			if (!is_player_moving_) {
				m_utilities.setCurrentShapePointer(move_button.shape, &m_current_shape_ptr);
				if (m_current_shape_ptr->containtsPoint(move_button, focus_screen_input_point)) {
					is_player_moving_ = true;
					m_move_button_texture.setIsActive(true);
				}
			}


			//if (!is_player_moving_) {
				m_utilities.setCurrentShapePointer(outer_direction_button.shape, &m_current_shape_ptr);
				/*
				 A second finger that it probably touching some where elses should not disrupt a players movement
				*/
				if (m_current_shape_ptr->containtsPoint(outer_direction_button, focus_screen_input_point)) {
					inner_direction_button.position = focus_screen_input_point;
					glm::vec2 original_direction = player_direction_;
					glm::vec2 new_direction = glm::normalize(focus_screen_input_point - outer_direction_button.position);
					if (!std::isnan(new_direction.x) && !std::isnan(new_direction.y)) {
						player_direction_ = new_direction;
						///is_player_moving_ = true;
					}
					else {
						// happens when distance is equal to 0
						SBX_ERROR("Invalid Player location {} {} ", __FILE__, __LINE__);
					}

				}
			//}
		
		}
	}

#endif // SQB_PLATFORM_ANDROID
}
void GameHUDandGUI::draw(SquareBox::RenderEngine::SpriteBatch & sprite_batch_, SquareBox::RenderEngine::SpriteFont & sprite_font_, const std::vector<SquareBox::GWOM::Layer>& layers_)
{
	int screen_width = m_game_ptr->getWindow()->getScreenWidth();
	int screen_height = m_game_ptr->getWindow()->getScreenHeight();
	m_utilities.setCurrentShapePointer(m_avaliable_gui_space_object.shape, &m_current_shape_ptr);
	m_current_shape_ptr->draw(m_avaliable_gui_space_object, sprite_batch_,20);
#ifdef SQB_PLATFORM_ANDROID
	m_utilities.setCurrentShapePointer(fire_button.shape, &m_current_shape_ptr);
	m_fire_button_texture.draw(fire_button.position, fire_button.radius*1.2f, sprite_batch_);
	m_move_button_texture.draw(move_button.position, move_button.radius*1.2f, sprite_batch_);
	m_current_shape_ptr->draw(outer_direction_button, sprite_batch_);
	m_current_shape_ptr->draw(inner_direction_button, sprite_batch_);
#endif // SQB_PLATFORM_ANDROID

	char buffer[256];

	sprintf(buffer, "HUMANS %d", layers_[LayerIndicies::humans_layer_index].alive_cluster_objects.size());
	int font_height=sprite_font_.getFontHeight();

	float font_height_portion_of_screen = SquareBox::MathLib::Float::divideAndGetFloat(font_height/ m_camera_scale, m_gui_space_dest_rect.w);

	float distance_from_top = m_gui_space_dest_rect.y + m_gui_space_dest_rect.w * (1 - font_height_portion_of_screen * 1.0f);
	
	sprite_font_.draw(sprite_batch_, buffer, glm::vec2(m_gui_space_dest_rect.x,distance_from_top), glm::vec2(1/m_camera_scale), 0.0f, SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white));
	
	sprintf(buffer, "ZOMBIES %d", layers_[LayerIndicies::zombies_layer_index].alive_cluster_objects.size());
	distance_from_top = m_gui_space_dest_rect.y + m_gui_space_dest_rect.w * (1 - font_height_portion_of_screen * 2.0f);
	sprite_font_.draw(sprite_batch_, buffer, glm::vec2(m_gui_space_dest_rect.x, distance_from_top), glm::vec2(1/m_camera_scale), 0.0f, SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white));

	distance_from_top = m_gui_space_dest_rect.y + m_gui_space_dest_rect.w * (1 - font_height_portion_of_screen * 3.0f);
	sprintf(buffer, "BULLETS %d", layers_[LayerIndicies::ammunition_layer_index].alive_cluster_objects.size());
	sprite_font_.draw(sprite_batch_, buffer, glm::vec2(m_gui_space_dest_rect.x, distance_from_top), glm::vec2(1/m_camera_scale), 0.0f, SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white));

	distance_from_top = m_gui_space_dest_rect.y + m_gui_space_dest_rect.w * (1 - font_height_portion_of_screen * 4.0f);
	sprintf(buffer, "FPS %d", m_game_ptr->getFps());
	sprite_font_.draw(sprite_batch_, buffer, glm::vec2(m_gui_space_dest_rect.x, distance_from_top), glm::vec2(1/m_camera_scale), 0.0f, SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white));

}

void GameHUDandGUI::dispose()
{
	m_fire_button_texture.dispose();
	m_utilities.dispose();
}
