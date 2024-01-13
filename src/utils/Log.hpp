#pragma once

#include "model/level_log/LevelLogModel.hpp"
#include "model/log/LogRepository.hpp"
#include "model/tipo_log/TipoLogModel.hpp"

#include <source_location>
#include <string>

#include <fmt/format.h>

struct Log {
  static void init() {
    mRepository = jinject::inject<std::unique_ptr<LogRepository>>();
  }

  static void level(LevelLog value) {
    mLevel = value;
  }

  template <typename... Args>
  static void msg(std::source_location const &location, LevelLog level,
                  TipoLog type, std::string const &tag, std::string const &msg,
                  Args... args) {
    if (static_cast<int>(level) < static_cast<int>(mLevel)) {
      return;
    }

    LogModel model;

    model["level_log_id"] = static_cast<int>(level);
    model["tipo_log_id"] = static_cast<int>(type);
    model["localizacao"] =
        fmt::format("{} ({}:{}) {}: ", location.file_name(), location.line(),
                    location.column(), location.function_name());
    model["tag"] = tag;
    model["descricao"] = fmt::vformat(msg, fmt::make_format_args(args...));

    mRepository->save(model);
  }

private:
  inline static std::unique_ptr<LogRepository> mRepository;
  inline static LevelLog mLevel = LevelLog::Trace;
};

#define logt(...)                                                              \
  Log::msg(std::source_location::current(), LevelLog::Trace, ##__VA_ARGS__)
#define logd(...)                                                              \
  Log::msg(std::source_location::current(), LevelLog::Debug, ##__VA_ARGS__)
#define logi(...)                                                              \
  Log::msg(std::source_location::current(), LevelLog::Info, ##__VA_ARGS__)
#define logw(...)                                                              \
  Log::msg(std::source_location::current(), LevelLog::Warn, ##__VA_ARGS__)
#define loge(...)                                                              \
  Log::msg(std::source_location::current(), LevelLog::Error, ##__VA_ARGS__)
#define logf(...)                                                              \
  Log::msg(std::source_location::current(), LevelLog::Fatal, ##__VA_ARGS__)
