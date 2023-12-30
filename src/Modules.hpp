#pragma once

#include "control/produto/ProdutoInteractor.hpp"
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
    load_repositories();
    load_controls();
  }

private:
  static void load_log() {
    using MyDatabase = SqliteDatabase<LogModel>;

    SINGLE(std::shared_ptr<Database>, LogModel) {
      auto db = std::make_shared<MyDatabase>("log.db");

      db->add_migration(Migration{1,
                                  [](Database &db) {
                                  }})
          .build();

      return db;
    };

    UNIQUE(LogRepository) { return new LogRepository{get{}}; };
  }

  static void load_sistema() {
    using MyDatabase =
        SqliteDatabase<CategoriaProdutoModel, PrecoModel, ProdutoModel>;

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
                        db.insert(produtoModel);

                        produtoModel["categoria_id"] = 3;
                        produtoModel["nome"] = "antialergico";
                        produtoModel["descricao"] = "medicamento utilizado para evitar alergias";
                        produtoModel["validade"] = "09152020";
                        db.insert(produtoModel);

                        PrecoModel precoModel;
                        precoModel["produto_id"] = 1;
                        precoModel["valor"] = 12.05;
                        precoModel["timestamp"] = "01012020";
                        db.insert(precoModel);

                        precoModel["produto_id"] = 1;
                        precoModel["valor"] = 11.16;
                        precoModel["timestamp"] = "02022022";
                        db.insert(precoModel);

                        precoModel["produto_id"] = 2;
                        precoModel["valor"] = 8.10;
                        precoModel["timestamp"] = "03032023";
                        db.insert(precoModel);
                      }})
          .build();

      return db;
    };
  }

  static void load_repositories() {
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

  static void load_controls() {
    UNIQUE(ProdutoInteractor) {
      return new ProdutoInteractor{get{}, get{}, get{}};
    };

    UNIQUE(ProdutoController) {
      return new ProdutoController{inject<std::unique_ptr<ProdutoInteractor>>()};
    };
  }
};
