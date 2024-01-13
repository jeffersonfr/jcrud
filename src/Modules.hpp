#pragma once

#include "control/login/LoginController.hpp"
#include "control/admin/AdminController.hpp"
#include "control/produto/ProdutoController.hpp"
#include "implementation/model/SqliteDatabase.hpp"
#include "model/categoria_produto/CategoriaProdutoRepository.hpp"
#include "model/filial/FilialRepository.hpp"
#include "model/log/LogRepository.hpp"
#include "model/level_log/LevelLogRepository.hpp"
#include "model/tipo_log/TipoLogRepository.hpp"
#include "model/login/LoginRepository.hpp"
#include "model/preco/PrecoRepository.hpp"
#include "model/produto/ProdutoRepository.hpp"
#include "model/usuario/UsuarioRepository.hpp"
#include "model/cargo/CargoRepository.hpp"
#include "model/cargo_usuario/CargoUsuarioRepository.hpp"

#include "jinject/jinject.h"

using namespace jinject;

struct Modules
{
  static void load_modules()
  {
    load_log();
    load_sistema();
    load_repositories();
    load_controls();
  }

private:
  static void load_log()
  {
    using MyDatabase = SqliteDatabase<LevelLogModel, TipoLogModel, LogModel>;

    SINGLE(std::shared_ptr<Database>, LogModel) {
      auto db = std::make_shared<MyDatabase>("log.db");

      db->add_migration(Migration{1, [](Database &db) {}}).build();

      return db;
    };

    UNIQUE(LogRepository) {
      return new LogRepository{inject<std::shared_ptr<Database>, LogModel>()};
    };
  }

  static void load_sistema()
  {
    using MyDatabase =
        SqliteDatabase<CategoriaProdutoModel, PrecoModel, ProdutoModel,
                       FilialModel, UsuarioModel, LoginModel, CargoModel, CargoUsuarioModel>;

    SINGLE(std::shared_ptr<Database>) {
      auto db = std::make_shared<MyDatabase>("sistema.db");

      db->add_migration(
          Migration{1, [](Database &db) {
            insert<CategoriaProdutoModel, "descricao">(db)
              .values("higiene")
              .values("limpeza")
              .values("medicamento");

            insert<ProdutoModel, "categoria_id", "nome", "descricao">(db)
              .values(1, "papel higienico", "utensilio")
              .values(3, "anti alergico", "medicamento utilizado para evitar alergias");

            insert<PrecoModel, "produto_id", "valor">(db)
              .values(1, 12.05)
              .values(1, 11.10)
              .values(2, 8.10);

            insert<FilialModel, "nome", "endereco">(db)
              .values("Jeff's Mercado", "Rua Jose Cavalcante, Joao Pessoa, PB");

            insert<CargoModel, "descricao">(db)
              .values("administrador")
              .values("operador");

            char senha[] = "12345678";
            char *senhaEncriptada = crypt(senha, "aa");

            insert<UsuarioModel, "nome", "cpf", "telefone", "email", "senha", "apelido">(db)
              .values("Administrador", "123826331-17", "9 1234 5432", "admin@gmail.com", senhaEncriptada, "admin")
              .values("Jeff Ferr", "123826332-17", "9 1234 5433", "jeffferr@gmail.com", senhaEncriptada, "jeff");

            insert<CargoUsuarioModel, "usuario_id", "cargo_id">(db)
              .values(1, 1)
              .values(1, 2)
              .values(2, 2);

            insert<LoginModel, "filial_id", "usuario_id", "logado">(db)
              .values(1, 1, false)
              .values(1, 2, false);
          }}
        ).build();

      return db;
    };
  }

  static void load_repositories()
  {
    UNIQUE(LoginRepository) {
      return new LoginRepository{inject<std::shared_ptr<Database>>()};
    };

    UNIQUE(CategoriaProdutoRepository) {
      return new CategoriaProdutoRepository{inject<std::shared_ptr<Database>>()};
    };

    UNIQUE(ProdutoRepository) {
      return new ProdutoRepository{inject<std::shared_ptr<Database>>()};
    };

    UNIQUE(PrecoRepository) {
      return new PrecoRepository{inject<std::shared_ptr<Database>>()};
    };

    UNIQUE(FilialRepository) {
      return new FilialRepository{inject<std::shared_ptr<Database>>()};
    };

    UNIQUE(UsuarioRepository) {
      return new UsuarioRepository{inject<std::shared_ptr<Database>>()};
    };

    UNIQUE(CargoRepository) {
      return new CargoRepository{inject<std::shared_ptr<Database>>()};
    };

    UNIQUE(CargoUsuarioRepository) {
      return new CargoUsuarioRepository{inject<std::shared_ptr<Database>>()};
    };
  }

  static void load_controls()
  {
    UNIQUE(LoginInteractor) {
      return new LoginInteractor{};
    };

    UNIQUE(LoginController) {
      return new LoginController{inject<std::unique_ptr<LoginInteractor>>()};
    };

    UNIQUE(AdminInteractor) {
      return new AdminInteractor{get{}, get{}, get{}};
    };

    UNIQUE(AdminController) {
      return new AdminController{get{}, get{}};
    };

    UNIQUE(ProdutoInteractor) {
      return new ProdutoInteractor{get{}, get{}, get{}};
    };

    UNIQUE(ProdutoController) {
      return new ProdutoController{inject<std::unique_ptr<ProdutoInteractor>>()};
    };
  }
};
