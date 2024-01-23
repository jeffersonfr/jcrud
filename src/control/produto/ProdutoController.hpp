#pragma once

#include "control/produto/ProdutoInteractor.hpp"
#include "model/categoria_produto/CategoriaProdutoRepository.hpp"
#include "model/produto/ProdutoRepository.hpp"
#include "ui/Form.hpp"
#include "ui/Table.hpp"
#include "utils/Format.hpp"
#include "utils/Log.hpp"

#include <cstdlib>
#include <fstream>
#include <functional>
#include <iostream>
#include <optional>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

#include <fmt/format.h>

#include "jinject/jinject.h"

enum class SelecaoProduto {
  Inserir = 1,
  Listar,
  Exibir,
  Alterar,
  Remover,
  Relatorio
};

using namespace jui;

struct ProdutoController {
  inline static const std::string Tag = "ProdutoController";

  ProdutoController(std::unique_ptr<ProdutoInteractor> produtoInteractor)
      : mProdutoInteractor{std::move(produtoInteractor)} {}

  bool execute() {
    using namespace jui;

    bool result = true;

    Form<Item<"opcao", "Selecione uma opcao do menu", TypeItem::Int>>{}
        .title("Produtos")
        .before([&]() {
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
        })
        .on_success([&](Input input) {
          auto opcao = input.get_int("opcao");

          if (!opcao.has_value()) {
            return;
          }

          if (*opcao == static_cast<int>(SelecaoProduto::Inserir)) {
            inserir();
          } else if (*opcao == static_cast<int>(SelecaoProduto::Listar)) {
            listar();
          } else if (*opcao == static_cast<int>(SelecaoProduto::Exibir)) {
            exibir();
          } else if (*opcao == static_cast<int>(SelecaoProduto::Alterar)) {
            alterar();
          } else if (*opcao == static_cast<int>(SelecaoProduto::Remover)) {
            remover();
          } else if (*opcao == static_cast<int>(SelecaoProduto::Relatorio)) {
            relatorio();
          }

          fmt::print("Pressione qualquer tecla para continuar ...");

          std::cin.ignore();
        })
        .on_failed(opcao_invalida)
        .show();

    return result;
  }

  void inserir() {
    Table{mProdutoInteractor->load_all_categorias()}
        .title("Listagem de categorias")
        .head([]() {
          return std::vector<Col>{Col{"id", 15}, Col{"nome", 32}};
        })
        .data([&](auto const &item) {
          return Row{item["id"], item["descricao"]};
        })
        .show();

    Form<Item<"categoria", "Categoria do produto [usar o id]", TypeItem::Int>,
         Item<"nome", "Nome do produto", TypeItem::Text>,
         Item<"descricao", "Descricao do produto", TypeItem::Text>,
         Item<"validade", "Data no formato 'ddmmaaaa'", TypeItem::Text>,
         Item<"preco", "Preco do produto", TypeItem::Decimal>>{}
        .on_success([&](Input input) {
          ProdutoInteractorModel item;
          ProdutoModel &produto = item.template get<ProdutoModel>();
          PrecoModel &preco = item.template get<PrecoModel>();

          produto["categoria_id"] = input.get_int("categoria");
          produto["nome"] = input.get_text("nome");
          produto["descricao"] = input.get_text("descricao");
          produto["validade"] = input.get_text("validade");

          preco["valor"] = input.get_decimal("preco");

          mProdutoInteractor->save_produto(item);

          logt(TipoLog::Sistema, Tag, "inserindo produto:[{}]", item);
        })
        .on_failed(opcao_invalida)
        .show();
  }

  void listar() { listar_produtos(mProdutoInteractor->load_all_produtos()); }

  void exibir() {
    Form<Item<"id", "Id do produto", TypeItem::Int>>{}
        .on_success([&](Input input) {
          auto produtoId = input.get_int("id");

          if (!produtoId.has_value()) {
            fmt::print("Identificador do produto invalido");

            return;
          }

          mProdutoInteractor->load_produto_by_id(produtoId.value())
              .and_then([&](auto produto) {
                auto historicoPrecos =
                    mProdutoInteractor->load_historico_precos(
                        produto.template get<ProdutoModel>("id")
                            .get_int()
                            .value());

                Table{historicoPrecos}
                    .title("Historico de precos")
                    .head([]() {
                      return std::vector<Col>{Col{"id", 15}, Col{"valor", 32},
                                              Col{"data", 32}};
                    })
                    .data([&](auto const &item) {
                      return Row{item["id"], format_currency(item["valor"]),
                                 item["last"]};
                    })
                    .show();

                logt(TipoLog::Sistema, Tag, "exibindo produto:[{}]", produto);
      
                return std::optional<bool>{true};
              });
        })
        .on_failed(opcao_invalida)
        .show();
  }

  void alterar() {
    Form<Item<"id", "Id do produto", TypeItem::Int>>{}
        .on_success([&](Input input) {
          auto produtoId = input.get_int("id");

          if (!produtoId.has_value()) {
            fmt::print("Identificador do produto invalido");

            return;
          }

          mProdutoInteractor->load_produto_by_id(produtoId.value())
              .and_then([&](auto item) -> std::optional<bool> {
                Form<Item<"nome", "Nome do produto", TypeItem::Text>,
                     Item<"descricao", "Descricao do produto", TypeItem::Text>,
                     Item<"preco", "Preco do produto", TypeItem::Decimal>>{}
                    .set("nome", item.template get<ProdutoModel>("nome"))
                    .set("descricao",
                         item.template get<ProdutoModel>("descricao"))
                    .set("preco", item.template get<PrecoModel>("valor"))
                    .on_success([&](Input input) {
                      item.template get<ProdutoModel>("nome") =
                          input.get_text("nome");
                      item.template get<ProdutoModel>("descricao") =
                          input.get_text("descricao");
                      item.template get<PrecoModel>("valor") =
                          input.get_decimal("preco");

                      mProdutoInteractor->save_produto(item);
      
                      logt(TipoLog::Sistema, Tag, "alterando produto:[{}]", item);
                    })
                    .on_failed(opcao_invalida)
                    .show();

                return {};
              });
        })
        .on_failed(opcao_invalida)
        .show();
  }

  void remover() {
    Form<Item<"id", "Id do produto", TypeItem::Int>>{}
        .on_success([&](Input input) {
          auto produtoId = input.get_int("id");

          if (!produtoId.has_value()) {
            fmt::print("Identificador do produto invalido");

            return;
          }

          mProdutoInteractor->load_produto_by_id(produtoId.value())
              .and_then([&](auto item) -> std::optional<bool> {
                mProdutoInteractor->remove_produto(item);

                logt(TipoLog::Sistema, Tag, "removendo produto:[{}]", item);

                return {};
              });
        })
        .on_failed(opcao_invalida)
        .show();
  }

  void relatorio() {
    Table{mProdutoInteractor->load_all_categorias()}
        .title("Listagem de categorias")
        .head([]() {
          return std::vector<Col>{Col{"id", 15}, Col{"nome", 32}};
        })
        .data([&](auto const &item) {
          return Row{item["id"], item["descricao"]};
        })
        .show();

    listar();
  }

private:
  std::unique_ptr<ProdutoInteractor> mProdutoInteractor;

  void listar_produtos(std::vector<ProdutoInteractorModel> &&items) {
    Table{items}
        .title("Listagem de produtos")
        .head([]() {
          return std::vector<Col>{{"id", 10},       {"categoria", 15},
                                  {"nome", 32},     {"descricao", 32},
                                  {"validade", 15}, {"preco", 15}};
        })
        .data([&](auto const &item) {
          auto categoriaProduto = item.template get<CategoriaProdutoModel>();
          auto produto = item.template get<ProdutoModel>();
          auto preco = item.template get<PrecoModel>();

          return Row{produto["id"],       categoriaProduto["descricao"],
                     produto["nome"],     produto["descricao"],
                     produto["validade"], format_currency(preco["valor"])};
        })
        .show();

        logt(TipoLog::Sistema, Tag, "listando produtos");
  }
};
