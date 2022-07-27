#include "GuiReaderWriter.h"
#include "DSUGUIV1_generated.h"

bool SquareBox::GWOM::GuiReaderWriter::saveGuiLayerDataAsBinary(const std::string& file_path_, const SquareBox::GWOM::GuiLayer& gui_layer_)
{
    flatbuffers::FlatBufferBuilder builder(1024);

    //Serializing the Layer
    
    std::vector<flatbuffers::Offset<SquareBox::DSUGUIV1::GUIElement>>  fb_gui_elements;
    for (unsigned int  i = 0; i < gui_layer_.gui_elements.size(); i++)
    {
        const SquareBox::GWOM::GUIElement& focus_gui_element = gui_layer_.gui_elements[i];
       
        std::vector<flatbuffers::Offset<SquareBox::DSUGUIV1::FontInfo>> fb_gui_element_fonts;
        std::vector<flatbuffers::Offset<SquareBox::DSUGUIV1::TextureInfo>> fb_gui_element_textures;

        for (unsigned int j = 0; j < focus_gui_element.fonts.size(); j++)
        {
            const SquareBox::GWOM::FontInfo& focus_font_info = focus_gui_element.fonts[j];
            auto fb_font_info_color = SquareBox::DSUGUIV1::Ivec4(focus_font_info.color.x, focus_font_info.color.y, focus_font_info.color.z, focus_font_info.color.w);
            fb_gui_element_fonts.push_back(SquareBox::DSUGUIV1::CreateFontInfo(builder,
                &fb_font_info_color,
                focus_font_info.font_index,
                builder.CreateString(std::string(focus_font_info.text)),
                focus_font_info.show_text,
                focus_font_info.text_to_box_height_scale,
                focus_font_info.opacity,
                static_cast<int>(focus_font_info.justification)
                )
            );
        }

        for (unsigned int j = 0;  j < focus_gui_element.textures.size();  j++)
        {
            const SquareBox::GWOM::TextureInfo& focus_texture_info = focus_gui_element.textures[j];
            auto focus_texture_info_color = SquareBox::DSUGUIV1::Ivec4(focus_texture_info.color.x, focus_texture_info.color.y, focus_texture_info.color.z, focus_texture_info.color.w);
            auto fb_texture_info_uv_rect = SquareBox::DSUGUIV1::Vec4(focus_texture_info.uv_rect.x, focus_texture_info.uv_rect.y, focus_texture_info.uv_rect.z, focus_texture_info.uv_rect.w);

            fb_gui_element_textures.push_back(SquareBox::DSUGUIV1::CreateTextureInfo(builder, 
                static_cast<int>(focus_texture_info.texture_type),
                focus_texture_info.texture_index,
                focus_texture_info.tile_sheet_index,
                focus_texture_info.texture_id,
                &fb_texture_info_uv_rect,
                &focus_texture_info_color,
                focus_texture_info.opacity
            ));
        }
        auto temp_vec2 = SquareBox::DSUGUIV1::Vec2(focus_gui_element.location_ratio.x, focus_gui_element.location_ratio.y);
        fb_gui_elements.push_back(SquareBox::DSUGUIV1::CreateGUIElement(builder,
            static_cast<int>(focus_gui_element.shape),
            static_cast<int>(focus_gui_element.state),
            focus_gui_element.id,
            builder.CreateString(std::string(focus_gui_element.name)),
            focus_gui_element.is_hidden,
            focus_gui_element.is_locked,
            &temp_vec2,
            focus_gui_element.height_ratio,
            focus_gui_element.width_ratio,
            focus_gui_element.radius_ratio,
            focus_gui_element.angle,
            focus_gui_element.is_alive,
            builder.CreateVector(fb_gui_element_fonts),
            builder.CreateVector(fb_gui_element_textures)
            ));
    }

    std::vector<flatbuffers::Offset<SquareBox::DSUGUIV1::GLTexture>> fb_single_textures;
    for (unsigned int i = 0; i < gui_layer_.single_textures.size(); i++)
    {
        const SquareBox::AssetManager::GLTexture& current_single_texture = gui_layer_.single_textures[i];
        auto temp_vec2 = SquareBox::DSUGUIV1::Ivec2(current_single_texture.tiling.x, current_single_texture.tiling.y);
        fb_single_textures.push_back(SquareBox::DSUGUIV1::CreateGLTexture(builder,
            builder.CreateString(current_single_texture.asset_file_path),
            builder.CreateString(current_single_texture.display_name),
            &temp_vec2
            ));
    }

    std::vector<flatbuffers::Offset<SquareBox::DSUGUIV1::GLTexture>> fb_tiled_textures;
    for (unsigned int i = 0; i < gui_layer_.tiled_textures.size(); i++)
    {
        const SquareBox::AssetManager::GLTexture& current_tiled_texture = gui_layer_.tiled_textures[i];
        auto temp_vec2 = SquareBox::DSUGUIV1::Ivec2(current_tiled_texture.tiling.x, current_tiled_texture.tiling.y);
        fb_single_textures.push_back(SquareBox::DSUGUIV1::CreateGLTexture(builder,
            builder.CreateString(current_tiled_texture.asset_file_path),
            builder.CreateString(current_tiled_texture.display_name),
            &temp_vec2
        ));
    }

    std::vector<flatbuffers::Offset<SquareBox::DSUGUIV1::SpriteFont>> fb_sprite_fonts;
    for (unsigned int i = 0; i < gui_layer_.sprite_fonts.size(); i++)
    {
        const SquareBox::RenderEngine::SpriteFont& focus_sprite_font = gui_layer_.sprite_fonts[i];
        fb_sprite_fonts.push_back(SquareBox::DSUGUIV1::CreateSpriteFont(builder,
            builder.CreateString(focus_sprite_font.getFontFilePath()),
            builder.CreateString(focus_sprite_font.getDisplayName()),
            focus_sprite_font.getFontSize()
            ));
    }
    SquareBox::DSUGUIV1::Vec2 fb_editing_screen_dimensions(gui_layer_.editing_screen_dimensions.x, gui_layer_.editing_screen_dimensions.y);

    auto complete_layer=SquareBox::DSUGUIV1::CreateSquareBoxGuiLayer(builder, builder.CreateVector(fb_gui_elements), builder.CreateVector(fb_single_textures), builder.CreateVector(fb_tiled_textures), builder.CreateVector(fb_sprite_fonts),&fb_editing_screen_dimensions);

    builder.Finish(complete_layer);

    uint8_t* buf = builder.GetCurrentBufferPointer();
    int size = builder.GetSize();

    return SquareBox::AssetManager::IOManager::saveRawDataToFile((const char*)buf, size, file_path_);
    return true;
}

bool SquareBox::GWOM::GuiReaderWriter::loadGuiLayerDataAsBinary(const std::string& file_path_, SquareBox::GWOM::GuiLayer& gui_layer_)
{
    std::pair<char*, int> fileInfo = SquareBox::AssetManager::IOManager::getRawDataFromFile(file_path_);
    //construct a m_buffer object that knows its size
    char* dataBuffer = new char[fileInfo.second];
    dataBuffer = fileInfo.first;

    auto complete_layer = SquareBox::DSUGUIV1::GetSquareBoxGuiLayer(dataBuffer);
    gui_layer_.gui_elements.clear();

    auto fb_gui_elements = complete_layer->gui_elements();

    for (unsigned int i = 0; i < fb_gui_elements->size(); i++)
    {
        auto fb_gui_element = fb_gui_elements->Get(i);
        SquareBox::GWOM::GUIElement gui_element;

        gui_element.shape = static_cast<GUIElementShapeEnum>(fb_gui_element->shape());
        gui_element.state = static_cast<GUIElementStateEnum>(fb_gui_element->state());
        gui_element.id = fb_gui_element->id();

        auto fb_name = fb_gui_element->name()->str();
        for (unsigned int j = 0; j < fb_name.length(); j++)
        {
            gui_element.name[j] = fb_name[j];
        }
        gui_element.name[fb_name.length()] = '\0';

        gui_element.is_hidden = fb_gui_element->is_hidden();
        gui_element.is_locked = fb_gui_element->is_locked();
        gui_element.location_ratio.x = fb_gui_element->location_ratio()->x();
        gui_element.location_ratio.y = fb_gui_element->location_ratio()->y();
        gui_element.height_ratio = fb_gui_element->height_ratio();
        gui_element.width_ratio = fb_gui_element->width_ratio();
        gui_element.radius_ratio = fb_gui_element->radius_ratio();
        gui_element.angle = fb_gui_element->angle();
        gui_element.is_alive = fb_gui_element->is_alive();

        gui_element.fonts.clear();
        for (unsigned int j = 0; j < fb_gui_element->fonts()->size(); j++)
        {
            auto fb_font = fb_gui_element->fonts()->Get(j);
            SquareBox::GWOM::FontInfo font_info;

            font_info.color = glm::ivec4(fb_font->color()->x(), fb_font->color()->y(), fb_font->color()->z(), fb_font->color()->w());
            font_info.font_index = fb_font->font_index();

            auto fb_font_text = fb_font->text()->str();
            for (unsigned int k = 0; k < fb_font_text.length(); k++)
            {
                font_info.text[k] = fb_font_text[k];
            }
            font_info.text[fb_font_text.length()] = '\0';

            font_info.show_text = fb_font->show_text();
            font_info.text_to_box_height_scale = fb_font->text_to_box_height_scale();
            font_info.opacity = fb_font->opacity();
            font_info.justification = static_cast<SquareBox::JustificationEnum>(fb_font->justification());

            gui_element.fonts.push_back(font_info);
        }

        gui_element.textures.clear();
        for (unsigned int j = 0; j < fb_gui_element->textures()->size(); j++)
        {
            auto fb_texture = fb_gui_element->textures()->Get(j);
            SquareBox::GWOM::TextureInfo texture_info;
            texture_info.texture_type = static_cast<SquareBox::TextureEnum>(fb_texture->texture_type());
            texture_info.texture_index = fb_texture->texture_index();
            texture_info.tile_sheet_index = fb_texture->tile_sheet_index();
            texture_info.texture_id = fb_texture->texture_id();
            texture_info.uv_rect = glm::vec4(fb_texture->uv_rect()->x(), fb_texture->uv_rect()->y(), fb_texture->uv_rect()->z(), fb_texture->uv_rect()->w());
            texture_info.color = glm::ivec4(fb_texture->color()->x(), fb_texture->color()->y(), fb_texture->color()->z(), fb_texture->color()->w());
            texture_info.opacity = fb_texture->opacity();
            gui_element.textures.push_back(texture_info);
        }

        gui_layer_.gui_elements.push_back(gui_element);
    }
        gui_layer_.single_textures.clear();

        auto fb_single_textures = complete_layer->single_textures();

        for (unsigned int i = 0; i < fb_single_textures->size(); i++)
        {
            auto fb_single_texture = fb_single_textures->Get(i);
            SquareBox::AssetManager::GLTexture single_texture;
            single_texture.asset_file_path = fb_single_texture->assets_file_path()->str();
            single_texture.display_name = fb_single_texture->display_name()->str();
            single_texture.tiling.x = fb_single_texture->tiling()->x();
            single_texture.tiling.y = fb_single_texture->tiling()->y();
            gui_layer_.single_textures.push_back(single_texture);
        }

        gui_layer_.tiled_textures.clear();
        auto fb_tiled_textures = complete_layer->tiled_textures();

        for (unsigned int i = 0; i < fb_tiled_textures->size(); i++)
        {
            auto fb_tiled_texture = fb_tiled_textures->Get(i);
            SquareBox::AssetManager::GLTexture tiled_texture;
            tiled_texture.asset_file_path = fb_tiled_texture->assets_file_path()->str();
            tiled_texture.display_name = fb_tiled_texture->display_name()->str();
            tiled_texture.tiling.x = fb_tiled_texture->tiling()->x();
            tiled_texture.tiling.y = fb_tiled_texture->tiling()->y();
            gui_layer_.single_textures.push_back(tiled_texture);
        }

        gui_layer_.sprite_fonts.clear();
        auto fb_sprite_fonts = complete_layer->sprite_fonts();

        for (unsigned int i = 0; i < fb_sprite_fonts->size(); i++)
        {
            auto fb_sprite_font = fb_sprite_fonts->Get(i);
            SquareBox::RenderEngine::SpriteFont sprite_font;
            sprite_font.setProperties(fb_sprite_font->font_path()->str(),fb_sprite_font->display_name()->str(),fb_sprite_font->font_size());
            gui_layer_.sprite_fonts.push_back(sprite_font);
        }

        auto fb_editing_screen_dimensions = complete_layer->editing_screen_dimensions();
        gui_layer_.editing_screen_dimensions.x = fb_editing_screen_dimensions->x();
        gui_layer_.editing_screen_dimensions.y = fb_editing_screen_dimensions->y();
    delete[] dataBuffer;
    return true;
}
