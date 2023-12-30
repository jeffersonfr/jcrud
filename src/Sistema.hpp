#pragma once

#include "control/produto/ProdutoController.hpp"
#include "ui/Form.hpp"
#include "Modules.hpp"
#include "Format.hpp"

#include <fmt/format.h>

enum SelecaoProduto
{
  Inserir = 1,
  Listar = 2,
  Exibir = 3,
  Alterar = 4,
  Remover = 5,
  Relatorio = 6,
  Sair = 7
};

struct Sistema
{
  Sistema()
  {
    Modules::load_modules();
  }

  void execute()
  {
    using namespace jui;

  ProdutoInteractorModel m;
  Repository<ProdutoInteractorModel> r{inject<std::shared_ptr<Database>>()};
  r.load_all();

    do
    {
      Form<Item<"opcao", "Selecione uma opcao do menu", TypeItem::Int>>{}
          .before([&]()
                  {
            system("clear");

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
            fmt::print("\t{} - Sair\n", static_cast<int>(SelecaoProduto::Sair)); })
          .on_success([&](Input input)
                      {
            auto opcao = input.get_int("opcao");

            if (!opcao) {
              fmt::print("Opcao invalida");

              return;
            }

            std::unique_ptr<ProdutoController> produtoController = get{};

            if (*opcao == SelecaoProduto::Inserir) {
              produtoController->inserir();
            } else if (*opcao == SelecaoProduto::Listar) {
              produtoController->listar();
            } else if (*opcao == SelecaoProduto::Exibir) {
              produtoController->exibir();
            } else if (*opcao == SelecaoProduto::Alterar) {
              produtoController->alterar();
            } else if (*opcao == SelecaoProduto::Remover) {
              produtoController->remover();
            } else if (*opcao == SelecaoProduto::Relatorio) {
              produtoController->relatorio();
            } else if (*opcao == SelecaoProduto::Sair) {
              std::exit(0);
            }

            fmt::print("Pressione qualquer tecla para continuar ...");
            
            std::cin.ignore(); })
          .on_failed(opcao_invalida)
          .show();
    } while (true);
  }
};
