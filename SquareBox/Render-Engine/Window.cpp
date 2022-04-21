#include "Window.h"

#include <SquareBox-Logger/Logger.h>

namespace SquareBox {
	namespace RenderEngine {
		Window::Window() : m_screen_width(600), m_screen_height(400)
		{
		}

		Window::~Window()
		{
		}

		int Window::create(std::string windowName, int screenWidth, int screenHeight, unsigned int currentflags, ColorRGBA8 initColor /* = ColorRGBA8(255,255,255,255)*/)
		{
			m_screen_width = screenWidth;
			m_screen_height = screenHeight;
			int window_pos_x = SDL_WINDOWPOS_CENTERED;
			int window_pos_y = SDL_WINDOWPOS_CENTERED;

			SDL_DisplayMode mode;
			SDL_GetDisplayMode(0, 0, &mode);
			m_max_max_screen_width = mode.w;
			m_max_max_screen_height = mode.h;
			Uint32 flags = SDL_WINDOW_OPENGL;//Every one of our windows should use an openglContext
			flags |= currentflags;

			if (flags & SquareBox::RenderEngine::WindowFlags::WINDOW_FULLSCREEN)
			{

				flags = excludeTargetWindowFlag(flags, SquareBox::RenderEngine::WindowFlags::WINDOW_FULLSCREEN);
				SDL_Rect usable_display_dimensions;
				SDL_GetDisplayUsableBounds(0, &usable_display_dimensions);
				m_screen_width = usable_display_dimensions.w;
				m_screen_height = usable_display_dimensions.h;
				window_pos_x = usable_display_dimensions.x;
				window_pos_y = usable_display_dimensions.y;

				SBX_CORE_INFO("Default Screen : X - {} Y - {}  Width - {} , Height - {}", window_pos_x, window_pos_y, m_max_max_screen_width, m_max_max_screen_height);
				SBX_CORE_INFO("Usable Screen : X - {} Y - {}  Width - {} , Height - {}", window_pos_x, window_pos_y, m_screen_width, m_screen_height);
			}
			m_original_screen_width	 = m_screen_width;
			m_original_screen_height = m_screen_height;
			m_sdl_window = SDL_CreateWindow(windowName.c_str(), window_pos_x, window_pos_y, m_screen_width, m_screen_height, flags);

			//Error checking

			if (m_sdl_window == nullptr) {
				SBX_CORE_CRITICAL("SDL Window could not be created!");
			}
			//get the windows Dpi infor
			if (SDL_GetDisplayDPI(0, &m_ddpi, &m_hdpi, &m_vdpi) != 0) {
				SBX_CORE_ERROR("Failed to get DPI info");
			}
			//create context and give it to the window after
			SDL_GLContext gl_context = SDL_GL_CreateContext(m_sdl_window);
			m_gl_context_ptr = &gl_context;
			SDL_GL_MakeCurrent(m_sdl_window, gl_context);
			SDL_GL_SetSwapInterval(1); // Enable vsync

			if (gl_context == nullptr) {
				SBX_CORE_CRITICAL("SDL_GL opengl context  could not be created!");
			}

#ifndef SQB_PLATFORM_ANDROID
			//initializing glew  ,which sets up extra things for us
			GLenum error = glewInit();
			if (error != GLEW_OK) {
				SBX_CORE_CRITICAL("Could not initialize GLEW");
			}
#endif // !__FOR_ANDROID

			//print OpenGl Version
			SBX_CORE_INFO("***** openGL version {} **** \n", glGetString(GL_VERSION));

			//Sets the initial background color for our window

			//set VSYNC //This prevents screen tiring
			SDL_GL_SetSwapInterval(0);
			//enable alpha blending
			SBX_GLCall(glEnable(GL_BLEND));
			//specify which type of blending we would like to do
			SBX_GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

			m_background_color = initColor;
			if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
				SBX_CORE_ERROR("Failed to set SDL_HINT_RENDER_SCALE_QUALITY");
			}
			return 0;
		}

		void Window::setWindowIcon(unsigned char* pixelBuffer_, unsigned int width_, unsigned int height_)
		{
			Uint32 rmask = 0x000000ff;
			Uint32 gmask = 0x0000ff00;
			Uint32 bmask = 0x00ff0000;
			Uint32 amask = 0xff000000;
			SDL_Surface* icon_surface = SDL_CreateRGBSurfaceFrom(pixelBuffer_, width_, height_, 32, width_ * 4, rmask, gmask, bmask, amask);

			if (icon_surface == NULL) {
				SBX_CORE_ERROR("Failed to create surface for Icon");
			}
			else {
				SDL_SetWindowIcon(m_sdl_window, icon_surface);
				SDL_FreeSurface(icon_surface);
			}

		}

		void Window::setWindowOpacity(float opacity_)
		{
			m_window_opactiy = opacity_;
			SDL_SetWindowOpacity(m_sdl_window, m_window_opactiy);
		}

		void Window::useAdditiveBlending()
		{
			//Additive blending
			SBX_GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE));
		}

		void Window::useAlphaBlending()
		{
			//Reset to regular alpha blending
			SBX_GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
			/*
			Parameter 1 -> Source factor (GL_SRC_ALPHA)
			if source alpha is one ,multiply source color by one

			Parameter 2 -> Destination factor (GL_ONE_MINUS_SRC_ALPHA)
			GL_ONE_MINUS_SRC_ALPHA = 1-GL_SRC_ALPHA * Destination color
			*/
		}

		void Window::swapBuffer()
		{
			//rendering code
			SDL_GL_SwapWindow(m_sdl_window);//swap the buffer and drawa everything onto the screen
		}

		void Window::resizeWindow(int screenWidth, int screenHeight)
		{
			//Window must first be resize able
			SDL_SetWindowSize(m_sdl_window, screenWidth, screenHeight);
			glViewport(0, 0, screenWidth, screenHeight);
		}

		void Window::resizable(bool isWindowResizable)
		{
			SDL_bool resizeBool = SDL_bool::SDL_TRUE;

			if (isWindowResizable)
			{
				resizeBool = SDL_bool::SDL_TRUE;
			}
			else {
				resizeBool = SDL_bool::SDL_FALSE;
			}

			SDL_SetWindowResizable(m_sdl_window, resizeBool);
		}

		void Window::update()
		{

			//refresh the windows back ground Color
			SBX_GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
			float newr = (m_background_color.r / 255.0f);
			float newg = (m_background_color.g / 255.0f);
			float newb = (m_background_color.b / 255.0f);
			float newa = (m_background_color.a / 255.0f);
			SBX_GLCall(glClearColor(newr, newg, newb, newa));

			//wrong dimensions after resize
			//SDL_GetRendererOutputSize(_sdlRenderer, &m_screen_width, &m_screen_height);//update our private dimensions varialbles

			//Correct
			SDL_GL_GetDrawableSize(m_sdl_window, &m_screen_width, &m_screen_height);
			//SDL_GetWindowSize(m_sdl_window, &m_screen_width, &m_screen_height);//update our private dimensions variables
			SBX_GLCall(glViewport(0, 0, m_screen_width, m_screen_height));
			//attempting to solve the issue of wrong input device coordinates after resize
			//didnt work and cause crush on android
			//SDL_RenderSetLogicalSize(SDL_GetRenderer(m_sdl_window), m_screen_width, m_screen_height);

		}

		int Window::excludeTargetWindowFlag(const int collection_,const WindowFlags target_flag_)
		{
			//first check wether we even the have target flag as part of the collection
			if (collection_ & target_flag_) {
				int cleaned_up_flags=0;
			

				if ((SquareBox::RenderEngine::WindowFlags::WINDOW_FULLSCREEN != target_flag_) && (collection_ & SquareBox::RenderEngine::WindowFlags::WINDOW_FULLSCREEN)) {
					cleaned_up_flags |= SquareBox::RenderEngine::WindowFlags::WINDOW_FULLSCREEN;
				}


				if ((SquareBox::RenderEngine::WindowFlags::WINDOW_SHOWN != target_flag_) && (collection_ & SquareBox::RenderEngine::WindowFlags::WINDOW_SHOWN)) {
					cleaned_up_flags |= SquareBox::RenderEngine::WindowFlags::WINDOW_SHOWN;
				}


				if ((SquareBox::RenderEngine::WindowFlags::WINDOW_HIDDEN != target_flag_) && (collection_ & SquareBox::RenderEngine::WindowFlags::WINDOW_HIDDEN)) {
					cleaned_up_flags |= SquareBox::RenderEngine::WindowFlags::WINDOW_HIDDEN;
				}


				if ((SquareBox::RenderEngine::WindowFlags::WINDOW_BORDERLESS != target_flag_) && (collection_ & SquareBox::RenderEngine::WindowFlags::WINDOW_BORDERLESS)) {
					cleaned_up_flags |= SquareBox::RenderEngine::WindowFlags::WINDOW_BORDERLESS;
				}


				if ((SquareBox::RenderEngine::WindowFlags::WINDOW_RESIZABLE != target_flag_) && (collection_ & SquareBox::RenderEngine::WindowFlags::WINDOW_RESIZABLE)) {
					cleaned_up_flags |= SquareBox::RenderEngine::WindowFlags::WINDOW_RESIZABLE;
				}


				if ((SquareBox::RenderEngine::WindowFlags::WINDOW_MINIMIZED != target_flag_) && (collection_ & SquareBox::RenderEngine::WindowFlags::WINDOW_MINIMIZED)) {
					cleaned_up_flags |= SquareBox::RenderEngine::WindowFlags::WINDOW_MINIMIZED;
				}


				if ((SquareBox::RenderEngine::WindowFlags::WINDOW_MAXIMIZED != target_flag_) && (collection_ & SquareBox::RenderEngine::WindowFlags::WINDOW_MAXIMIZED)) {
					cleaned_up_flags |= SquareBox::RenderEngine::WindowFlags::WINDOW_MAXIMIZED;
				}

				/*
				 this if block always returns true and kills all our hard work
				if ((SquareBox::RenderEngine::WindowFlags::WINDOW_FULLSCREEN_DESKTOP != target_flag_) && (collection_ & SquareBox::RenderEngine::WindowFlags::WINDOW_FULLSCREEN_DESKTOP)) {
					cleaned_up_flags |= SquareBox::RenderEngine::WindowFlags::WINDOW_FULLSCREEN_DESKTOP;
				}
				*/

				if ((SquareBox::RenderEngine::WindowFlags::WINDOW_ALLOW_HIGHDPI != target_flag_) && (collection_ & SquareBox::RenderEngine::WindowFlags::WINDOW_ALLOW_HIGHDPI)) {
					cleaned_up_flags |= SquareBox::RenderEngine::WindowFlags::WINDOW_ALLOW_HIGHDPI;
				}


				if ((SquareBox::RenderEngine::WindowFlags::WINDOW_ALWAYS_ON_TOP != target_flag_) && (collection_ & SquareBox::RenderEngine::WindowFlags::WINDOW_ALWAYS_ON_TOP)) {
					cleaned_up_flags |= SquareBox::RenderEngine::WindowFlags::WINDOW_ALWAYS_ON_TOP;
				}


				if ((SquareBox::RenderEngine::WindowFlags::WINDOW_SKIP_TASKBAR != target_flag_) && (collection_ & SquareBox::RenderEngine::WindowFlags::WINDOW_SKIP_TASKBAR)) {
					cleaned_up_flags |= SquareBox::RenderEngine::WindowFlags::WINDOW_SKIP_TASKBAR;
				}


				if ((SquareBox::RenderEngine::WindowFlags::WINDOW_UTITLITY != target_flag_) && (collection_ & SquareBox::RenderEngine::WindowFlags::WINDOW_UTITLITY)) {
					cleaned_up_flags |= SquareBox::RenderEngine::WindowFlags::WINDOW_UTITLITY;
				}


				if ((SquareBox::RenderEngine::WindowFlags::WINDOW_TOOLTIP != target_flag_) && (collection_ & SquareBox::RenderEngine::WindowFlags::WINDOW_TOOLTIP)) {
					cleaned_up_flags |= SquareBox::RenderEngine::WindowFlags::WINDOW_TOOLTIP;
				}


				if ((SquareBox::RenderEngine::WindowFlags::WINDOW_POPUP_MENU != target_flag_) && (collection_ & SquareBox::RenderEngine::WindowFlags::WINDOW_POPUP_MENU)) {
					cleaned_up_flags |= SquareBox::RenderEngine::WindowFlags::WINDOW_POPUP_MENU;
				}
				cleaned_up_flags |= SDL_WINDOW_OPENGL;
				return cleaned_up_flags;
			}
			else {
				return collection_;
			}
		}

		void Window::destory()
		{
			SDL_Quit();
		}
	}
}