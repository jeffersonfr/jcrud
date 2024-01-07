#pragma once

#include "model/log/LogRepository.hpp"

#include <string>

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
  static void msg(LogLevel level, LogType type, std::string const &msg, Args ...args) {
    std::unique_ptr<LogRepository> mRepository = jinject::get{};

    LogModel model;

    model["level"] = static_cast<int>(level);
    model["tipo"] = static_cast<int>(type);
    model["descricao"] = fmt::vformat(msg, fmt::make_format_args(std::forward<Args>(args)...));

    mRepository->save(model);
  }

  template <typename ...Args>
  static void d(LogType type, std::string const &msg, Args ...args) {
    Log::msg(LogLevel::Debug, type, msg);
  }

  template <typename ...Args>
  static void i(LogType type, std::string const &msg, Args ...args) {
    Log::msg(LogLevel::Info, type, msg);
  }

  template <typename ...Args>
  static void w(LogType type, std::string const &msg, Args ...args) {
    Log::msg(LogLevel::Warn, type, msg);
  }

  template <typename ...Args>
  static void e(LogType type, std::string const &msg, Args ...args) {
    Log::msg(LogLevel::Error, type, msg);
  }

  template <typename ...Args>
  static void f(LogType type, std::string const &msg, Args ...args) {
    Log::msg(LogLevel::Fatal, type, msg);
  }
};
