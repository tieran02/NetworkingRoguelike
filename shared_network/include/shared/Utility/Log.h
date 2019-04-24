#pragma once
#include  "spdlog/spdlog.h"

class Log
{
public:

	static void Init();

	inline static std::shared_ptr<spdlog::logger>& GetLogger()
	{
		return m_logger;
	}
private:
	static std::shared_ptr<spdlog::logger> m_logger;
};

#ifdef DEBUG
	#define LOG_TRACE(...) ::Log::GetLogger()->trace(__VA_ARGS__)
#else
	#define LOG_TRACE(...)
#endif

#define LOG_INFO(...) ::Log::GetLogger()->info(__VA_ARGS__)
#define LOG_WARN(...) ::Log::GetLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...) ::Log::GetLogger()->error(__VA_ARGS__)
#define LOG_FATAL(...) ::Log::GetLogger()->critical(__VA_ARGS__)