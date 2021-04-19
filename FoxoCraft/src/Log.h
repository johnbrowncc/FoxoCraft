#pragma once

#include <spdlog/spdlog.h>

#define FC_LOG_TRACE(...) ::spdlog::trace(__VA_ARGS__)
#define FC_LOG_DEBUG(...) ::spdlog::debug(__VA_ARGS__)
#define FC_LOG_INFO(...) ::spdlog::info(__VA_ARGS__)
#define FC_LOG_WARN(...) ::spdlog::warn(__VA_ARGS__)
#define FC_LOG_ERROR(...) ::spdlog::error(__VA_ARGS__)
#define FC_LOG_CRITICAL(...) ::spdlog::critical(__VA_ARGS__)