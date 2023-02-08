#pragma once

#ifdef SQB_PLATFORM_ANDROID
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#else
#include<GL/glew.h>
#endif

#include <SDL/include/SDL.h>
#include<string>
#include "ColorRGBA8.h"
#include <SquareBox-Logger/Logger.h>
namespace SquareBox {
	namespace RenderEngine {
		enum  WindowFlags {
			//Removed because it crashed program when two monitors were ding used WINDOW_FULLSCREEN = SDL_WINDOW_FULLSCREEN,         /**< fullscreen window  but custom edited to level the taskbar area unoccupied*/
			WINDOW_SHOWN = SDL_WINDOW_SHOWN,              /**< window is visible */
			WINDOW_HIDDEN = SDL_WINDOW_HIDDEN,             /**< window is not visible */
			WINDOW_BORDERLESS = SDL_WINDOW_BORDERLESS,         /**< no window decoration */
			WINDOW_RESIZABLE = SDL_WINDOW_RESIZABLE,          /**< window can be resized */
			WINDOW_MINIMIZED = SDL_WINDOW_MINIMIZED,          /**< window is minimized */
			WINDOW_MAXIMIZED = SDL_WINDOW_MAXIMIZED,          /**< window is maximized */
			WINDOW_FULLSCREEN_DESKTOP = SDL_WINDOW_FULLSCREEN_DESKTOP,
			WINDOW_ALLOW_HIGHDPI = SDL_WINDOW_ALLOW_HIGHDPI,      /**< window should be created in high-DPI mode if supported.
															 On macOS NSHighResolutionCapable must be set true in the
															 application's Info.plist for this to have any effect. */
															 WINDOW_ALWAYS_ON_TOP = SDL_WINDOW_ALWAYS_ON_TOP,      /**< window should always be above others */
															 WINDOW_SKIP_TASKBAR = SDL_WINDOW_SKIP_TASKBAR,      /**< window should not be added to the task bar */
															 WINDOW_UTITLITY = SDL_WINDOW_UTILITY,      /**< window should be treated as a utility window */
															 WINDOW_TOOLTIP = SDL_WINDOW_TOOLTIP,      /**< window should be treated as a tooltip */
															 WINDOW_POPUP_MENU = SDL_WINDOW_POPUP_MENU      /**< window should be treated as a pop up menu */
		};

		class Window
		{
		public:
			Window();
			~Window();

			int create(std::string windowName, int screenWidth, int screenHeight, unsigned int currentflags, ColorRGBA8 initColor = ColorRGBA8(255, 255, 255, 255));
			//getters
			int getMaxMaxScreenWidth() const { return m_max_max_screen_width; }
			int getMaxMaxScreenHeight() const { return m_max_max_screen_height; }
			int getScreenWidth() const { return m_screen_width; }
			int getScreenHeight() const { return  m_screen_height; }
			float getDDPI()const { return m_ddpi; }
			float getHDPI()const { return m_hdpi; }
			float getVDPI()const { return m_vdpi; }

			float getWindowOpactity() const { return m_window_opactiy; }
			void setWindowIcon(unsigned char* pixelBuffer_, unsigned int width_, unsigned int height_);
			void setWindowOpacity(float opacity_);
			void useAdditiveBlending();
			void useAlphaBlending();
			ColorRGBA8 getBackGroundColor() const { return m_background_color; };
			void setBackGroundColor(ColorRGBA8 newBackGroundColor_) { m_background_color = newBackGroundColor_; };
			//setters
			void swapBuffer();
			void resizeWindow(int windowWidth, int windowHeight);
			void resizable(bool isWindowResizable);
			void update();
			void destory();


			//this is a big violations of our rules , but we had nothing to do for now
			//don't use unless giving directly to imGUI, cos this will be removed
			SDL_GLContext* getGLContextPointerForImGui() const { return m_gl_context_ptr; }

			//this is a big violations of our rules , but we had nothing to do for now
			//don't use unless giving directly to imGUI, cos this will be removed
			SDL_Window* getWindowPointerForImGui() const { return m_sdl_window; }


		private:
			int excludeTargetWindowFlag(const int collection_,const WindowFlags target_flag_);
			ColorRGBA8 m_background_color;
			int m_screen_width;
			int m_screen_height;
			int m_original_screen_width;
			int m_original_screen_height;
			int m_max_max_screen_width;
			int m_max_max_screen_height;
			SDL_GLContext* m_gl_context_ptr = nullptr;
			SDL_Window* m_sdl_window = nullptr;
			float m_window_opactiy = 1.0f;
			float m_ddpi;
			float m_hdpi;
			float m_vdpi;
		};
	}
}