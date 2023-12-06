#pragma once

#include "utils/DataReader.hpp"
#include "utils/DataWriter.hpp"

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include <fmt/format.h>

#include "jmixin/jstring.h"

struct FileHelper {
  static void read_lines(std::string path,
                         std::function<void(DataReader)> callback) {
    std::ifstream file;

    file.open(path);

    if (!file.is_open()) {
      fmt::print("Unable to access file '{}'\n", path);

      return;
    }

    while (!file.eof()) {
      std::string line;

      std::getline(file, line);

      line = jmixin::String(line).trim();

      if (!line.empty()) {
        std::istringstream stream{line};

        callback(DataReader{stream});
      }
    }
  }

  template <std::ranges::range Data>
  static void write_lines(std::string path, Data &data,
                          std::function<void(DataWriter, std::ranges::range_value_t<Data>)> callback) {
    std::ofstream file;

    file.open(path, std::ios_base::app);

    if (!file.is_open()) {
      fmt::print("Unable to access file '{}'\n", path);

      return;
    }

    DataWriter stream(file);

    for (auto const &item : data) {
      callback(stream, item);

      file << std::endl;
    }
  }
};
