#pragma once

#include <map>
#include <string>

#include "jmixin/jstring.h"

namespace jui {
  enum class TypeItem { Int, Decimal, Text, Date };

  struct Input {
    explicit Input(std::map<std::string, std::optional<std::string> > &values) : mValues{values} {
    }

    [[nodiscard]] std::optional<std::string> get_text(std::string const &key) const {
      auto it = mValues.find(key);

      if (it != mValues.end()) {
        return it->second;
      }

      return {};
    }

    [[nodiscard]] std::optional<int32_t> get_int(std::string const &key) const {
      auto text = get_text(key);

      if (!text.has_value()) {
        return {};
      }

      try {
        return std::stoi(*text);
      } catch (std::invalid_argument &) {
        // logt
      } catch (std::out_of_range &) {
        // logt
      }

      return {};
    }

    [[nodiscard]] std::optional<int64_t> get_long(std::string const &key) const {
      auto text = get_text(key);

      if (!text.has_value()) {
        return {};
      }

      try {
        return std::stoll(*text);
      } catch (std::invalid_argument &) {
        // logt
      } catch (std::out_of_range &) {
        // logt
      }

      return {};
    }

    [[nodiscard]] std::optional<double> get_decimal(std::string const &key) const  {
      auto text = get_text(key);

      if (!text.has_value()) {
        return {};
      }

      try {
        return std::stod(*text);
      } catch (std::invalid_argument &) {
        // logt
      } catch (std::out_of_range &) {
        // logt
      }

      return {};
    }

    [[nodiscard]] std::optional<std::chrono::year_month_day> get_date(std::string const &key) const {
      auto text = get_text(key);

      if (!text.has_value()) {
        return {};
      }

      const std::regex date_regex("[\\d]{8}");
      auto const &value = *text;

      if (!std::regex_match(value, date_regex)) {
        return {};
      }

      auto d = std::chrono::year{std::stoi(value.substr(0, 2))};
      auto m = std::chrono::month{static_cast<unsigned>(std::stoi(value.substr(2, 2)))};
      auto y = std::chrono::day{static_cast<unsigned>(std::stoi(value.substr(4, 4)))};

      return std::chrono::year_month_day{d, m, y};
    }

  private:
    std::map<std::string, std::optional<std::string> > &mValues;
  };

  template<typename T>
    requires (std::is_scoped_enum_v<T>)
  bool operator ==(std::optional<int32_t> const &a, T b) {
    return a.has_value() and *a == static_cast<int32_t>(b);
  }

  template<typename T>
    requires (std::is_scoped_enum_v<T>)
  bool operator ==(std::optional<int64_t> const &a, T b) {
    return a.has_value() and *a == static_cast<int32_t>(b);
  }
} // namespace jui
