#pragma once

#include "ui/Input.hpp"

#include <algorithm>
#include <charconv>
#include <functional>
#include <iostream>
#include <map>
#include <string>

#include <signal.h>
#include <unistd.h>

#include <fmt/format.h>

#include "jmixin/jstring.h"

static bool cancelled = false;

namespace jui {

template <StringLiteral Name, StringLiteral Description, TypeItem Type>
struct Item {
  static constexpr std::string get_name() { return Name.to_string(); }

  static constexpr std::string get_description() {
    return Description.to_string();
  }

  static constexpr TypeItem get_type() { return Type; }
};

template <typename... Items> struct Form {
  Form() {
    std::array<std::string_view, sizeof...(Items)> items;
    int i = 0;

    (static_cast<void>(items[i++] = Items::get_name()), ...);

    std::sort(items.begin(), items.end());

    if (auto it = std::unique(items.begin(), items.end());
        items.size() != 0 and it != items.end()) {
      throw std::runtime_error("Duplicated itens in form");
    }
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

  Form &on_cancelled(std::function<void()> callback) {
    mOnCancelled = callback;

    return *this;
  }

  Form &after(std::function<void()> callback) {
    mAfter = callback;

    return *this;
  }

  template <typename T> Form &set(std::string const &key, T const &value) {
    using namespace std;

    mValues[key] = to_string(value);

    return *this;
  }

  Form &interruptable() {
    mInterruptable = true;
    return *this;
  }

  void show() {
    struct sigaction new_action, old_action;

    mBefore();

    if (!mInterruptable) {
      new_action.sa_handler = [](int) { cancelled = true; };
      sigemptyset(&new_action.sa_mask);
      new_action.sa_flags = 0;

      sigaction(SIGINT, &new_action, &old_action);
    }

    try {
      (execute(Items{}), ...);

      if (mInterruptable or !cancelled) {
        mOnSuccess(Input(mValues));
      }
    } catch (std::runtime_error &e) {
      fmt::print("[error]: {}\n", e.what());

      mOnFailed();
    }

    if (!mInterruptable) {
      if (cancelled) {
        std::cin.clear();

        mOnCancelled();
      }

      cancelled = false;

      sigaction(SIGINT, &old_action, NULL);
    }

    mAfter();
  }

private:
  std::map<std::string, std::string> mValues;

  std::function<void()> mBefore = []() {};
  std::function<void()> mAfter = []() {};
  std::function<void(Input)> mOnSuccess = [](Input) {};
  std::function<void()> mOnFailed = []() {};
  std::function<void()> mOnCancelled = []() {};

  bool mInterruptable = false;

  template <StringLiteral Name, StringLiteral Description, TypeItem Type>
  void execute(Item<Name, Description, Type> item) {
    if (!mInterruptable) {
      if (cancelled) {
        return;
      }
    }

    auto defaultValue = mValues.find(Name.to_string());
    std::string line, help;

    if (Type == TypeItem::Text) {
      help = "[ texto livre ]";
    } else if (Type == TypeItem::Int) {
      help = "[ valor inteiro ]";
    } else if (Type == TypeItem::Decimal) {
      help = "[ valor decimal ]";
    } else if (Type == TypeItem::Date) {
      help = "[ data no formato: ddmmaaaa ]";
    }

    if (defaultValue == mValues.end()) {
      fmt::print("{} {}\n: ", Description.to_string(), help);
    } else {
      fmt::print("{} <default: {}> {}\n: ", Description.to_string(),
                 defaultValue->second, help);
    }

    std::getline(std::cin, line);

    line = jmixin::String(line).trim();

    if (line.empty() and defaultValue != mValues.end()) {
      line = defaultValue->second;
    }

    mValues[Name.to_string()] = line;
  }
};
} // namespace jui
