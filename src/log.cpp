#include "headers/log.h"
#include <spdlog/sinks/basic_file_sink.h>

void SetupLog()
{
	/*
    * in case automatic path detection breaks
    std::filesystem::path path = "2342342tergsfgfdgfdgd"sv;
    auto pluginName = SKSE::PluginDeclaration::GetSingleton()->GetName();
    auto logFilePath = path / std::format("{}.log", pluginName);
    */

	auto logsFolder = SKSE::log::log_directory();
	if (!logsFolder)
		SKSE::stl::report_and_fail("SKSE log_directory not provided, logs disabled.");
	auto pluginName = SKSE::GetPluginName();
	auto logFilePath = *logsFolder / std::format("{}.log", pluginName);
	auto fileLoggerPtr = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFilePath.string(), true);
	auto loggerPtr = std::make_shared<spdlog::logger>("log", std::move(fileLoggerPtr));
	spdlog::set_default_logger(std::move(loggerPtr));
	spdlog::set_level(spdlog::level::trace);
	spdlog::flush_on(spdlog::level::trace);
}