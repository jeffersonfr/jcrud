#pragma once

#include "utils/StringLiteral.hpp"

#include <chrono>
#include <functional>
#include <iomanip>
#include <iostream>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include <fmt/format.h>

template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };

enum class FieldType {
  Serial,  // primary key auto incremented
  Bool,    // bool
  Int,     // integer
  Decimal, // double
  Text     // string
};

template <typename T>
concept FieldConcept = requires(T t) {
  { T::get_name() } -> std::same_as<std::string>;
  { T::get_type() } -> std::same_as<FieldType>;
  { T::is_nullable() } -> std::same_as<bool>;
};

template <StringLiteral Name, FieldType Type, bool Nullable = true>
struct Field {
  constexpr static std::string get_name() { return Name.to_string(); }

  constexpr static FieldType get_type() { return Type; }

  constexpr static bool is_nullable() { return Nullable; }
};

template <StringLiteral Name, FieldType Type, bool Nullable = true>
std::ostream &operator<<(std::ostream &out, Field<Name, Type, Nullable> field) {
  out << get_name(field) << " " << get_type(field) << " " << is_nullable(field);

  return out;
}

template <StringLiteral Name, bool Nullable = true>
using BoolField = Field<Name, FieldType::Bool, Nullable>;

template <StringLiteral Name, bool Nullable = true>
using IntField = Field<Name, FieldType::Int, Nullable>;

template <StringLiteral Name, bool Nullable = true>
using DecimalField = Field<Name, FieldType::Decimal, Nullable>;

template <StringLiteral Name, bool Nullable = true>
using TextField = Field<Name, FieldType::Text, Nullable>;

template <typename T>
concept PrimaryConcept = requires(T t) {
  { T::get_size() } -> std::same_as<std::size_t>;
  { T::get_keys([]<Field F>() {}) };
};

template <StringLiteral... Keys> struct Primary {
  constexpr Primary() {
    std::array<std::string_view, sizeof...(Keys)> primaryKeys;
    int i = 0;

    (static_cast<void>(primaryKeys[i++] = Keys.to_string()), ...);

    std::sort(primaryKeys.begin(), primaryKeys.end());

    if (auto it = std::unique(primaryKeys.begin(), primaryKeys.end());
        primaryKeys.size() != 0 and it != primaryKeys.end()) {
      throw std::runtime_error("Duplicated primary key");
    }
  }

  static constexpr std::size_t get_size() { return sizeof...(Keys); }

  template <typename F> static constexpr void get_keys(F callback) {
    for_each<F, Keys...>(callback);
  }

private:
  template <typename F, StringLiteral Arg, StringLiteral... Args>
  static constexpr void for_each(F callback) {
    callback.template operator()<Arg>();

    if constexpr (sizeof...(Args) > 0) {
      return for_each<F, Args...>(callback);
    }
  }

  template <typename F> static constexpr void for_each(F callback) {
    // no primary keys
  }
};

using NoPrimary = Primary<>;

template <typename T>
concept ReferConcept = requires(T t) {
  { typename T::Model{} } -> std::same_as<typename T::Model>;
  { T::get_name() } -> std::same_as<std::string>;
};

template <typename T, StringLiteral Field> struct Refer {
  using Model = T;

  constexpr static std::string get_name() { return Field.to_string(); }
};

template <typename T>
concept ForeignConcept = requires(T t) {
  { T::get_size() } -> std::same_as<std::size_t>;
  { T::get_refers([]<Refer R>() {}) };
};

template <ReferConcept ...Refers> struct Foreign {
  constexpr Foreign() {
    std::array<std::string_view, sizeof...(Refers)> refers;
    int i = 0;

    (static_cast<void>(refers[i++] = Refers::get_name()), ...);

    std::sort(refers.begin(), refers.end());

    if (auto it = std::unique(refers.begin(), refers.end());
        refers.size() != 0 and it != refers.end()) {
      throw std::runtime_error("Duplicated foreign key");
    }
  }

  static constexpr std::size_t get_size() { return sizeof...(Refers); }

  template <typename F> static constexpr void get_refers(F callback) {
    for_each<F, Refers...>(callback);
  }

private:
  template <typename F, typename Arg, typename... Args>
  static constexpr void for_each(F callback) {
    callback.template operator()<Arg>();

    if constexpr (sizeof...(Args) > 0) {
      return for_each<F, Args...>(callback);
    }
  }

  template <typename F> static constexpr void for_each(F callback) {
    // no primary keys
  }
};

using NoForeign = Foreign<>;

struct Data {
  using MyData = std::variant<nullptr_t, bool, int64_t, double, std::string>;

  Data() = default;

  template <typename T> Data(T &&data) : mData{std::forward<T>(data)} {}

  template <typename T> Data(std::optional<T> data) : mData{data.value()} {}

  template <typename F> constexpr void get_value(F &&callback) const {
    std::visit(std::forward<F>(callback), mData);
  }

  bool is_null() const { return std::get_if<nullptr_t>(&mData) != nullptr; }

  std::optional<bool> get_bool() const {
    return get_int().and_then(
        [](auto value) { return std::optional{static_cast<bool>(value)}; });
  }

  std::optional<int64_t> get_int() const {
    if (auto *value = std::get_if<int64_t>(&mData); value != nullptr) {
      return {*value};
    }

    return {};
  }

  std::optional<double> get_decimal() const {
    if (auto *value = std::get_if<double>(&mData); value != nullptr) {
      return {*value};
    }

    return {};
  }

  std::optional<std::string> get_text() const {
    if (auto *value = std::get_if<std::string>(&mData); value != nullptr) {
      return {*value};
    }

    return {};
  }

private:
  MyData mData;
};

template <StringLiteral Name, PrimaryConcept PrimaryKeys, ForeignConcept ForeignKeys, FieldConcept... Fields>
struct DataClass {
  using Keys = PrimaryKeys;
  using Refers = ForeignKeys;

  constexpr DataClass() {
    std::array<std::string_view, sizeof...(Fields)> fields;
    int i = 0;

    (static_cast<void>(fields[i++] = Fields::get_name()), ...);

    std::sort(fields.begin(), fields.end());
    if (auto it = std::unique(fields.begin(), fields.end());
        it != fields.end()) {
      throw std::runtime_error(fmt::format(
          "Duplicated fields in model definition of '{}'", Name.to_string()));
    }
  }

  static constexpr std::string get_name() { return Name.to_string(); }

  template <typename F> static constexpr void get_fields(F callback) {
    for_each<Fields...>(callback);
  }

  template <typename F> static constexpr void get_keys(F callback) {
    PrimaryKeys::template get_keys([&]<StringLiteral Key>() {
      int index = index_of<0, Fields...>(Key.to_string());

      if (index < 0) {
        throw std::runtime_error(
            fmt::format("Inexistent primary key '{}' on table '{}'",
                        Key.to_string(), get_name()));
      }

      get_fields([&]<typename Field>() {
        if (Field::get_name() == Key.to_string()) {
          callback.template operator()<Field>();
        }
      });
    });
  }

  template <typename F> static constexpr void get_refers(F callback) {
    ForeignKeys::template get_refers([&]<typename FKey>() {
      callback.template operator()<FKey>();
    });
  }

  constexpr Data const &operator[](std::string_view name) const {
    int index = index_of<0, Fields...>(name);

    if (index < 0) {
      throw std::runtime_error(
          fmt::format("Field '{}' not available in '{}'", name, get_name()));
    }

    return mFields[index];
  }

  constexpr Data &operator[](std::string_view name) {
    int index = index_of<0, Fields...>(name);

    if (index < 0) {
      throw std::runtime_error(
          fmt::format("Field '{}' not available in '{}'", name, get_name()));
    }

    return mFields[index];
  }

  virtual std::string to_string() const {
    std::ostringstream o;
    bool first = true;

    o << std::quoted(get_name()) << ": {";

    get_fields([&]<typename Field>() {
      if (!first) {
        o << ", ";
      }

      first = false;

      o << std::quoted(Field::get_name()) << ":";

      this->operator[](Field::get_name())
          .get_value(overloaded{
              [&](nullptr_t arg) { o << "null"; },
              [&](bool arg) { o << (arg ? "true" : "false"); },
              [&](int64_t arg) { o << arg; }, [&](double arg) { o << arg; },
              [&](std::string arg) { o << std::quoted(arg); }});
    });

    o << "}";

    return o.str();
  }

  friend std::ostream &operator<<(std::ostream &out, DataClass const &value) {
    out << value.to_string();

    return out;
  }

private:
  std::array<Data, sizeof...(Fields)> mFields;

  template <typename Arg, typename... Args, typename F>
  static void for_each(F callback) {
    callback.template operator()<Arg>();

    if constexpr (sizeof...(Args) > 0) {
      return for_each<Args...>(callback);
    }
  }

  template <typename F> static void for_each(F callback) {
    throw std::runtime_error(
        fmt::format("No fields available in '{}'", get_name()));
  }

  template <int Index = 0, typename Arg, typename... Args>
  static constexpr int index_of(std::string_view name) {
    if (Arg::get_name() == name) {
      return Index;
    }

    if constexpr (sizeof...(Args) > 0) {
      return index_of<Index + 1, Args...>(name);
    } else {
      return -1;
    }
  }
};

std::string format_date(Data const &value) {
  if (value.is_null()) {
    return "<null>";
  }

  std::ostringstream o;
  std::string date = value.get_text().value();

  o << date.substr(0, 2) << "/" << date.substr(2, 2) << "/" << date.substr(4);

  return o.str();
}

std::string format_currency(Data const &value) {
  if (value.is_null()) {
    return "<null>";
  }

  return fmt::format("{:>10.2F}", value.get_decimal().value());
}
