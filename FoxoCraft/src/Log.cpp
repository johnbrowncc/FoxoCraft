#include "Log.h"

#include <iomanip>
#include <ctime>
#include <sstream>

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/dist_sink.h>


namespace FoxoEngine
{
	std::shared_ptr<spdlog::logger> logger;

	void CreateLogger()
	{
		auto dist_sink = std::make_shared<spdlog::sinks::dist_sink_mt>();
		auto sink1 = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		

		dist_sink->add_sink(sink1);
		

		logger = std::make_shared<spdlog::logger>("FoxoEngine", dist_sink);
		logger->flush_on(spdlog::level::trace);
		logger->set_level(spdlog::level::trace);
	}

	void DestroyLogger()
	{
		logger->flush();
		spdlog::drop_all();
		spdlog::shutdown();
	}
}