#pragma once

#include "ui/Input.hpp"

#include <algorithm>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <format>

namespace jui {

struct Col {
  std::string name;
  std::size_t size;
};

struct Row {
  Row(auto &&...Items) {
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
        [&](bool arg) { o << (arg ? "true" : "false"); },
        [&](int64_t arg) { o << arg; }, [&](double arg) { o << arg; },
        [&](std::string arg) { o << arg; }});

    return o.str();
  }
};

template <std::ranges::range T> struct Table {
  using value_type = std::ranges::range_value_t<T>;

  Table(T items) : mItems{std::move(items)} {}

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
      std::print("\n# {}", mTitle);
    }

    // head
    std::vector<Col> headItems = mHead();

    std::string line = "+";
    std::string head = "|";

    for (auto &item : headItems) {
      line += jmixin::String().center(item.size + 1, '-');
      head += jmixin::String(item.name).upper_case().center(item.size) + '|';
    }

    line[line.size() - 1] = '+';
    head[head.size() - 1] = '|';

    std::print("\n");
    std::print("{}\n", line);
    std::print("{}\n", head);
    std::print("{}\n", line);

    // rows
    for (auto &item : mItems) {
      auto cols = mData(item).items();

      std::string row = "|";

      for (int i = 0; i < cols.size(); i++) {
        row += jmixin::String(cols[i]).upper_case().ellipses(headItems[i].size).center(headItems[i].size) + '|';
      }

      std::print("{}\n", row);
    }

    std::print("{}\n\n", line);
  }

private:
  std::function<std::vector<struct Col>()> mHead;
  std::function<Row(value_type const &item)> mData;

  T mItems;
  std::string mTitle;
};

} // namespace jui
