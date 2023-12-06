#pragma once

#include "control/produto/ProdutoController.hpp"
#include "ui/widgets/Form.hpp"
#include "Modules.hpp"

#include <fmt/format.h>

enum SelecaoProduto {
  Inserir = 1,
  Listar = 2,
  Exibir = 3,
  Alterar = 4,
  Remover = 5,
  Relatorio = 6,
  Sair= 7
};

struct Sistema {
  Sistema() { 
    Modules::load_modules();
  }

  void execute() {
    using namespace jui;

    do {
      Form<Item<"opcao", "Selecione uma opcao do menu", TypeItem::Int>>{}
          .before([&]() {
            // system("clear");

            fmt::print("Bem-vindo ao sistema da Farmácia Pague++ !\n\n");
            fmt::print("Escolha uma opção:\n");
            fmt::print("\t{} - Inserir produto\n",
                       static_cast<int>(SelecaoProduto::Inserir));
            fmt::print("\t{} - Listar todos os produtos\n",
                       static_cast<int>(SelecaoProduto::Listar));
            fmt::print("\t{} - Exibir um produto (pesquisar por nome)\n",
                       static_cast<int>(SelecaoProduto::Exibir));
            fmt::print("\t{} - Alterar um produto (pesquisar por nome)\n",
                       static_cast<int>(SelecaoProduto::Alterar));
            fmt::print("\t{} - Remover um produto (pesquisar por nome)\n",
                       static_cast<int>(SelecaoProduto::Remover));
            fmt::print("\t{} - Exibir relatório\n",
                       static_cast<int>(SelecaoProduto::Relatorio));
            fmt::print("\t{} - Sair\n", static_cast<int>(SelecaoProduto::Sair));
          })
          .on_success([&](Input input) {
            int opcao = input.get_int("opcao");

            ProdutoController produtoController;

            if (opcao == SelecaoProduto::Inserir) {
              produtoController.inserir();
            } else if (opcao == SelecaoProduto::Listar) {
              produtoController.listar();
            } else if (opcao == SelecaoProduto::Exibir) {
              produtoController.exibir();
            } else if (opcao == SelecaoProduto::Alterar) {
              produtoController.alterar();
            } else if (opcao == SelecaoProduto::Remover) {
              produtoController.remover();
            } else if (opcao == SelecaoProduto::Relatorio) {
              produtoController.relatorio();
            } else if (opcao == SelecaoProduto::Sair) {
              std::exit(0);
            }
          })
          .on_failed(opcao_invalida)
          .show();
    } while (true);
  }
};
