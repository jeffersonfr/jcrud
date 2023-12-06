#pragma once

#include <chrono>
#include <iostream>
#include <sstream>
#include <string>

struct DataWriter {
  DataWriter(std::ostream &stream) : mStream{stream} {}

  void write_int(int value) { mStream << ' ' << value; }

  void write_decimal(float value, std::size_t precision = 2) {
    mStream << ' ' << static_cast<int>(value * std::pow(10, precision)) << ' '
            << precision;
  }

  void write_text(std::string value) {
    mStream << ' ' << value.size() << ' ' << value;
  }

  void write_date(std::chrono::year_month_day value) {
    if (!value.ok()) {
      throw std::runtime_error("Invalid date");
    }

    mStream << ' ' << static_cast<int>(value.year()) << ' '
            << static_cast<unsigned>(value.month()) << ' '
            << static_cast<unsigned>(value.day());
  }

  template <typename Rep, typename Period>
  void write_time(std::chrono::duration<Rep, Period> value) {
    mStream << ' '
            << std::chrono::duration_cast<std::chrono::seconds>(value).count();
  }

  void write_timestamp(std::time_t value = std::chrono::system_clock::to_time_t(
                           std::chrono::system_clock::now())) {
    mStream << ' ' << value;
  }

private:
  std::ostream &mStream;
};
