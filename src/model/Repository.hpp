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

    mDb->query_string(fmt::format("SELECT * from {}", Model::get_name()),
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

  template <StringLiteral... Fields>
  std::vector<Model> load_by(auto... values) const {
    std::vector<Model> items;
    std::ostringstream o;

    o << "SELECT * from " << Model::get_name() << " WHERE ";

    for_each_where<0, Fields...>(o, values...);

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
  std::vector<Model> first_by(auto... values) const {
    std::vector<Model> items;
    std::ostringstream o;

    o << "SELECT * from " << Model::get_name() << " ORDER BY ";

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

      return true;
    });

    if (items.empty()) {
      return {};
    }

    return {items[0]};
  }

  template <StringLiteral... Fields>
  std::vector<Model> last_by(auto... values) const {
    std::vector<Model> items;
    std::ostringstream o;

    o << "SELECT * from " << Model::get_name() << " ORDER BY ";

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

      return true;
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

  void save(Model const &item) const { mDb->insert(item); }

  void save_all(std::vector<Model> const &items) const {
    for (auto const &item : items) {
      save(item);
    }
  }

  /*
  void update(Model const &item) const override {
    mDb->update(item);
  }
  */

  void remove(ProdutoModel const &model) const { mDb->remove(model); }

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
