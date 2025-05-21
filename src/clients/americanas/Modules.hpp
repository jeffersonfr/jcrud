#pragma once

#include "control/admin/AdminController.hpp"
#include "control/estoque/EstoqueController.hpp"
#include "control/login/LoginController.hpp"
#include "control/produto/ProdutoController.hpp"
#include "model/cargo/CargoRepository.hpp"
#include "model/cargoUsuario/CargoUsuarioRepository.hpp"
#include "model/categoriaProduto/CategoriaProdutoRepository.hpp"
#include "model/estoque/EstoqueRepository.hpp"
#include "model/filial/FilialRepository.hpp"
#include "model/historicoEstoque/HistoricoEstoqueRepository.hpp"
#include "model/levelLog/LevelLogRepository.hpp"
#include "model/log/LogRepository.hpp"
#include "model/login/LoginRepository.hpp"
#include "model/preco/PrecoRepository.hpp"
#include "model/produto/ProdutoRepository.hpp"
#include "model/tipoLog/TipoLogRepository.hpp"
#include "model/usuario/UsuarioRepository.hpp"
#include "clients/americanas/Sistema.hpp"
#include "clients/americanas/strategy/DecorateTitleStrategy.hpp"
#include "database/SqliteDatabase.hpp"

#include "jinject/jinject.h"
#include "model/tipoNegocio/TipoNegocioRepository.hpp"

using namespace jinject;

struct Modules {
  static void load_modules() {
    load_modelo_log();
    load_modelo_sistema();
    load_repositories();
    load_controls();
    load_strategies();
  }

private:
  static void load_modelo_log() {
    using MyDatabase = SqliteDatabase<LevelLogModel, TipoLogModel, LogModel>;

    SINGLE(std::shared_ptr<Database>, LogModel) {
      auto db = std::make_shared<MyDatabase>("log.db");

      db
        ->add_migration(Migration{
          1,
          [](Database &db) {
            insert<LevelLogModel, "descricao">(db)
              .values("Trace")
              .values("Debug")
              .values("Info")
              .values("Warn")
              .values("Error")
              .values("Fatal");

            insert<TipoLogModel, "descricao">(db).values("Sistema").values(
              "Modelo");
          }
        })
        .build();

      return db;
    };

    UNIQUE(LogRepository) {
      return new LogRepository{inject<std::shared_ptr<Database>, LogModel>()};
    };
  }

  static void load_modelo_sistema() {
    using MyDatabase =
      SqliteDatabase<CategoriaProdutoModel, PrecoModel, ProdutoModel,
        FilialModel, UsuarioModel, LoginModel, CargoModel,
        CargoUsuarioModel, EstoqueModel, TipoNegocioModel,
        HistoricoEstoqueModel>;

    SINGLE(std::shared_ptr<Database>) {
      auto db = std::make_shared<MyDatabase>("sistema.db");

      db
        ->add_migration(Migration{
          1,
          [](Database &db) {
            insert<CategoriaProdutoModel, "descricao">(db)
              .values("Higiene")
              .values("Limpeza")
              .values("Medicamento");

            insert<ProdutoModel, "categoria_id", "nome", "descricao">(db)
              .values(1, "papel higienico", "utensilio")
              .values(3, "anti alergico",
                      "medicamento utilizado para evitar alergias");

            insert<PrecoModel, "produto_id", "valor">(db)
              .values(1, 12.05)
              .values(1, 11.10)
              .values(2, 8.10);

            insert<FilialModel, "nome", "endereco">(db).values(
              "Jeff's Mercado", "Rua Jose Cavalcante, Joao Pessoa, PB");

            insert<CargoModel, "descricao">(db)
              .values("administrador")
              .values("operador");

            char senha[] = "12345678";
            char *senhaEncriptada = crypt(senha, "aa");

            insert<UsuarioModel, "nome", "cpf", "telefone", "email",
                "senha", "apelido">(db)
              .values("Administrador", "123826331-17", "9 1234 5432",
                      "admin@gmail.com", senhaEncriptada, "admin")
              .values("Jeff Ferr", "123826332-17", "9 1234 5433",
                      "jeffferr@gmail.com", senhaEncriptada, "jeff");

            insert<CargoUsuarioModel, "usuario_id", "cargo_id">(db)
              .values(1, 1)
              .values(1, 2)
              .values(2, 2);

            insert<LoginModel, "filial_id", "usuario_id", "logado">(db)
              .values(1, 1, false)
              .values(1, 2, false);

            insert<TipoNegocioModel, "descricao">(db)
              .values("Compra")
              .values("Venda");
          }
        })
        .build();

      return db;
    };
  }

  static void load_repositories() {
    UNIQUE(LoginRepository) {
      return new LoginRepository{inject<std::shared_ptr<Database> >()};
    };

    UNIQUE(CategoriaProdutoRepository) {
      return new CategoriaProdutoRepository{
        inject<std::shared_ptr<Database> >()
      };
    };

    UNIQUE(ProdutoRepository) {
      return new ProdutoRepository{inject<std::shared_ptr<Database> >()};
    };

    UNIQUE(PrecoRepository) {
      return new PrecoRepository{inject<std::shared_ptr<Database> >()};
    };

    UNIQUE(FilialRepository) {
      return new FilialRepository{inject<std::shared_ptr<Database> >()};
    };

    UNIQUE(UsuarioRepository) {
      return new UsuarioRepository{inject<std::shared_ptr<Database> >()};
    };

    UNIQUE(CargoRepository) {
      return new CargoRepository{inject<std::shared_ptr<Database> >()};
    };

    UNIQUE(CargoUsuarioRepository) {
      return new CargoUsuarioRepository{inject<std::shared_ptr<Database> >()};
    };

    UNIQUE(EstoqueRepository) {
      return new EstoqueRepository{inject<std::shared_ptr<Database> >()};
    };

    UNIQUE(TipoNegocioRepository) {
      return new TipoNegocioRepository{inject<std::shared_ptr<Database> >()};
    };

    UNIQUE(HistoricoEstoqueRepository) {
      return new HistoricoEstoqueRepository{
        inject<std::shared_ptr<Database> >()
      };
    };
  }

  static void load_controls() {
    // repository
    UNIQUE(ProdutoInteractorRepository) {
      return new ProdutoInteractorRepository{};
    };

    UNIQUE(EstoqueInteractorRepository) {
      return new EstoqueInteractorRepository{};
    };

    // interactor
    UNIQUE(AdminInteractor) {
      return new AdminInteractor{get{}, get{}, get{}};
    };

    UNIQUE(LoginInteractor) {
      return new LoginInteractor{inject<std::unique_ptr<LoginInteractorRepository>>()};
    };

    UNIQUE(ProdutoInteractor) {
      return new ProdutoInteractor{get{}, get{}, get{}, get{}};
    };

    UNIQUE(EstoqueInteractor) {
      return new EstoqueInteractor{get{}, get{}, get{}};
    };

    // controller
    UNIQUE(LoginController) {
      return new LoginController{inject<std::unique_ptr<LoginInteractor> >()};
    };

    UNIQUE(AdminController) { return new AdminController{get{}, get{}}; };

    UNIQUE(ProdutoController) {
      return new ProdutoController{
        inject<std::unique_ptr<ProdutoInteractor> >()
      };
    };

    UNIQUE(EstoqueController) {
      return new EstoqueController{
        inject<std::unique_ptr<EstoqueInteractor> >()
      };
    };

    // system
    UNIQUE(Sistema) {
      return new Sistema{get{}, get{}, get{}};
    };
  }

  static void load_strategies() {
    UNIQUE(DecorateTitleStrategy) { return new DecorateTitleStrategyAmericanas{}; };
  }
};
