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
  static void msg(LogLevel level, LogType type, std::string const &description) {
    std::unique_ptr<LogRepository> mRepository = jinject::get{};

    LogModel model;

    model["level"] = static_cast<int>(level);
    model["tipo"] = static_cast<int>(type);
    model["descricao"] = description;

    mRepository->save(model);
  }

  static void d(LogType type, std::string const &msg) {
    Log::msg(LogLevel::Debug, type, msg);
  }

  static void i(LogType type, std::string const &msg) {
    Log::msg(LogLevel::Info, type, msg);
  }

  static void w(LogType type, std::string const &msg) {
    Log::msg(LogLevel::Warn, type, msg);
  }

  static void e(LogType type, std::string const &msg) {
    Log::msg(LogLevel::Error, type, msg);
  }

  static void f(LogType type, std::string const &msg) {
    Log::msg(LogLevel::Fatal, type, msg);
  }
};

