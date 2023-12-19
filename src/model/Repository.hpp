#pragma once

#include "model/Database.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include "jinject/jinject.h"

#include <fmt/format.h>

template <typename T> struct Repository {

  using Model = T;

  Repository(std::shared_ptr<Database> db) : mDb{db} {}

  std::vector<Model> load_all() const {
    std::vector<Model> items;

    mDb->query_string(
        fmt::format("SELECT * from {} ORDER BY ROWID", Model::get_name()),
        [&](std::vector<std::string> const &columns,
            std::vector<Data> const &values) {
          Model item;

          for (int i = 0; i < columns.size(); i++) {
            std::string const &column = columns[i];

            item[column] = values[i];
          }

          items.emplace_back(item);

          return true;
        });

    return items;
  }

  template <StringLiteral... Fields> int64_t count_by(auto... values) const {
    std::ostringstream o;
    int64_t result = 0L;

    o << "SELECT COUNT (*) from " << Model::get_name() << " WHERE ";

    for_each_where<0, Fields...>(o, values...);

    mDb->query_string(o.str(), [&](std::vector<std::string> const &columns,
                                   std::vector<Data> const &values) {
      result = values[0].get_int().value();

      return false;
    });

    return result;
  }

  template <StringLiteral... Fields>
  std::vector<Model> load_by(auto... values) const {
    std::vector<Model> items;
    std::ostringstream o;

    o << "SELECT * from " << Model::get_name() << " WHERE ";

    for_each_where<0, Fields...>(o, values...);

    o << " ORDER BY ROWID";

    mDb->query_string(o.str(), [&](std::vector<std::string> const &columns,
                                   std::vector<Data> const &values) {
      Model item;

      for (int i = 0; i < columns.size(); i++) {
        std::string const &column = columns[i];

        item[column] = values[i];
      }

      items.emplace_back(item);

      return true;
    });

    return items;
  }

  template <StringLiteral... Fields>
  std::optional<Model> first_by(auto... values) const {
    std::vector<Model> items;
    std::ostringstream o;

    if constexpr (sizeof...(values) == 0) {
      o << "SELECT * from " << Model::get_name() << " ORDER BY ";
    } else {
      o << "SELECT * from " << Model::get_name() << " WHERE ";

      for_each_where<0, Fields...>(o, values...);

      o << " ORDER BY ROWID, ";
    }

    for_each_order<0, Fields...>(o);

    o << " ASC LIMIT 1";

    mDb->query_string(o.str(), [&](std::vector<std::string> const &columns,
                                   std::vector<Data> const &values) {
      Model item;

      for (int i = 0; i < columns.size(); i++) {
        std::string const &column = columns[i];

        item[column] = values[i];
      }

      items.emplace_back(item);

      return false;
    });

    if (items.empty()) {
      return {};
    }

    return {items[0]};
  }

  template <StringLiteral... Fields>
  std::optional<Model> last_by(auto... values) const {
    std::vector<Model> items;
    std::ostringstream o;

    if constexpr (sizeof...(values) == 0) {
      o << "SELECT * from " << Model::get_name() << " ORDER BY ";
    } else {
      o << "SELECT * from " << Model::get_name() << " WHERE ";

      for_each_where<0, Fields...>(o, values...);

      o << " ORDER BY ROWID, ";
    }

    for_each_order<0, Fields...>(o);

    o << " DESC LIMIT 1";

    mDb->query_string(o.str(), [&](std::vector<std::string> const &columns,
                                   std::vector<Data> const &values) {
      Model item;

      for (int i = 0; i < columns.size(); i++) {
        std::string const &column = columns[i];

        item[column] = values[i];
      }

      items.emplace_back(item);

      return false;
    });

    if (items.empty()) {
      return {};
    }

    return {items[0]};
  }

  /*
    Find a unique registry using the primary key's ids. Is there is more than
    one registry, the method returns the first result of the list;
  */
  std::optional<Model> find(auto... values) {
    auto result = find_expanded(typename Model::Keys{}, values...);

    if (!result.empty()) {
      return {result[0]};
    }

    return {};
  }

  std::expected<Model, std::exception> save(Model const &item) const {
    try {
      return mDb->insert(item);
    } catch (std::runtime_error &e) {
      return std::unexpected{e};
    }
  }

  void save_all(std::vector<Model> const &items) const {
    for (auto const &item : items) {
      save(item);
    }
  }

  void update(Model const &item) const {
    mDb->update(item);
  }

  void remove(Model const &model) const { mDb->remove(model); }

  void remove_all(std::vector<Model> const &items) const {
    for (auto const &item : items) {
      remove(item);
    }
  }

  template <StringLiteral... Fields> void remove_by(Data values...) const {
    auto items = load_by<Fields...>(values);

    for (auto const &item : items) {
      remove(item);
    }
  }

private:
  std::shared_ptr<Database> mDb;

  template <StringLiteral... Keys>
  auto find_expanded(PrimaryKeys<Keys...> primaryKeys, auto... values) {
    return load_by<Keys...>(values...);
  }

  template <std::size_t Index, StringLiteral Field, StringLiteral... Fields>
  void for_each_where(std::ostream &out, Data value, auto... values) const {
    if (Index != 0) {
      out << " AND ";
    }

    value.get_value(overloaded{
        [&](nullptr_t arg) { out << "(" << Field.to_string() << " IS NULL)"; },
        [&](bool arg) {
          out << "(" << Field.to_string() << " = " << (arg ? "true" : "false")
              << ")";
        },
        [&](int64_t arg) {
          out << "(" << Field.to_string() << " = " << arg << ")";
        },
        [&](double arg) {
          out << "(" << Field.to_string() << " = " << arg << ")";
        },
        [&](std::string arg) {
          out << "(" << Field.to_string() << " LIKE '%" << arg << "%')";
        }});

    if constexpr (sizeof...(Fields) > 0) {
      for_each_where<Index + 1, Fields...>(values...);
    }
  }

  template <std::size_t Index, StringLiteral Field, StringLiteral... Fields>
  void for_each_order(std::ostream &out) const {
    if (Index != 0) {
      out << ", ";
    }

    out << Field.to_string();

    if constexpr (sizeof...(Fields) > 0) {
      for_each_order<Index + 1, Fields...>();
    }
  }
};
