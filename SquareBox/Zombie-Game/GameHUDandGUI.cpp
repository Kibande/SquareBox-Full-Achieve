#include "GameHUDandGUI.h"
#include "LayerIndicies.h"
void GameHUDandGUI::init(SquareBox::IMainGame * game_, SquareBox::RenderEngine::Window * window_)
{
	//set our references
	m_game_ptr = game_;
	m_window_ptr = window_;
	m_utilities.init();
	yellow_texture = SquareBox::AssetManager::TextureManager::getTextureByName("round_yellow.png", m_window_ptr->getDDPI());
	blue_texture = SquareBox::AssetManager::TextureManager::getTextureByName("round_blue.png", m_window_ptr->getDDPI());

	glm::ivec2 screen_dimensions;
	screen_dimensions.x = std::max(m_window_ptr->getScreenWidth(), m_window_ptr->getScreenHeight());
	screen_dimensions.y = std::min(m_window_ptr->getScreenWidth(), m_window_ptr->getScreenHeight());

	//fireButtonPoint.x = 0.86500174;
	//fireButtonPoint.y = 0.86400175;
	//buttons
	fire_button.texture_info.texture_id = blue_texture.id;
	fire_button.texture_info.texture_type = SquareBox::TextureEnum::SINGLE;
	fire_button.texture_info.uv_rect = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	fire_button.radius = screen_dimensions.y* 0.15;
	fire_button.is_alive = true;
	fire_button.shape = SquareBox::BodyShapeEnum::Circle;
	fire_button.is_hidden = false;
	fire_button.position = glm::vec2(screen_dimensions.x*0.903, screen_dimensions.y*0.2);//Center the camera

	outer_movement_button.texture_info.texture_id = blue_texture.id;
	outer_movement_button.texture_info.texture_type = SquareBox::TextureEnum::SINGLE;
	outer_movement_button.texture_info.uv_rect = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	outer_movement_button.radius = screen_dimensions.y* 0.3;
	outer_movement_button.is_alive = true;
	outer_movement_button.shape = SquareBox::BodyShapeEnum::Circle;
	outer_movement_button.is_hidden = false;
	outer_movement_button.position = glm::vec2(screen_dimensions.x*0.12, screen_dimensions.y*0.2);//Center the camera
	
	inner_movement_button = outer_movement_button;
	inner_movement_button.radius = outer_movement_button.radius*0.2;
	outer_movement_button.color = glm::vec4(255, 255, 255, 125);
	inner_movement_button.color = glm::vec4(255, 255, 255, 255);
}

void GameHUDandGUI::update(SquareBox::Camera::ParallelCamera & game_camera_, glm::vec2 player_position_, bool & is_player_moving_, glm::vec2 & player_direction_, bool & is_firing_amo_)
{
	const std::vector<SquareBox::InputManager::LocationDetails> & locations_of_fingers_in_world = m_game_ptr->getInputDevice()->getScreenLocations();
	const int number_of_fingers_on_screen = locations_of_fingers_in_world.size();

#ifndef SQB_PLATFORM_ANDROID
	//moving
	if (m_game_ptr->getInputDevice()->isInputIdBeingReceived(static_cast<int>(SquareBox::KeyBoardEnum::SAPCE))) {
		is_player_moving_ = true;
	}
	
	//firing
	if (m_game_ptr->getInputDevice()->isInputIdBeingReceived(static_cast<int>(SquareBox::MouseEnum::LEFT_CLICK))) {
		is_firing_amo_ = true;
	}

	//direction
	glm::vec2 focus_screen_input_point = game_camera_.convertScreenToWorld(m_game_ptr->getInputDevice()->getScreenLocations()[0].coordinates);

	//Over writting the players movements on a Pc Only for easier movement
	player_direction_ = glm::normalize(focus_screen_input_point - player_position_);
	if (std::isnan(player_direction_.x) || std::isnan(player_direction_.y)) {
		//player_direction_ = glm::vec2(0, 0);
		/*
			This should never happen , so if it does , i should cross check this
		*/
		
 		SBX_CRITICAL("Is {} {}  NAN {} {} ", player_direction_.x, player_direction_.y,__FILE__,__LINE__);
	}
#endif


#ifdef SQB_PLATFORM_ANDROID
	if (number_of_fingers_on_screen > 0) {
		for (size_t i = 0; i < number_of_fingers_on_screen; i++)
		{
			glm::ivec2 screen_dimensions;
			screen_dimensions.x = std::max(m_window_ptr->getScreenWidth(), m_window_ptr->getScreenHeight());
			screen_dimensions.y = std::min(m_window_ptr->getScreenWidth(), m_window_ptr->getScreenHeight());
			glm::vec2 focus_screen_input_point = game_camera_.convertScreenToWorld(locations_of_fingers_in_world[i].coordinates*glm::vec2(screen_dimensions.x, screen_dimensions.y));

			m_utilities.setCurrentShapePointer(fire_button.shape, &m_current_shape_ptr);
			if (m_current_shape_ptr->containtsPoint(fire_button, focus_screen_input_point)) {
				fire_button.texture_info.texture_id = yellow_texture.id;
				is_firing_amo_ = true;
			}
			else {
				is_firing_amo_ = false;
				fire_button.texture_info.texture_id = blue_texture.id;
			}

			m_utilities.setCurrentShapePointer(outer_movement_button.shape, &m_current_shape_ptr);
			if (m_current_shape_ptr->containtsPoint(outer_movement_button, focus_screen_input_point)) {

				inner_movement_button.position = focus_screen_input_point;
				glm::vec2 original_direction = player_direction_;
				glm::vec2 new_direction = glm::normalize(focus_screen_input_point - outer_movement_button.position);
				if (!std::isnan(new_direction.x) && !std::isnan(new_direction.y)) {
					player_direction_ = new_direction;
					is_player_moving_ = true;
				}
				else {
					SBX_ERROR("Invalid Player location {} {} ",__FILE__,__LINE__);
				}

			}
			else {
				is_player_moving_ = false;
				inner_movement_button.position = outer_movement_button.position;
			}
		}
	}
	else
	{
		is_firing_amo_ = false;
		fire_button.texture_info.texture_id = blue_texture.id;

		is_player_moving_ = false;
		inner_movement_button.position = outer_movement_button.position;
	}
#endif // SQB_PLATFORM_ANDROID



}
void GameHUDandGUI::draw(SquareBox::RenderEngine::SpriteBatch & sprite_batch_, SquareBox::RenderEngine::SpriteFont & sprite_font_, const std::vector<SquareBox::GWOM::Layer>& layers_)
{

#ifdef SQB_PLATFORM_ANDROID
	m_utilities.setCurrentShapePointer(fire_button.shape, &m_current_shape_ptr);
	m_current_shape_ptr->draw(fire_button, sprite_batch_);
	
	m_current_shape_ptr->draw(outer_movement_button, sprite_batch_);
	m_current_shape_ptr->draw(inner_movement_button, sprite_batch_);
#endif // SQB_PLATFORM_ANDROID



	char buffer[256];
	glm::ivec2 screen_dimensions;
	screen_dimensions.x = std::max(m_window_ptr->getScreenWidth(), m_window_ptr->getScreenHeight());
	screen_dimensions.y = std::min(m_window_ptr->getScreenWidth(), m_window_ptr->getScreenHeight());

	sprintf(buffer, "Human %d", layers_[LayerIndicies::humans_layer_index].alive_cluster_objects.size());
	int font_height=sprite_font_.getFontHeight();
	sprite_font_.draw(sprite_batch_, buffer, glm::vec2(0, screen_dimensions.y - font_height), glm::vec2(0.5f), 0.0f, SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white));
	sprintf(buffer, "Zombies %d", layers_[LayerIndicies::zombies_layer_index].alive_cluster_objects.size());
	sprite_font_.draw(sprite_batch_, buffer, glm::vec2(0, screen_dimensions.y - font_height*2), glm::vec2(0.5f), 0.0f, SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white));

	sprintf(buffer, "Bullets %d", layers_[LayerIndicies::ammunition_layer_index].alive_cluster_objects.size());
	sprite_font_.draw(sprite_batch_, buffer, glm::vec2(0, screen_dimensions.y - font_height*3), glm::vec2(0.5f), 0.0f, SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white));

	sprintf(buffer, "FPS %d", m_game_ptr->getFps());
	sprite_font_.draw(sprite_batch_, buffer, glm::vec2(0, screen_dimensions.y - font_height*4), glm::vec2(0.5f), 0.0f, SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white));

}

void GameHUDandGUI::dispose()
{
	m_utilities.dispose();
}
