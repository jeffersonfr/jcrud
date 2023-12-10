#pragma once

#include "control/produto/interactor/ProdutoInteractor.hpp"
#include "model/categoria_produto/CategoriaProdutoRepository.hpp"
#include "model/produto/ProdutoRepository.hpp"
#include "ui/widgets/Form.hpp"
#include "ui/widgets/Table.hpp"

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

using namespace jui;

void opcao_invalida() { fmt::print("Opcao Invalida !\n"); }

struct ProdutoController {
  void inserir() {
    Form<
        Item<
            "categoria",
            "Categoria do produto [1:medicamento, 2:beleza, 3:higiene pessoal]",
            TypeItem::Int>,
        Item<"nome", "Nome do produto", TypeItem::Text>,
        Item<"descricao", "Descricao do produto", TypeItem::Text>,
        Item<"validade", "Data no formato 'ddmmaaaa'", TypeItem::Text>,
        Item<"preco", "Preco do produto", TypeItem::Int>>{}
        .on_success([&](Input input) {
          DataClass<"categoria_produto", PrimaryKeys<"id">,
                    Field<"id", FieldType::Int, false>,
                    Field<"descricao", FieldType::Text>>{};
          ProdutoModel item;

          item["categoria"] = input.get_int("categoria");
          item["nome"] = input.get_text("nome");
          item["descricao"] = input.get_text("descricao");
          item["validade"] = input.get_text("validade");
          item["preco_id"] = input.get_decimal("preco");

          mProdutoInteractor->save_produto(item);
        })
        .on_failed(opcao_invalida)
        .show();
  }

  void listar() {
    Table{mProdutoInteractor->load_all_categorias()}
        .title("Listagem de categorias")
        .head([]() {
          return std::vector<Col>{Col{"id", 15}, Col{"nome", 32}};
        })
        .data([&](auto const &item) {
          return Row{item["id"], item["descricao"]};
        })
        .show();

    Table{mProdutoInteractor->load_all_produtos()}
        .title("Listagem de produtos")
        .head([]() {
          return std::vector<Col>{{"id", 15},
                                  {"nome", 32},
                                  {"descricao", 32},
                                  {"validade", 15},
                                  {"preco", 15}};
        })
        .data([&](auto const &item) {
          auto categoriaProduto = item.template get<CategoriaProdutoModel>();
          auto produto = item.template get<ProdutoModel>();

          return Row{categoriaProduto["descricao"], produto["nome"],
                     produto["descricao"], produto["validade"],
                     produto["preco_id"]};
        })
        .show();
  }

  void exibir() {
    listar();

    Form<Item<"nome", "Nome do produto", TypeItem::Text>>{}
        .on_success([&](Input input) {
          auto item =
              mProdutoInteractor->load_produto_by_name(input.get_text("nome"));

          if (item) {
            fmt::print("Item encontrado !\n");
          } else {
            fmt::print("Item inexistente !\n");
          }
        })
        .on_failed(opcao_invalida)
        .show();
  }

  void alterar() {
    listar();

    Form<Item<"nome", "Nome do produto", TypeItem::Text>>{}
        .on_success([&](Input input) {
          auto item =
              mProdutoInteractor->load_produto_by_name(input.get_text("nome"));

          if (item) {
            fmt::print("Item encontrado !\n");

            mProdutoInteractor->save_produto(*item);
          } else {
            fmt::print("Item inexistente !\n");
          }
        })
        .on_failed(opcao_invalida)
        .show();
  }

  void remover() {
    listar();

    Form<Item<"nome", "Nome do produto", TypeItem::Text>>{}
        .on_success([&](Input input) {
          auto item =
              mProdutoInteractor->load_produto_by_name(input.get_text("nome"));

          if (item) {
            fmt::print("Item encontrado !\n");

            mProdutoInteractor->remove_produto(*item);
          } else {
            fmt::print("Item inexistente !\n");
          }
        })
        .on_failed(opcao_invalida)
        .show();
  }

  void relatorio() { listar(); }

private:
  std::unique_ptr<ProdutoInteractor> mProdutoInteractor = jinject::get{};
};
