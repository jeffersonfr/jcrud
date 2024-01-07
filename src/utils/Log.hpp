#pragma once

#include "model/log/LogRepository.hpp"

#include <string>
#include <source_location>

#include <fmt/format.h>

enum class LogLevel {
  Debug,
  Info,
  Warn,
  Error,
  Fatal
};

enum class LogType {
  System,
  Model
};

struct Log {
  template <typename ...Args>
  static void msg(LogLevel level, LogType type, std::string const &msg, 
  Args ...args, std::source_location const location = std::source_location::current()) {
    std::unique_ptr<LogRepository> mRepository = jinject::get{};

    LogModel model;

    std::string locationStr = fmt::format("{} ({}:{}) {}: ", 
      location.file_name(), location.line(), location.column(), location.function_name());

    model["level"] = static_cast<int>(level);
    model["tipo"] = static_cast<int>(type);
    model["descricao"] = locationStr + fmt::vformat(msg, fmt::make_format_args(std::forward<Args>(args)...));

    mRepository->save(model);
  }

  template <typename ...Args>
  static void d(LogType type, std::string const &msg, Args ...args, 
  std::source_location const location = std::source_location::current()) {
    Log::msg(LogLevel::Debug, type, msg, location);
  }

  template <typename ...Args>
  static void i(LogType type, std::string const &msg, Args ...args, 
  std::source_location const location = std::source_location::current()) {
    Log::msg(LogLevel::Info, type, msg, location);
  }

  template <typename ...Args>
  static void w(LogType type, std::string const &msg, Args ...args, 
  std::source_location const location = std::source_location::current()) {
    Log::msg(LogLevel::Warn, type, msg, location);
  }

  template <typename ...Args>
  static void e(LogType type, std::string const &msg, Args ...args, 
  std::source_location const location = std::source_location::current()) {
    Log::msg(LogLevel::Error, type, msg, location);
  }

  template <typename ...Args>
  static void f(LogType type, std::string const &msg, Args ...args, 
  std::source_location const location = std::source_location::current()) {
    Log::msg(LogLevel::Fatal, type, msg, location);
  }
};
