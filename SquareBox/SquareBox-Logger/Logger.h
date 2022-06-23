#pragma once
#include <memory>
#include <iostream>

// This ignores all warnings raised inside External headers
//#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
//#pragma warning(pop)

namespace SquareBox {
	class  Log
	{
	public:
		static void Init(std::string log_file_name_,bool write_to_log_file_);
#ifdef SQB_PLATFORM_ANDROID
		//we can add other logger with time
		static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return android_logger; }
		static std::shared_ptr<spdlog::logger>& GetClientLogger() { return android_logger; }
	private:
		static std::shared_ptr<spdlog::logger> android_logger;
#else
		static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;

#endif
	public:
		static void GLClearError();
		static void SDLClearError();
		static bool GLLogCall(const char* function, const char* file, int line);
		static bool SDLLogCall(const char* function, const char* file, int line);
		static std::vector<std::string> latest_logs;
	};
}

//GL Error Checking macro
#define ASSERT(x) if(!(x)) (void)0
#define SBX_GLCall(x) SquareBox::Log::GLClearError();\
	x;\
	ASSERT(SquareBox::Log::GLLogCall(#x,__FILE__,__LINE__))

#define SBX_SDLCall(x) SquareBox::Log::SDLClearError();\
	x;\
	ASSERT(SquareBox::Log::SDLLogCall(#x,__FILE__,__LINE__))


#define SBX_SDLNetCall(x) SquareBox::Log::SDLNetClearError();\
	x;\
	ASSERT(SquareBox::Log::SDLNetLogCall(#x,__FILE__,__LINE__))
/* edit this macro so that it also displays where the error is coming from exactly, eg the file
this failed
	#define SBX_CORE_CRITICAL(...) ::SquareBox::Log::GetCoreLogger()->critical(__VA_ARGS__,__FILE__,__LINE__)
	#define SBX_CORE_CRITICAL(x) ::SquareBox::Log::GetCoreLogger()->critical(#x,__FILE__,__LINE__)

*/
// Core log macros
#define SBX_CORE_TRACE(...)    ::SquareBox::Log::GetCoreLogger()->trace(__VA_ARGS__,__FILE__,__LINE__)
#define SBX_CORE_INFO(...)     ::SquareBox::Log::GetCoreLogger()->info(__VA_ARGS__,__FILE__,__LINE__)
#define SBX_CORE_WARN(...)     ::SquareBox::Log::GetCoreLogger()->warn(__VA_ARGS__,__FILE__,__LINE__)
#define SBX_CORE_ERROR(...)    ::SquareBox::Log::GetCoreLogger()->error(__VA_ARGS__,__FILE__,__LINE__)
#ifdef SQB_PLATFORM_ANDROID
#define SBX_CORE_CRITICAL(...) ::SquareBox::Log::GetCoreLogger()->error(__VA_ARGS__)
#else
//Android has no critical debug flag
#define SBX_CORE_CRITICAL(...) ::SquareBox::Log::GetCoreLogger()->critical(__VA_ARGS__)
#endif // DEBUG

// Client log macros
#define SBX_TRACE(...)         ::SquareBox::Log::GetClientLogger()->trace(__VA_ARGS__)
#define SBX_INFO(...)          ::SquareBox::Log::GetClientLogger()->info(__VA_ARGS__)
#define SBX_WARN(...)          ::SquareBox::Log::GetClientLogger()->warn(__VA_ARGS__)
#define SBX_ERROR(...)         ::SquareBox::Log::GetClientLogger()->error(__VA_ARGS__)

#ifdef SQB_PLATFORM_ANDROID
#define SBX_CRITICAL(...) ::SquareBox::Log::GetCoreLogger()->error(__VA_ARGS__)
#else
//Android has no critical debug flag
#define SBX_CRITICAL(...) ::SquareBox::Log::GetCoreLogger()->critical(__VA_ARGS__)
#endif // DEBUG

//// Core log macros
//#define SBX_CORE_TRACE(...)    (void)0
//#define SBX_CORE_INFO(...)     (void)0
//#define SBX_CORE_WARN(...)     (void)0
//#define SBX_CORE_ERROR(...)    (void)0
//#define SBX_CORE_CRITICAL(...) (void)0
//
//// Client log macros
//#define SBX_TRACE(...)         (void)0
//#define SBX_INFO(...)          (void)0
//#define SBX_WARN(...)          (void)0
//#define SBX_ERROR(...)         (void)0
//#define SBX_CRITICAL(...)      (void)0
//#endif
//
