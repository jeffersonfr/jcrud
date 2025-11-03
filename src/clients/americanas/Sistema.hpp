#pragma once

#include "control/admin/AdminController.hpp"
#include "control/produto/ProdutoController.hpp"
#include "control/estoque/EstoqueController.hpp"
#include "control/login/LoginController.hpp"
#include "api/Routes.hpp"
#include "api/v1/routes/BasicRoutes.hpp"
#include "api/v1/routes/LoginRoutes.hpp"
#include "ui/Form.hpp"
#include "utils/Log.hpp"
#include "Ambiente.hpp"

#include <fmt/format.h>

enum class SelecaoSistema { Administracao = 1, Produtos, Estoque, Sair };

struct Sistema {
  inline static std::string const Tag = "Sistema";

  Sistema(std::unique_ptr<AdminController> adminController,
          std::unique_ptr<ProdutoController> produtoController,
          std::unique_ptr<EstoqueController> estoqueController)
    : mAdminController{std::move(adminController)},
      mProdutoController{std::move(produtoController)},
      mEstoqueController{std::move(estoqueController)} {
    Log::instance().level(LevelLog::Trace);
  }

  void execute() {
    auto routes = create_routes(
      v1::LoginRoutes{}, v1::BasicRoutes{});

    routes->start();

    selecaoSet = std::set{
      SelecaoSistema::Administracao,
      SelecaoSistema::Produtos,
      SelecaoSistema::Estoque,
      SelecaoSistema::Sair
    };

    do {
      do_menu();
    } while (true);
  }

private:
  std::unique_ptr<AdminController> mAdminController;
  std::unique_ptr<ProdutoController> mProdutoController;
  std::unique_ptr<EstoqueController> mEstoqueController;
  std::set<SelecaoSistema> selecaoSet;

  void do_menu() const {
    Form<Item<"opcao", "Selecione uma opcao do menu", TypeItem::Int> >{}
        .title("Farmacia Pague+ - " + Ambiente::version)
        .before([&]() {
          fmt::print("{}", "Escolha uma opção:\n");

          if (selecaoSet.contains(SelecaoSistema::Administracao)) {
            fmt::print("\t{} - Administracao\n", static_cast<int>(SelecaoSistema::Administracao));
          }

          if (selecaoSet.contains(SelecaoSistema::Produtos)) {
            fmt::print("\t{} - Produtos\n", static_cast<int>(SelecaoSistema::Produtos));
          }

          if (selecaoSet.contains(SelecaoSistema::Estoque)) {
            fmt::print("\t{} - Estoque\n", static_cast<int>(SelecaoSistema::Estoque));
          }

          if (selecaoSet.contains(SelecaoSistema::Sair)) {
            fmt::print("\t{} - Sair\n", static_cast<int>(SelecaoSistema::Sair));
          }
        })
        .on_success([&](Input input) {
          auto opcao = input.get_int("opcao").transform([](auto value) { return static_cast<SelecaoSistema>(value); });

          if (!opcao or !selecaoSet.contains(*opcao)) {
            return;
          }

          if (opcao == SelecaoSistema::Administracao) {
            mAdminController->execute();
          } else if (opcao == SelecaoSistema::Produtos) {
            mProdutoController->execute();
          } else if (opcao == SelecaoSistema::Estoque) {
            mEstoqueController->execute();
          } else if (opcao == SelecaoSistema::Sair) {
            std::exit(0);
          }
        })
        .on_failed(opcao_invalida)
        .interruptable()
        .show();
  }
};
