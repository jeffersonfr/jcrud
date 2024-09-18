#pragma once

#include "control/admin/AdminController.hpp"
#include "control/produto/ProdutoController.hpp"
#include "control/estoque/EstoqueController.hpp"
#include "ui/Form.hpp"
#include "utils/Log.hpp"

#include <format>

enum class SelecaoSistema { Administracao = 1, Produtos, Estoque };

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

    do {
        do_menu();
    } while (true);
  }

private:
  std::unique_ptr<AdminController> mAdminController;
  std::unique_ptr<ProdutoController> mProdutoController;
  std::unique_ptr<EstoqueController> mEstoqueController;
  std::set<int> selecaoSet;

  void do_menu() {
    Form<Item<"opcao", "Selecione uma opcao do menu", TypeItem::Int>>{}
        .title("Farmacia Pague+")
        .before([&]() {
          std::print("{}", "Escolha uma opção:\n");

          if (selecaoSet.count(
                  static_cast<int>(SelecaoSistema::Administracao))) {
            std::print("\t{} - Administracao\n",
                       static_cast<int>(SelecaoSistema::Administracao));
          }

          if (selecaoSet.count(static_cast<int>(SelecaoSistema::Produtos))) {
            std::print("\t{} - Produtos\n",
                       static_cast<int>(SelecaoSistema::Produtos));
          }

          if (selecaoSet.count(static_cast<int>(SelecaoSistema::Estoque))) {
            std::print("\t{} - Estoque\n",
                       static_cast<int>(SelecaoSistema::Estoque));
          }
        })
        .on_success([&](Input input) {
          auto opcao = input.get_int("opcao");

          if (!opcao.has_value() or selecaoSet.count(*opcao) == 0) {
            return;
          }

          if (*opcao == static_cast<int>(SelecaoSistema::Administracao)) {
            mAdminController->execute();
          } else if (*opcao == static_cast<int>(SelecaoSistema::Produtos)) {
            mProdutoController->execute();
          } else if (*opcao == static_cast<int>(SelecaoSistema::Estoque)) {
            mEstoqueController->execute();
          }
        })
        .on_failed(opcao_invalida)
        .interruptable()
        .show();
  }
};
