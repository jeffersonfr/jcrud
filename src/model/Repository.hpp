#pragma once

#include "model/Database.hpp"
#include "utils/Format.hpp"

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
  std::vector<Model> load_by(auto ...values) const {
    std::vector<Model> items;
    std::ostringstream o;

    o << "SELECT * from " << Model::get_name() << " WHERE ";

    for_each<0, Fields...>(o, values...);

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
  std::shared_ptr<Database> mDb = jinject::get{};

  template <std::size_t Index, StringLiteral Field, StringLiteral... Fields>
  void for_each(std::ostream &out, Data value, auto ...values) const {
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
        },
        [&](std::chrono::year_month_day arg) {
          /* TODO:: see this type
          std::ostringstream o;

          o << fmt::format("{:02}", static_cast<unsigned>(arg.day())) << "/"
            << fmt::format("{:02}", static_cast<unsigned>(arg.month())) << "/"
            << fmt::format("{:04}", static_cast<int>(arg.year()));

          query.bind(i + 1, o.str());
          */
        }});

    if constexpr (sizeof...(Fields) > 0) {
      for_each<Index + 1, Fields...>(values...);
    }
  }
};
