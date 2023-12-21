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

enum class TypeItem { Int, Decimal, Text, Date };

struct Input {
  Input(std::map<std::string, std::string> &values) : mValues{values} {}

  std::optional<std::string> get_text(std::string key) {
    auto it = mValues.find(key);

    for (auto &it : mValues) {
      if (strncmp(it.first.data(), key.data(), key.size()) == 0) {
        return {it.second};
      }
    }

    return {};
  }

  std::optional<long> get_int(std::string key) {
    auto text = get_text(key);

    if (!text) {
      return {};
    }

    auto value = *text;
    long number;

    auto [ptr, ec] =
        std::from_chars(value.data(), value.data() + value.size(), number);

    if (ec != std::errc{}) {
      return {};
    }

    return number;
  }

  std::optional<double> get_decimal(std::string key) {
    auto text = get_text(key);

    if (!text) {
      return {};
    }

    auto value = *text;
    double number;

    auto [ptr, ec] =
        std::from_chars(value.data(), value.data() + value.size(), number);

    if (ec != std::errc{}) {
      return {};
    }

    return number;
  }

  std::optional<std::chrono::year_month_day> get_date(std::string key) {
    auto text = get_text(key);

    if (!text) {
      return {};
    }

    const std::regex date_regex("[\\d]{8}");
    auto value = *text;
 
    if (!std::regex_match(value, date_regex)) {
      return {};      
    }

    auto d = std::chrono::year{std::stoi(value.substr(0, 2))};
    auto m = std::chrono::month{static_cast<unsigned>(std::stoi(value.substr(2, 2)))};
    auto y = std::chrono::day{static_cast<unsigned>(std::stoi(value.substr(4, 4)))};

    return std::chrono::year_month_day{d, m, y};
  }

private:
  std::map<std::string, std::string> &mValues;
};

} // namespace jui
