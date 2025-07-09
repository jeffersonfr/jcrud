#pragma once

#include "control/admin/AdminController.hpp"
#include "control/estoque/EstoqueController.hpp"
#include "control/produto/ProdutoController.hpp"
#include "model/estoque/EstoqueRepository.hpp"
#include "model/filial/FilialRepository.hpp"
#include "model/historicoEstoque/HistoricoEstoqueRepository.hpp"
#include "model/login/LoginRepository.hpp"
#include "model/preco/PrecoRepository.hpp"
#include "model/produto/ProdutoRepository.hpp"
#include "model/tipoNegocio/TipoNegocioRepository.hpp"
#include "database/SqliteDatabase.hpp"

#if JCRUD_CLIENT == Americanas
#include "clients/americanas/Sistema.hpp"
#else
#include "Sistema.hpp"
#endif

void sistema_module() {
  using namespace jinject;

  using MyDatabase =
      SqliteDatabase<CategoriaProdutoModel, PrecoModel, ProdutoModel,
        FilialModel, UsuarioModel, LoginModel, CargoModel,
        CargoUsuarioModel, EstoqueModel, TipoNegocioModel,
        HistoricoEstoqueModel>;

  SINGLE(std::shared_ptr<Database>) {
    auto db = std::make_shared<MyDatabase>("sistema.db");

    db->add_migration(Migration{
          1, [](Database &db) {
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

#if JCRUD_CLIENT == Americanas
  UNIQUE(Sistema) {
    return new Sistema{get{}, get{}, get{}};
  };
#else
    UNIQUE(Sistema) {
        return new Sistema{get{}, get{}, get{}, get{}};
    };
#endif
}
