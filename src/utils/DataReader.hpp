#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <chrono>

struct DataReader {
  inline static constinit int MAX_TEXT_SIZE = 1024;

  DataReader(std::istream &stream) : mStream{stream} {}

  int read_int() {
    int value;

    mStream >> value;

    return value;
  }

  float read_decimal() {
    int value;
    int precision;

    mStream >> value >> precision;

    return static_cast<float>(value) / std::pow(10, precision);
  }

  std::string read_text() {
    std::string value;
    int size;

    mStream >> size;

    if (size > MAX_TEXT_SIZE) {
      throw std::runtime_error("Unable to read huge texts");
    }

    value.resize(size);

    mStream.ignore();
    mStream.read(&value[0], size);

    return value;
  }

  std::chrono::year_month_day read_date() {
    int y;
    unsigned m;
    unsigned d;

    mStream >> y >> m >> d;
    
    return std::chrono::year_month_day{std::chrono::year{y}, std::chrono::month{m}, std::chrono::day{d}};
  }

  std::chrono::seconds read_time() {
    typename std::chrono::seconds::rep value;

    mStream >> value;

    return std::chrono::seconds{value};
  }

  std::time_t read_timestamp() {
    std::time_t value;

    mStream >> value;

    return value;
  }

private:
  std::istream &mStream;
};
