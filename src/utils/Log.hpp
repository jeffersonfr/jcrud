#pragma once

#include "model/log/LogRepository.hpp"
#include "model/level_log/LevelLogModel.hpp"
#include "model/tipo_log/TipoLogModel.hpp"

#include <string>
#include <source_location>

#include <fmt/format.h>

struct Log {
  template <typename ...Args>
  static void msg(LevelLog level, TipoLog type, std::string const &msg, 
  Args ...args, std::source_location const location = std::source_location::current()) {
    std::unique_ptr<LogRepository> mRepository = jinject::get{};

    LogModel model;

    model["level_log_id"] = static_cast<int>(level);
    model["tipo_log_id"] = static_cast<int>(type);
    model["localizacao"] = fmt::format("{} ({}:{}) {}: ", 
      location.file_name(), location.line(), location.column(), location.function_name());
    model["descricao"] = fmt::vformat(msg, fmt::make_format_args(std::forward<Args>(args)...));

    mRepository->save(model);
  }

  template <typename ...Args>
  static void d(TipoLog type, std::string const &msg, Args ...args, 
  std::source_location const location = std::source_location::current()) {
    Log::msg(LevelLog::Debug, type, msg, location);
  }

  template <typename ...Args>
  static void i(TipoLog type, std::string const &msg, Args ...args, 
  std::source_location const location = std::source_location::current()) {
    Log::msg(LevelLog::Info, type, msg, location);
  }

  template <typename ...Args>
  static void w(TipoLog type, std::string const &msg, Args ...args, 
  std::source_location const location = std::source_location::current()) {
    Log::msg(LevelLog::Warn, type, msg, location);
  }

  template <typename ...Args>
  static void e(TipoLog type, std::string const &msg, Args ...args, 
  std::source_location const location = std::source_location::current()) {
    Log::msg(LevelLog::Error, type, msg, location);
  }

  template <typename ...Args>
  static void f(TipoLog type, std::string const &msg, Args ...args, 
  std::source_location const location = std::source_location::current()) {
    Log::msg(LevelLog::Fatal, type, msg, location);
  }
};
