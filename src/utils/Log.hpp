#pragma once

#include "model/levelLog/LevelLogModel.hpp"
#include "model/log/LogRepository.hpp"
#include "model/tipoLog/TipoLogModel.hpp"
#include "jdb/utils/Format.hpp"

#include <source_location>
#include <string>

#include <fmt/format.h>

struct Log {
  static Log &instance() {
    static Log log;

    if (!log.mRepository) {
      log.mRepository = jinject::inject<std::unique_ptr<LogRepository> >();

      if (!log.mRepository) {
        throw std::runtime_error("Instantiation of LogRepository is incomplete");
      }
    }

    return log;
  }

  void level(LevelLog value) { mLevel = value; }

  template<typename... Args>
  void msg(std::source_location const &location, LevelLog level, TipoLog type,
           std::string const &tag, std::string const &msg, Args... args) const {
    if (static_cast<int>(level) < static_cast<int>(mLevel)) {
      return;
    }

    LogModel model;

    model["level_log_id"] = static_cast<int>(level);
    model["tipo_log_id"] = static_cast<int>(type);
    model["localizacao"] = sanitize(fmt::format("{} ({}:{}) {}: ", location.file_name(), location.line(),
                  location.column(), location.function_name()));
    model["tag"] = sanitize(tag);
    model["descricao"] = sanitize(fmt::vformat(msg, fmt::make_format_args(args...)));
    model["last"] = jdb::format_timestamp(std::chrono::system_clock::now());;

    if (auto e = mRepository->save(model); !e.has_value()) {
      throw std::move(e.error());
    }
  }

  void msg(std::source_location const &location, LevelLog level, TipoLog type,
           std::string const &tag, std::optional<std::string> msg) const {
    msg.and_then([&](std::string const &value) -> std::optional<bool> {
      this->msg(location, level, type, tag, value);

      return {};
    });
  }

private:
  std::unique_ptr<LogRepository> mRepository;
  LevelLog mLevel = LevelLog::Trace;

  Log() = default;

  [[nodiscard]] static std::string sanitize(std::string const &value) {
    return jmixin::String{value}.replace("\\\"", "'");
  }
};

#define logt(...)                                                              \
  Log::instance().msg(std::source_location::current(), LevelLog::Trace,        \
                      ##__VA_ARGS__)
#define logd(...)                                                              \
  Log::instance().msg(std::source_location::current(), LevelLog::Debug,        \
                      ##__VA_ARGS__)
#define logi(...)                                                              \
  Log::instance().msg(std::source_location::current(), LevelLog::Info,         \
                      ##__VA_ARGS__)
#define logw(...)                                                              \
  Log::instance().msg(std::source_location::current(), LevelLog::Warn,         \
                      ##__VA_ARGS__)
#define loge(...)                                                              \
  Log::instance().msg(std::source_location::current(), LevelLog::Error,        \
                      ##__VA_ARGS__)
#define logf(...)                                                              \
  Log::instance().msg(std::source_location::current(), LevelLog::Fatal,        \
                      ##__VA_ARGS__)

#define logopt(...)                                                            \
  Log::instance().msg(std::source_location::current(), LevelLog::Error,        \
                      ##__VA_ARGS__)
