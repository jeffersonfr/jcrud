#pragma once

#include "control/estoque/EstoqueInteractorModel.hpp"
#include "model/estoque/EstoqueRepository.hpp"
#include "model/historicoEstoque/HistoricoEstoqueRepository.hpp"
#include "model/base/cnpj.hpp"

#include <expected>
#include <optional>
#include <ranges>
#include <string>
#include <utility>
#include <vector>

#include "jinject/jinject.h"

struct EstoqueInteractor : public Repository<EstoqueInteractorModel> {
  EstoqueInteractor(
    std::unique_ptr<EstoqueRepository> estoqueRepository,
    std::unique_ptr<HistoricoEstoqueRepository> historicoEstoqueRepository)
    : mEstoqueRepository{std::move(estoqueRepository)},
      mHistoricoEstoqueRepository{std::move(historicoEstoqueRepository)} {
  }

  std::optional<std::string> save_compra(EstoqueModel const &item, Cnpj cnpj) {
    HistoricoEstoqueModel historicoEstoque;

    historicoEstoque["produto_id"] = item["produto_id"];
    historicoEstoque["tipo_negocio_id"] = static_cast<int>(TipoNegocio::Compra);
    historicoEstoque["quantidade"] = item["quantidade"];
    historicoEstoque["lote"] = item["lote"];
    historicoEstoque["cnpj"] = cnpj.value();
    historicoEstoque["validade"] = item["validade"];

    try {
      mEstoqueRepository->get_database()->transaction([&](Database &db) {
        if (auto result = mEstoqueRepository->save(item); !result.has_value()) {
          throw result.error();
        }

        if (auto result = mHistoricoEstoqueRepository->save(historicoEstoque);
          !result.has_value()) {
          throw result.error();
        }
      });
    } catch (std::runtime_error &e) {
      return e.what();
    }

    return {};
  }

  std::optional<std::string> store_venda(Id estoque, int quantidadeVenda, Cnpj cnpj) {
    return load_estoque_by_id(estoque)
      .and_then([&](EstoqueModel const &item) -> std::optional<std::string> {
        HistoricoEstoqueModel historicoEstoque;

        historicoEstoque["produto_id"] = item["produto_id"];
        historicoEstoque["tipo_negocio_id"] = static_cast<int>(TipoNegocio::Venda);
        historicoEstoque["quantidade"] = quantidadeVenda;
        historicoEstoque["lote"] = item["lote"];
        historicoEstoque["cnpj"] = cnpj.value();
        historicoEstoque["validade"] = item["validade"];

        try {
          mEstoqueRepository->get_database()->transaction([&](Database &db) {
            int quantidadeAtual = item["quantidade"].get_int().value();

            if (quantidadeAtual < quantidadeVenda) {
              throw std::runtime_error("Quantidade superior ao limite disponivel");
            } else if (quantidadeAtual == quantidadeVenda) {
              mEstoqueRepository->remove(item);
            } else {
              EstoqueModel estoque = std::move(item);

              estoque["quantidade"] = quantidadeAtual - quantidadeVenda;

              mEstoqueRepository->update(estoque);
            }

            mHistoricoEstoqueRepository->save(historicoEstoque)
              .or_else([](auto e) -> std::expected<HistoricoEstoqueModel, std::runtime_error> {
                throw e;
              });
          });
        } catch (std::runtime_error &e) {
          return e.what();
        }

        return {};
      }).or_else([]() -> std::optional<std::string> {
        return "Estoque id invalido.";
      });
  }

  std::optional<EstoqueModel> load_estoque_by_id(Id id) {
    auto items = mEstoqueRepository->load_by<"id">(id.value());

    for (auto const &item: items) {
      return {item};
    }

    return {};
  }

private:
  std::unique_ptr<EstoqueRepository> mEstoqueRepository;
  std::unique_ptr<HistoricoEstoqueRepository> mHistoricoEstoqueRepository;
};
