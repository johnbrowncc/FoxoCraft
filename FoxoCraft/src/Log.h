#pragma once

#include <memory>
#include <spdlog/spdlog.h>

namespace FoxoEngine
{
	extern std::shared_ptr<spdlog::logger> logger;

	void CreateLogger();
	void DestroyLogger();
}

#define FE_LOG_TRACE(...) ::FoxoEngine::logger->trace(__VA_ARGS__)
#define FE_LOG_DEBUG(...) ::FoxoEngine::logger->debug(__VA_ARGS__)
#define FE_LOG_INFO(...) ::FoxoEngine::logger->info(__VA_ARGS__)
#define FE_LOG_WARN(...) ::FoxoEngine::logger->warn(__VA_ARGS__)
#define FE_LOG_ERROR(...) ::FoxoEngine::logger->error(__VA_ARGS__)
#define FE_LOG_CRITICAL(...) ::FoxoEngine::logger->critical(__VA_ARGS__)