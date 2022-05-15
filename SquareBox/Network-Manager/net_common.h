
#pragma once
#ifdef SQB_PLATFORM_ANDROID
#include <experimental/optional>
#else
#include <optional>
#endif // SQB_PLATFORM_ANDROID

#include <memory>
#include <thread>
#include <mutex>
#include <deque>
#include <condition_variable>
#include <exception>

#include <vector>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstddef>
#include <system_error>
#include <string>
#include <type_traits>

#ifndef SQB_PLATFORM_ANDROID
    // this is a windows only problem
	// but if excluded , it defaults to something
	//#ifdef _WIN32
	//#define _WIN32_WINNT 0x0A00
	//#endif
#endif

#define ASIO_STANDALONE
#define ASIO_HAS_STD_ADDRESSOF
#define ASIO_HAS_STD_ARRAY
#define ASIO_HAS_CSTDINT
#define ASIO_HAS_STD_SHARED_PTR
#define ASIO_HAS_STD_TYPE_TRAITS
#define ASIO_DISABLE_NOEXCEPT
#define ASIO_ERROR_CATEGORY_NOEXCEPT noexcept(true)


#include <asio/asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>


