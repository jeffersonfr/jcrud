#pragma once

#include <algorithm>
#include <functional>
#include <sstream>
#include <string>

#include <fmt/format.h>

namespace jui {
  struct Col {
    std::string name;
    std::size_t size;
  };

  struct Row {
    explicit Row(auto &&... Items) {
      using namespace std;

      (mItems.push_back(to_string(Items)), ...);
    }

    std::vector<std::string> const &items() { return mItems; }

  private:
    std::vector<std::string> mItems;

    static std::string to_string(jdb::Data const &value) {
      std::ostringstream o;

      value.get_value(jdb::overloaded{
        [&]([[maybe_unused]] std::nullptr_t arg) { o << "null"; },
        [&](bool arg) { o << (arg ? "true" : "false"); },
        [&](int64_t arg) { o << arg; }, [&](double arg) { o << arg; },
        [&](std::string const &arg) { o << arg; }
      });

      return o.str();
    }
  };

  template<std::ranges::range T>
  struct Table {
    using value_type = std::ranges::range_value_t<T>;

    explicit Table(T items) : mItems{std::move(items)} {
    }

    Table &title(std::string const &title) {
      mTitle = title;

      return *this;
    }

    Table &head(std::function<std::vector<Col>()> const &callback) {
      mHead = callback;

      return *this;
    }

    Table &data(std::function<Row(value_type const &)> const &callback) {
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

      for (auto &item: headItems) {
        line += jmixin::String().center(item.size + 1, '-');
        head += jmixin::String(item.name).upper_case().center(item.size) + '|';
      }

      line[line.size() - 1] = '+';
      head[head.size() - 1] = '|';

      fmt::print("\n");
      fmt::print("{}\n", line);
      fmt::print("{}\n", head);
      fmt::print("{}\n", line);

      // rows
      for (auto &item: mItems) {
        auto cols = mData(item).items();

        std::string row = "|";

        for (int i = 0; i < static_cast<int>(cols.size()); i++) {
          row += jmixin::String(cols[i]).upper_case().ellipses(headItems[i].size).center(headItems[i].size) + '|';
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
