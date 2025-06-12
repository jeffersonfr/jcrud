#pragma once

#include "control/admin/AdminController.hpp"
#include "control/produto/ProdutoController.hpp"
#include "control/estoque/EstoqueController.hpp"
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
    using namespace jui;

    selecaoSet.insert(static_cast<int>(SelecaoSistema::Administracao));
    selecaoSet.insert(static_cast<int>(SelecaoSistema::Produtos));
    selecaoSet.insert(static_cast<int>(SelecaoSistema::Estoque));
    selecaoSet.insert(static_cast<int>(SelecaoSistema::Sair));

    do {
      do_menu();
    } while (true);
  }

private:
  std::unique_ptr<AdminController> mAdminController;
  std::unique_ptr<ProdutoController> mProdutoController;
  std::unique_ptr<EstoqueController> mEstoqueController;
  std::set<int> selecaoSet;

  void do_menu() const {
    Form<Item<"opcao", "Selecione uma opcao do menu", TypeItem::Int> >{}
      .title("Farmacia Pague+ - " + Ambiente::version)
      .before([&]() {
        fmt::print("{}", "Escolha uma opção:\n");

        if (selecaoSet.contains(
          static_cast<int>(SelecaoSistema::Administracao))) {
          fmt::print("\t{} - Administracao\n",
                     static_cast<int>(SelecaoSistema::Administracao));
        }

        if (selecaoSet.contains(static_cast<int>(SelecaoSistema::Produtos))) {
          fmt::print("\t{} - Produtos\n",
                     static_cast<int>(SelecaoSistema::Produtos));
        }

        if (selecaoSet.contains(static_cast<int>(SelecaoSistema::Estoque))) {
          fmt::print("\t{} - Estoque\n",
                     static_cast<int>(SelecaoSistema::Estoque));
        }

        if (selecaoSet.contains(static_cast<int>(SelecaoSistema::Sair))) {
          fmt::print("\t{} - Sair\n",
                     static_cast<int>(SelecaoSistema::Sair));
        }
})
      .on_success([&](Input input) {
        auto opcao = input.get_int("opcao");

        if (!opcao.has_value() or selecaoSet.contains(*opcao) == 0) {
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
