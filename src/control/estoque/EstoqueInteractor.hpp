#pragma once

#include "control/estoque/EstoqueInteractorModel.hpp"
#include "model/estoque/EstoqueRepository.hpp"
#include "model/historicoEstoque/HistoricoEstoqueRepository.hpp"

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
        mHistoricoEstoqueRepository{std::move(historicoEstoqueRepository)} {}

  void save_compra(EstoqueModel const &item, std::string cnpj) {
    HistoricoEstoqueModel historicoEstoque;

    historicoEstoque["produto_id"] = item["produto_id"];
    historicoEstoque["tipo_negocio_id"] = static_cast<int>(TipoNegocio::Compra);
    historicoEstoque["quantidade"] = item["quantidade"];
    historicoEstoque["lote"] = item["lote"];
    historicoEstoque["cnpj"] = cnpj;
    historicoEstoque["validade"] = item["validade"];

    mEstoqueRepository->get_database()->transaction([&](Database &db) {
      auto estoqueE = mEstoqueRepository->save(item);

      if (!estoqueE.has_value()) {
        throw estoqueE.error();
      }

      auto historicoEstoqueE =
          mHistoricoEstoqueRepository->save(historicoEstoque);

      if (!historicoEstoqueE.has_value()) {
        throw historicoEstoqueE.error();
      }
    });
  }

  void save_venda(EstoqueModel const &item, int quantidadeVenda, std::string cnpj) {
    HistoricoEstoqueModel historicoEstoque;

    historicoEstoque["produto_id"] = item["produto_id"];
    historicoEstoque["tipo_negocio_id"] = static_cast<int>(TipoNegocio::Venda);
    historicoEstoque["quantidade"] = quantidadeVenda;
    historicoEstoque["lote"] = item["lote"];
    historicoEstoque["cnpj"] = cnpj;
    historicoEstoque["validade"] = item["validade"];

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

      auto e = mHistoricoEstoqueRepository->save(historicoEstoque);

      if (!e.has_value()) {
        throw e.error();
      }
    });
  }

  std::optional<EstoqueModel> load_estoque_by_id(int64_t id) {
    auto items = mEstoqueRepository->load_by<"id">(id);

    for (auto const &item : items) {
      return {item};
    }

    return {};
  }

private:
  std::unique_ptr<EstoqueRepository> mEstoqueRepository;
  std::unique_ptr<HistoricoEstoqueRepository> mHistoricoEstoqueRepository;
};
