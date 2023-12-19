#pragma once

#include "ui/Input.hpp"

#include <algorithm>
#include <charconv>
#include <functional>
#include <iostream>
#include <map>
#include <string>

#include <unistd.h>
#include <signal.h>

#include <fmt/format.h>

#include "jmixin/jstring.h"

static bool cancelled = false;

namespace jui {

template <StringLiteral Name, StringLiteral Description, TypeItem Type>
struct Item {
  static constexpr std::string name = Name.to_string();
  static constexpr std::string description = Description.to_string();
  static constexpr TypeItem type = Type;
};

template <typename... Items> struct Form {
  Form() {
    // TODO:: verify duplicates
  }

  Form &before(std::function<void()> callback) {
    mBefore = callback;

    return *this;
  }

  Form &on_success(std::function<void(Input)> callback) {
    mOnSuccess = callback;

    return *this;
  }

  Form &on_failed(std::function<void()> callback) {
    mOnFailed = callback;

    return *this;
  }

  Form &after(std::function<void()> callback) {
    mAfter = callback;

    return *this;
  }

  template <typename T>
  Form &set(std::string const &key, T const &value) {
    using namespace std;

    // TODO:: validar key
    mValues[key] = to_string(value);

    return *this;
  }

  void show() {
    struct sigaction new_action, old_action;

    mBefore();

    new_action.sa_handler = [](int) {
      cancelled = true;
    };
    sigemptyset(&new_action.sa_mask);
    new_action.sa_flags = 0;

    sigaction(SIGINT, &new_action, &old_action);

    try {
      (execute(Items{}), ...);

      mOnSuccess(Input(mValues));
    } catch (std::runtime_error &e) {
      fmt::print("[error]: {}\n", e.what());

      mOnFailed();
    }

    if (cancelled) {
      std::cin.clear();
    }

    cancelled = false;

    sigaction(SIGINT, &old_action, NULL);

    mAfter();
  }

private:
  std::map<std::string, std::string> mValues;

  std::function<void()> mBefore = []() {};
  std::function<void()> mAfter = []() { /* system("clear"); */ };
  std::function<void(Input)> mOnSuccess = [](Input) {};
  std::function<void()> mOnFailed = []() {};

  template <StringLiteral Name, StringLiteral Description, TypeItem Type>
  void execute(Item<Name, Description, Type> item) {
    auto defaultValue = mValues.find(Name.to_string());
    std::string line;

    if (defaultValue == mValues.end()) {
      fmt::print("{}\n: ", Description.to_string());
    } else {
      fmt::print("{} [default: {}]\n: ", Description.to_string(), defaultValue->second);
    }

    std::getline(std::cin, line);

    line = jmixin::String(line).trim();

    if (line.empty() and defaultValue != mValues.end()) {
      line = defaultValue->second;
    }

    if (Type == TypeItem::Text) {
      mValues[Name.to_string()] = line;
    } else if (Type == TypeItem::Decimal) {
      double number;

      auto [ptr, ec] =
          std::from_chars(line.data(), line.data() + line.size(), number);

      if (ec != std::errc{}) {
        throw std::runtime_error(
            fmt::format("Invalid conversion: '{}' to decimal", line));
      }

      mValues[Name.to_string()] = std::to_string(number);
    } else if (Type == TypeItem::Int) {
      long number;

      auto [ptr, ec] =
          std::from_chars(line.data(), line.data() + line.size(), number);

      if (ec != std::errc{}) {
        throw std::runtime_error(
            fmt::format("Invalid conversion: '{}' to integer", line));
      }

      mValues[Name.to_string()] = std::to_string(number);
    }
  }
};
} // namespace jui
