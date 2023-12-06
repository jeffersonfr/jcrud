#pragma once


#include <chrono>
#include <string>

struct Format {
  static std::chrono::year_month_day to_year_month_day(int year, unsigned month, unsigned day) {
    return std::chrono::year_month_day{std::chrono::year{year}, std::chrono::month{month}, std::chrono::day{day}};
  }

  // TODO::
  static std::chrono::year_month_day to_year_month_day(std::string value) {
    auto d = static_cast<unsigned>(std::stoi(value.substr(0, 2)));
    auto m = static_cast<unsigned>(std::stoi(value.substr(2, 2)));
    auto y = std::stoi(value.substr(4, 4));
    
    return std::chrono::year_month_day{std::chrono::year{y}, std::chrono::month{m}, std::chrono::day{d}};
  }
};
