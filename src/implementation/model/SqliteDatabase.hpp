#pragma once

#include "model/Database.hpp"
#include "model/Migration.hpp"

#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <SQLiteCpp/SQLiteCpp.h>

#include <fmt/format.h>

/* UTC TIME
  [&](std::chrono::seconds arg) {
    std::time_t time = arg.count();
    char timeString[std::size("yyyy-mm-dd hh:mm:ss.000")];
    std::strftime(std::data(timeString), std::size(timeString),
                  "%F %T.000", std::gmtime(&time));

    query.bind(i + 1, timeString);
*/

using MigracaoModel =
    DataClass<"migracao", PrimaryKeys<"id">, Field<"id", FieldType::Int, false>,
              Field<"version", FieldType::Int>>;

template <typename... Tables> struct SqliteDatabase : public Database {
  SqliteDatabase(std::string dbName)
      : mDb(dbName, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE) {

    // Initialize MigracaoModel locally
    query_string(this->template create_ddl(MigracaoModel{}),
                 [](auto...) { return false; });

    for_each<Tables...>([&]<typename Table>() {
      query_string(this->template create_ddl(Table{}),
                   [](auto...) { return false; });
    });
  }

  virtual ~SqliteDatabase() = default;

  int64_t query_string(std::string const &sql,
                   std::function<bool(std::vector<std::string> const &,
                                      std::vector<Data> const &)>
                       callback) override {
    try {
      SQLite::Transaction transaction(mDb);
      SQLite::Statement query(mDb, sql);

      std::vector<std::string> columns;
      std::vector<Data> values;

      while (query.executeStep()) {
        if (columns.empty()) {
          for (int i = 0; i < query.getColumnCount(); i++) {
            columns.emplace_back(query.getColumn(i).getName());
          }
        }

        for (int i = 0; i < query.getColumnCount(); i++) {
          SQLite::Column col = query.getColumn(i);

          if (col.isInteger()) {
            values.emplace_back(col.getInt64());
          } else if (col.isFloat()) {
            values.emplace_back(col.getDouble());
          } else if (col.isText()) {
            values.emplace_back(col.getString());
          } else if (col.isBlob()) {
            throw std::runtime_error("Type not implemented");
          } else {
            values.emplace_back(nullptr);
          }
        }

        if (!callback(columns, values)) {
          break;
        }

        values.clear();
      }

      query.reset();

      transaction.commit();

      return query.getChanges();
    } catch (std::exception &e) {
      throw std::runtime_error(fmt::format("{}: {}", e.what(), sql));
    }
  }

  int64_t get_last_rowid() override {
    return mDb.getLastInsertRowid();
  }

  SqliteDatabase &add_migration(Migration migration) override {
    if (std::find_if(mMigrations.begin(), mMigrations.end(),
                     [id = migration.get_id()](auto const &item) {
                       return item.get_id() == id;
                     }) != mMigrations.end()) {
      throw std::runtime_error("migration id already exists");
    }

    mMigrations.push_back(migration);

    return *this;
  }

  void build() {
    MigracaoModel migracaoModel;

    migracaoModel["id"] = 1;
    migracaoModel["version"] = 0;

    query_string(
        "CREATE TABLE IF NOT EXISTS migracao (version INTEGER NOT NULL);",
        [&](auto...) { return false; });

    query_string(fmt::format("SELECT * FROM {};", MigracaoModel::get_name()),
                 [&](std::vector<std::string> const &columns,
                     std::vector<Data> const &values) {
                   for (int i = 0; i < columns.size(); i++) {
                     if (columns[i] == "version") {
                       migracaoModel["version"] = values[0].get_int().value();
                     }
                   }

                   return false;
                 });

    if (migracaoModel["version"].get_int().value() == 0) {
      insert(migracaoModel);
    }

    std::sort(
        mMigrations.begin(), mMigrations.end(),
        [](auto const &a, auto const &b) { return a.get_id() < b.get_id(); });

    for (auto const &migration : mMigrations) {
      if (migration.get_id() <= migracaoModel["version"].get_int().value()) {
        continue;
      }

      try {
        migracaoModel["version"] = migration.get_id();

        migration.execute(*this);

        update(migracaoModel);
      } catch (std::exception &e) {
        throw std::runtime_error(fmt::format("Unable to proceed with migration [version: {}]: {}", migracaoModel["version"].get_int().value(), e.what()));
      }
    }
  }

private:
  std::vector<Migration> mMigrations;
  SQLite::Database mDb;

  void fillValues(SQLite::Statement &query, std::vector<Data> const &values) {
    for (int i = 0; i < values.size(); i++) {
      auto const &value = values[i];

      value.get_value(
          overloaded{[&](nullptr_t arg) { query.bind(i + 1, nullptr); },
                     [&](bool arg) { query.bind(i + 1, arg); },
                     [&](int64_t arg) { query.bind(i + 1, arg); },
                     [&](double arg) { query.bind(i + 1, arg); },
                     [&](std::string arg) { query.bind(i + 1, arg); }});
    }
  }

  template <typename Arg, typename... Args, typename F>
  static constexpr void for_each(F callback) {
    callback.template operator()<Arg>();

    if constexpr (sizeof...(Args) > 0) {
      return for_each<Args...>(callback);
    }
  }

  template <typename F> static constexpr void for_each(F callback) {}

  template <StringLiteral Name, typename PrimaryKeys, typename... Fields>
  static constexpr std::string
  create_ddl(DataClass<Name, PrimaryKeys, Fields...> &&model) {
    std::ostringstream ddl;
    bool hasSerial = false;

    ddl << "CREATE TABLE IF NOT EXISTS " << Name.to_string() << " (";

    for_each<Fields...>([&, first = true]<typename Field>() mutable {
      if (first == false) {
        ddl << ", ";
      }

      first = false;

      ddl << Field::get_name();

      if (Field::get_type() == FieldType::Serial) {
        hasSerial = true;

        ddl << " INTEGER PRIMARY KEY AUTOINCREMENT";
      } else if (Field::get_type() == FieldType::Bool) {
        ddl << " BOOLEAN";
      } else if (Field::get_type() == FieldType::Int) {
        ddl << " INTEGER";
      } else if (Field::get_type() == FieldType::Decimal) {
        ddl << " DECIMAL";
      } else if (Field::get_type() == FieldType::Text) {
        ddl << " TEXT";
      }

      if (Field::is_nullable()) {
        ddl << " NULL";
      } else {
        ddl << " NOT NULL";
      }
    });

    model.get_keys([&]<typename Field>() {
      if (Field::is_nullable()) {
        throw std::runtime_error(fmt::format(
            "Primary key of '{}' must be not null", model.get_name()));
      }
    });

    if (hasSerial) {
      if (PrimaryKeys::get_size() != 1) {
        throw std::runtime_error(
            fmt::format("Serial must be the unique primary key on table '{}'",
                        Name.to_string()));
      }
    } else {
      std::array<std::string, PrimaryKeys::get_size()> primaryKeys;
      int i{};

      model.get_keys(
          [&]<typename Field>() { primaryKeys[i++] = Field::get_name(); });

      if (!primaryKeys.empty()) {
        ddl << ", PRIMARY KEY (";

        bool first = true;

        for (auto const &key : primaryKeys) {
          if (first == false) {
            ddl << ", ";
          }

          first = false;

          ddl << key;
        }

        ddl << ")";
      }
    }

    ddl << ");";

    return ddl.str();
  }

  template <StringLiteral Name, typename PrimaryKeys, typename... Fields>
  static constexpr std::string
  drop_ddl(DataClass<Name, PrimaryKeys, Fields...> const &dataClass) {
    std::ostringstream ddl;

    ddl << "DROP TABLE " << Name.to_string() << ";";

    return ddl.str();
  }
};
