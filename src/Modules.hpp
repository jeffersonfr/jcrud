#pragma once

#include "control/produto/interactor/ProdutoInteractor.hpp"
#include "implementation/model/SqliteDatabase.hpp"
#include "model/categoria_produto/CategoriaProdutoRepository.hpp"
#include "model/log/LogRepository.hpp"
#include "model/preco/PrecoRepository.hpp"
#include "model/produto/ProdutoRepository.hpp"

#include "jinject/jinject.h"

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

    UNIQUE(LogRepository) { return new LogRepository{get{}}; };
  }

  static void load_sistema() {
    using MyDatabase =
        SqliteDatabase<CategoriaProdutoModel, PrecoModel, ProdutoModel>;

    // Database/sistema_crud
    SINGLE(std::shared_ptr<Database>) {
      auto db = std::make_shared<MyDatabase>("sistema.db");

      db->add_migration(
            Migration{1,
                      [](Database &db) {
                        CategoriaProdutoModel categoriaProdutoModel;
                        categoriaProdutoModel["descricao"] = "higiene";
                        db.insert(categoriaProdutoModel);
                        categoriaProdutoModel["descricao"] = "limpeza";
                        db.insert(categoriaProdutoModel);
                        categoriaProdutoModel["descricao"] = "medicamentos";
                        db.insert(categoriaProdutoModel);

                        ProdutoModel produtoModel;
                        produtoModel["categoria_id"] = 1;
                        produtoModel["nome"] = "papel higienico";
                        produtoModel["descricao"] = "utensilio";
                        produtoModel["validade"] = "09092017";
                        produtoModel["preco_id"] = 1;
                        db.insert(produtoModel);

                        PrecoModel precoModel;
                        precoModel["produto_id"] = 1;
                        precoModel["valor"] = 12.05;
                        db.insert(precoModel);
                      }})
          .build();

      return db;
    };

    UNIQUE(CategoriaProdutoRepository) {
      return new CategoriaProdutoRepository{
          inject<std::shared_ptr<Database>>()};
    };

    UNIQUE(ProdutoRepository) {
      return new ProdutoRepository{inject<std::shared_ptr<Database>>()};
    };

    UNIQUE(PrecoRepository) {
      return new PrecoRepository{inject<std::shared_ptr<Database>>()};
    };
  }

  static void load_interactors() {
    // ProdutoInteractor
    UNIQUE(ProdutoInteractor) {
      return new ProdutoInteractor{get{}, get{}, get{}};
    };
  }
};
