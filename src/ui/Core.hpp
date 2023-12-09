#pragma once

#include "utils/StringLiteral.hpp"

#include <algorithm>
#include <charconv>
#include <functional>
#include <iostream>
#include <map>
#include <string>

#include <fmt/format.h>

#include "jmixin/jstring.h"

namespace jui {

enum class TypeItem { Int, Decimal, Text };

struct Input {
  Input(std::map<std::string, std::string> &values) : mValues{values} {}

  std::string get_text(std::string key) {
    auto it = mValues.find(key);

    for (auto &it : mValues) {
      if (strncmp(it.first.data(), key.data(), key.size()) == 0) {
        return it.second;
      }
    }

    throw std::runtime_error("Inexistent key '" + key + "'");
  }

  long get_int(std::string key) {
    std::string value = get_text(key);

    long number;

    auto [ptr, ec] =
        std::from_chars(value.data(), value.data() + value.size(), number);

    if (ec != std::errc{}) {
      throw std::runtime_error(
          fmt::format("Unable to convert '{}' to integer", value));
    }

    return number;
  }

  long get_decimal(std::string key) {
    std::string value = get_text(key);

    double number;

    auto [ptr, ec] =
        std::from_chars(value.data(), value.data() + value.size(), number);

    if (ec != std::errc{}) {
      throw std::runtime_error(
          fmt::format("Unable to convert '{}' to decimal", value));
    }

    return number;
  }

  // TODO::
  std::chrono::seconds get_unixepoch(std::string key) {
    return std::chrono::seconds{};
  }

private:
  std::map<std::string, std::string> &mValues;
};

} // namespace jui
