#pragma once

#include "control/estoque/EstoqueInteractor.hpp"
#include "model/estoque/EstoqueRepository.hpp"
#include "model/produto/ProdutoRepository.hpp"
#include "model/base/cnpj.hpp"
#include "ui/Form.hpp"
#include "ui/Table.hpp"
#include "utils/Log.hpp"

#include <cstdlib>
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

enum class SelecaoEstoque { Exibir = 1, Comprar, Vender };

using namespace jui;

struct EstoqueController {
  inline static const std::string Tag = "EstoqueController";

  EstoqueController(std::unique_ptr<EstoqueInteractor> estoqueInteractor)
    : mEstoqueInteractor{std::move(estoqueInteractor)} {
  }

  bool execute() {
    using namespace jui;

    bool result = true;

    Form<Item<"opcao", "Selecione uma opcao do menu", TypeItem::Int> >{}
      .title("Estoque")
      .before([&]() {
        fmt::print("Escolha uma opção:\n");
        fmt::print("\t{} - Exibir\n",
                   static_cast<int>(SelecaoEstoque::Exibir));
        fmt::print("\t{} - Comprar\n",
                   static_cast<int>(SelecaoEstoque::Comprar));
        fmt::print("\t{} - Vender\n",
                   static_cast<int>(SelecaoEstoque::Vender));
      })
      .on_success([&](Input input) {
        try {
          input.get_int("opcao")
            .and_then([&](auto value) {
              SelecaoEstoque opcao = static_cast<SelecaoEstoque>(value);

              if (opcao == SelecaoEstoque::Exibir) {
                exibir();
              } else if (opcao == SelecaoEstoque::Comprar) {
                comprar();
              } else if (opcao == SelecaoEstoque::Vender) {
                vender();
              }

              return std::optional<bool>{true};
            });
        } catch (std::runtime_error &e) {
          loge(TipoLog::Sistema, Tag, "{}", e.what());

          fmt::print("{}", e.what());
        }

        fmt::print("Pressione qualquer tecla para continuar ...");

        std::cin.ignore();
      })
      .on_failed(opcao_invalida)
      .show();

    return result;
  }

  void exibir() {
    Form<Item<"id", "Id do produto", TypeItem::Int> >{}
      .on_success([&](Input input) {
        auto produtoId = Id::from(input.get_int("id"));

        if (!produtoId) {
          logt(TipoLog::Sistema, Tag, "Id do produto invalido.");

          return;
        }

        auto items = mEstoqueInteractor->load_all() |
                     std::ranges::views::filter([&](auto const &item) {
                       return item.template get<EstoqueModel>("produto_id") ==
                              produtoId->value();
                     });

        logt(TipoLog::Sistema, Tag, "exibindo produto em estoque:[{}]",
             produtoId->value());

        Table{std::move(items)}
          .title("Produto em Estoque")
          .head([]() {
            return std::vector<Col>{
              Col{"id", 15}, Col{"produto", 32},
              Col{"lote", 32}, Col{"quantidade", 32}
            };
          })
          .data([&](auto const &item) {
            auto produto = item.template get<ProdutoModel>();
            auto estoque = item.template get<EstoqueModel>();

            return Row{
              estoque["id"], produto["descricao"], estoque["lote"],
              estoque["quantidade"]
            };
          })
          .show();
      })
      .on_failed(opcao_invalida)
      .show();
  }

  void comprar() {
    Form<Item<"produto", "identificador do produto [usar o id]", TypeItem::Int>,

        Item<"quantidade", "Quantidade do produto (independente da unidade)",
          TypeItem::Int>,
        Item<"lote", "Identificacao do lote", TypeItem::Text>,
        Item<"validade", "Validade do lote do produto", TypeItem::Text>,
        Item<"cnpj", "CNPJ", TypeItem::Text> >{}
      .on_success([&](Input input) {
        EstoqueModel estoque;

        estoque["produto_id"] = input.get_int("produto");
        estoque["quantidade"] = input.get_int("quantidade");
        estoque["lote"] = input.get_text("lote");
        estoque["validade"] = input.get_text("validade");

        auto cnpj = Cnpj::from(input.get_text("cnpj"));

        if (!cnpj) {
          logt(TipoLog::Sistema, Tag, "CNPJ invalido.");

          return;
        }

        logopt(TipoLog::Sistema, Tag, mEstoqueInteractor->save_compra(estoque, *cnpj));
      })
      .on_failed(opcao_invalida)
      .show();
  }

  void vender() {
    Form<Item<"estoque", "identificador do estoque [usar o id]", TypeItem::Int>,
        Item<"quantidade", "Quantidade do produto (independente da unidade)",
          TypeItem::Int>,
        Item<"cnpj", "CNPJ", TypeItem::Text> >{}
      .on_success([&](Input input) {
        auto estoqueId = Id::from(input.get_int("estoque"));
        auto quantidade = input.get_int("quantidade").value();
        auto cnpj = Cnpj::from(input.get_text("cnpj"));

        if (!estoqueId) {
          logt(TipoLog::Sistema, Tag, "Id do estoque invalido.");

          return;
        }

        if (!cnpj) {
          logt(TipoLog::Sistema, Tag, "CNPJ invalido.");

          return;
        }

        logopt(TipoLog::Sistema, Tag, mEstoqueInteractor->store_venda(*estoqueId,
          quantidade, *cnpj));
      })
      .on_failed(opcao_invalida)
      .show();
  }

private:
  std::unique_ptr<EstoqueInteractor> mEstoqueInteractor;
};
