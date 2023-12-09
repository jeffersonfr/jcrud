#pragma once

#include "control/produto/interactor/ProdutoInteractor.hpp"
#include "implementation/model/SqliteDatabase.hpp"
#include "model/categoria_produto/CategoriaProdutoRepository.hpp"
#include "model/log/LogRepository.hpp"
#include "model/produto/ProdutoRepository.hpp"

#include "jinject/jinject.h"

#define _M(ARG, ...)                                                           \
  db.query_string(fmt:::format(ARG, ##__VA_ARGS__), [](...) { return false; })

using namespace jinject;

struct Modules {
  static void load_modules() {
    load_log();
    load_sistema();
    load_interactors();
  }

private:
  static void load_log() {
    using MyDatabase = SqliteDatabase<LogModel>;

    // Database/log
    SINGLE(std::shared_ptr<Database>, LogModel) {
      auto db = std::make_shared<MyDatabase>("log.db");

      db->add_migration(Migration{1,
                                  [](Database &db) {
                                    // do nothing
                                  }})
          .build();

      return db;
    };

    UNIQUE(LogRepository) { return new LogRepository{jinject::get{}}; };
  }

  static void load_sistema() {
    using MyDatabase =
        SqliteDatabase<CategoriaProdutoModel, ProdutoModel>;

    // Database/sistema_crud
    SINGLE(std::shared_ptr<Database>) {
      auto db = std::make_shared<MyDatabase>("sistema.db");

      db->add_migration(
            Migration{1,
                      [](Database &db) {
                        CategoriaProdutoModel categoriaProdutoModel;

                        categoriaProdutoModel["description"] = "higiene";
                        db.insert(categoriaProdutoModel);

                        categoriaProdutoModel["description"] = "limpeza";
                        db.insert(categoriaProdutoModel);

                        categoriaProdutoModel["description"] = "medicamentos";
                        db.insert(categoriaProdutoModel);

                        ProdutoModel produtoModel;

                        produtoModel["category"] = 1;
                        produtoModel["name"] = "papel higienico";
                        produtoModel["description"] = "utensilio";
                        produtoModel["validate"] = "09092017";
                        produtoModel["price"] = 15.42;
                        db.insert(produtoModel);
                      }})
          .build();

      return db;
    };

    UNIQUE(CategoriaProdutoRepository) {
      return new CategoriaProdutoRepository{};
    };

    UNIQUE(ProdutoRepository) { return new ProdutoRepository{}; };
  }

  static void load_interactors() {
    // ProdutoInteractor
    UNIQUE(ProdutoInteractor) {
      return new ProdutoInteractor{jinject::get{}, jinject::get{}};
    };
  }
};
