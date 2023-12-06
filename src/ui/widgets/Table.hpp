#pragma once

#include "model/DataClass.hpp"
#include "model/Database.hpp"
#include "ui/widgets/Core.hpp"

#include <algorithm>
#include <charconv>
#include <functional>
#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>

#include <fmt/format.h>

#include "jmixin/jstring.h"

namespace jui {

struct Col {
  std::string name;
  std::size_t size;
};

struct Row {
  Row(auto... Items) {
    using namespace std;

    (mItems.push_back(to_string(Items)), ...);
  }

  std::vector<std::string> const &items() { return mItems; }

private:
  std::vector<std::string> mItems;

  std::string to_string(Data const &value) {
    std::ostringstream o;

    value.get_value(overloaded{
        [&](nullptr_t arg) { o << "null"; },
        [&](bool arg) { o << ((arg) ? "true" : "false"); },
        [&](int64_t arg) { o << arg; }, [&](double arg) { o << arg; },
        [&](std::string arg) { o << arg; },
        [&](std::chrono::year_month_day arg) {
          std::ostringstream o;

          o << fmt::format("{:02}", static_cast<unsigned>(arg.day())) << "/"
            << fmt::format("{:02}", static_cast<unsigned>(arg.month())) << "/"
            << fmt::format("{:04}", static_cast<int>(arg.year()));
        }});

    return o.str();
  }

};

template <std::ranges::range T> struct Table {
  using value_type = std::ranges::range_value_t<T>;

  Table(T const &items) : mItems{items} {}

  Table &title(std::string title) {
    mTitle = title;

    return *this;
  }

  Table &head(std::function<std::vector<Col>()> callback) {
    mHead = callback;

    return *this;
  }

  Table &data(std::function<Row(value_type const &)> callback) {
    mData = callback;

    return *this;
  }

  void show() {
    // title
    if (!mTitle.empty()) {
      fmt::print("\n# {}", mTitle);
    }

    // head
    std::vector<Col> headItems = mHead();

    std::string line = "+";
    std::string head = "|";

    for (auto &item : headItems) {
      line += jmixin::String().center(item.size + 1, '-');
      head += jmixin::String(item.name).center(item.size) + '|';
    }

    line[line.size() - 1] = '+';
    head[head.size() - 1] = '|';

    fmt::print("\n");
    fmt::print("{}\n", line);
    fmt::print("{}\n", head);
    fmt::print("{}\n", line);

    // rows
    for (auto &item : mItems) {
      auto cols = mData(item).items();

      std::string row = "|";

      for (int i = 0; i < cols.size(); i++) {
        row += jmixin::String(cols[i]).center(headItems[i].size) + '|';
      }

      fmt::print("{}\n", row);
    }

    fmt::print("{}\n\n", line);
  }

private:
  std::function<std::vector<struct Col>()> mHead;
  std::function<Row(value_type const &item)> mData;

  T mItems;
  std::string mTitle;
};

} // namespace jui
