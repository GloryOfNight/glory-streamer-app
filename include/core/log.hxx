#pragma once
#include <chrono>
#include <cstdarg>
#include <format>
#include <iostream>

namespace gl::app::logging
{
	enum class log_level : uint8_t
	{
		NoLogs,
		Verbose,
		Display,
		Warning,
		Error
	};

	static log_level logLevel = log_level::Display;

	static std::string log_level_to_string(const log_level level)
	{
		switch (level)
		{
		case log_level::Error:
			return "Error";
		case log_level::Warning:
			return "Warning";
		case log_level::Display:
			return "Display";
		case log_level::Verbose:
			return "Verbose";
		default:
			return "Unknown";
		}
	}

	template <typename... Args>
	void log(const log_level level, const std::string_view format, Args... args)
	{
		if (level <= log_level::NoLogs || level < logLevel)
			return;

		std::ostream& ostream = level == log_level::Error ? std::cerr : std::cout;

		const auto now = std::chrono::utc_clock::now();
		const auto log_level_str = log_level_to_string(level);

		ostream << std::vformat("[{0:%F}T{0:%T}] {1}: ", std::make_format_args(now, log_level_str)) << std::vformat(format, std::make_format_args(args...)) << std::endl;
	}
} // namespace gl::app::logging

#define LOG(level, format, ...) gl::app::logging::log(gl::app::logging::log_level::level, format, ##__VA_ARGS__);