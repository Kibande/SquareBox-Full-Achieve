#include "Logger.h"
#include <spdlog/sinks/android_sink.h>
#include <spdlog/sinks/basic_file_sink.h>
#include "spdlog/sinks/stdout_color_sinks.h"
#include <SDL/include/SDL.h>
#ifdef SQB_PLATFORM_ANDROID
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#else
#include<GL/glew.h>
#endif

//a wrapper to our game engine
namespace SquareBox {
#ifdef SQB_PLATFORM_ANDROID
	std::shared_ptr<spdlog::logger> Log::android_logger;
#else
	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Log::s_ClientLogger;
#endif

	void Log::Init(std::string log_file_name_, bool write_to_log_file_) {
#ifdef SQB_PLATFORM_ANDROID
		std::vector<spdlog::sink_ptr> logSinks;
		logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		logSinks[0]->set_pattern("%^[%T] %n: %v%$");

		std::string tag = "spdlog-android";
		android_logger = spdlog::android_logger_mt("android", tag);
#else
		std::vector<spdlog::sink_ptr> logSinks;
		logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_file_name_, write_to_log_file_));

		logSinks[0]->set_pattern("%^[%T] %n: %v%$");
		logSinks[1]->set_pattern("[%T] [%l] %n: %v");

		s_CoreLogger = std::make_shared<spdlog::logger>("SQUAREBOX", begin(logSinks), end(logSinks));
		spdlog::register_logger(s_CoreLogger);
		s_CoreLogger->set_level(spdlog::level::trace);
		s_CoreLogger->flush_on(spdlog::level::trace);

		s_ClientLogger = std::make_shared<spdlog::logger>("APP", begin(logSinks), end(logSinks));
		spdlog::register_logger(s_ClientLogger);
		s_ClientLogger->set_level(spdlog::level::trace);
		s_ClientLogger->flush_on(spdlog::level::trace);
#endif
	}
	void Log::GLClearError()
	{
		//loop through,while emptying  till error buffer is  empty
		while (glGetError() != GL_NO_ERROR);
	}

	void Log::SDLClearError()
	{
		SDL_ClearError();
	}

	bool Log::GLLogCall(const char * function, const char * file, int line)
	{
		//this function will print out all the errors that we get
		while (GLenum error = glGetError())
		{
			SBX_CORE_ERROR("OpenGl Error {} : function : {} ,file : {} line : {} ", error, function, file, line);
			return false;
		}
		return true;
	}
	bool Log::SDLLogCall(const char * function, const char * file, int line)
	{
		
		/* 
			for sdl only a single error can be retirved , the most recent one.
			some times success messages also return errors
		*/
		const char *error = SDL_GetError();
		if (*error != '\0')
		{
			SBX_CORE_ERROR("SDL Error {} : function : {} ,file : {} line : {} ", std::string(error), function, file, line);
		}
		
		return true;
	}

}