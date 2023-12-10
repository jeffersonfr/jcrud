#pragma once

#include "model/DataClass.hpp"
#include "model/Migration.hpp"

#include <chrono>
#include <functional>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include <fmt/format.h>

struct Database {
  virtual ~Database() = default;

  virtual int query_string(std::string const &sql,
                           std::function<bool(std::vector<std::string> const &,
                                              std::vector<Data> const &)>
                               callback) = 0;

  template <typename Model> bool insert(Model const &model) {
    std::ostringstream o;
    bool first = true;

    o << "INSERT INTO " << Model::get_name() << " (";

    model.get_fields([&]<typename Field>() {
      if (!first) {
        o << ", ";
      }

      first = false;

      o << Field::get_name();
    });

    o << ") VALUES (";

    first = true;

    model.get_fields([&]<typename Field>() {
      if (!first) {
        o << ", ";
      }

      first = false;

      auto const &value = model[Field::get_name()];

      value.get_value(overloaded{
          [&](nullptr_t arg) {
            if (!Field::is_nullable() and
                Field::get_type() != FieldType::Serial) {
              throw std::runtime_error(
                  fmt::format("unable to insert '{}', field '{}' is not null",
                              Model::get_name(), Field::get_name()));
            }
            o << "null";
          },
          [&](bool arg) {
            if (Field::get_type() != FieldType::Bool and
                Field::get_type() != FieldType::Int) {
              throw std::runtime_error(
                  fmt::format("unable to insert '{}', field '{}' is not "
                              "convertible to boolean",
                              Model::get_name(), Field::get_name()));
            }
            o << (arg ? "true" : "false");
          },
          [&](int64_t arg) {
            if (Field::get_type() != FieldType::Int) {
              throw std::runtime_error(
                  fmt::format("unable to insert '{}', field '{}' is not "
                              "convertible to integer",
                              Model::get_name(), Field::get_name()));
            }
            o << arg;
          },
          [&](double arg) {
            if (Field::get_type() != FieldType::Decimal) {
              throw std::runtime_error(
                  fmt::format("unable to insert '{}', field '{}' is not "
                              "convertible to double",
                              Model::get_name(), Field::get_name()));
            }
            o << arg;
          },
          [&](std::string arg) {
            if (Field::get_type() != FieldType::Text) {
              throw std::runtime_error(fmt::format(
                  "unable to insert '{}', field '{}' is not a text value",
                  Model::get_name(), Field::get_name()));
            }
            o << std::quoted(arg);
          }});
    });

    o << ");";

    query_string(o.str(), [](auto...) { return false; });

    return true;
  }

  template <typename Model> bool update(Model const &model) {
    std::ostringstream o;
    bool first = true;

    o << "UPDATE " << Model::get_name() << " SET ";

    model.get_fields([&]<typename Field>() {
      if (!first) {
        o << ", ";
      }

      first = false;

      o << Field::get_name() << " = ";

      auto const &value = model[Field::get_name()];

      value.get_value(overloaded{
          [&](nullptr_t arg) {
            if (!Field::is_nullable() and
                Field::get_type() != FieldType::Serial) {
              throw std::runtime_error(
                  fmt::format("unable to insert '{}', field '{}' is not null",
                              Model::get_name(), Field::get_name()));
            }
            o << "null";
          },
          [&](bool arg) {
            if (Field::get_type() != FieldType::Bool and
                Field::get_type() != FieldType::Int) {
              throw std::runtime_error(
                  fmt::format("unable to insert '{}', field '{}' is not "
                              "convertible to boolean",
                              Model::get_name(), Field::get_name()));
            }
            o << (arg ? "true" : "false");
          },
          [&](int64_t arg) {
            if (Field::get_type() != FieldType::Int) {
              throw std::runtime_error(
                  fmt::format("unable to insert '{}', field '{}' is not "
                              "convertible to integer",
                              Model::get_name(), Field::get_name()));
            }
            o << arg;
          },
          [&](double arg) {
            if (Field::get_type() != FieldType::Decimal) {
              throw std::runtime_error(
                  fmt::format("unable to insert '{}', field '{}' is not "
                              "convertible to double",
                              Model::get_name(), Field::get_name()));
            }
            o << arg;
          },
          [&](std::string arg) {
            if (Field::get_type() != FieldType::Text) {
              throw std::runtime_error(fmt::format(
                  "unable to insert '{}', field '{}' is not a text value",
                  Model::get_name(), Field::get_name()));
            }
            o << std::quoted(arg);
          }});
    });

  get_where_from_primary_keys<Model>(o, model);

  o << ";";

  query_string(o.str(), [](auto...) { return false; });

  return true;
}

template <typename Model>
bool remove(Model const &model) {
  std::ostringstream o;
  bool first = true;

  o << "DELETE FROM " << Model::get_name();

  get_where_from_primary_keys<Model>(o, model);

  o << ";";

  query_string(o.str(), [](auto...) { return false; });

  return true;
}

virtual Database &add_migration(Migration migration) = 0;

private:
template <typename Model>
void get_where_from_primary_keys(std::ostream &out, Model const &model) {
  std::ostringstream o;
  bool first = true;

  out << " WHERE ";

  model.get_keys([&]<typename Field>() {
    if (!first) {
      out << " AND ";
    }

    auto const &value = model[Field::get_name()];

    value.get_value(overloaded{
        [&](nullptr_t arg) { out << "(" << Field::get_name() << " IS NULL)"; },
        [&](bool arg) {
          out << "(" << Field::get_name() << " = " << (arg ? "1" : "0") << ")";
        },
        [&](int64_t arg) {
          out << "(" << Field::get_name() << " = " << arg << ")";
        },
        [&](double arg) {
          out << "(" << Field::get_name() << " = " << arg << ")";
        },
        [&](std::string arg) {
          out << "(" << Field::get_name() << " LIKE '%" << arg << "%')";
        }});
  });
}
}
;
