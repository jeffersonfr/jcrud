#pragma once

#include "control/estoque/EstoqueInteractorModel.hpp"
#include "model/estoque/EstoqueRepository.hpp"
#include "model/historicoEstoque/HistoricoEstoqueRepository.hpp"
#include "control/estoque/EstoqueInteractorRepository.hpp"
#include "model/base/cnpj.hpp"

#include <optional>
#include <string>
#include <utility>

struct EstoqueInteractor {
  EstoqueInteractor(
    std::unique_ptr<EstoqueInteractorRepository> estoqueInteractorRepository,
    std::unique_ptr<EstoqueRepository> estoqueRepository,
    std::unique_ptr<HistoricoEstoqueRepository> historicoEstoqueRepository)
    : mEstoqueInteractorRepository{std::move(estoqueInteractorRepository)},
      mEstoqueRepository{std::move(estoqueRepository)},
      mHistoricoEstoqueRepository{std::move(historicoEstoqueRepository)} {
  }

  std::vector<EstoqueInteractorModel> load_all() {
    return mEstoqueInteractorRepository->load_all();
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
          throw std::move(result.error());
        }

        if (auto result = mHistoricoEstoqueRepository->save(historicoEstoque);
          !result.has_value()) {
          throw std::move(result.error());
        }
      });
    } catch (std::runtime_error &e) {
      return e.what();
    }

    return {};
  }

  std::optional<std::string> store_venda(Id estoque, int quantidadeVenda, Cnpj cnpj) {
    return load_estoque_by_id(estoque)
      .and_then([&](EstoqueModel item) -> std::optional<std::string> {
        auto quantidadeAtual = item["quantidade"].get_int().value();

        if (quantidadeAtual < quantidadeVenda) {
          return ErrorMsg::QUANTIDADE_INVALIDA;
        }

        HistoricoEstoqueModel historicoEstoque;

        historicoEstoque["produto_id"] = item["produto_id"];
        historicoEstoque["tipo_negocio_id"] = static_cast<int>(TipoNegocio::Venda);
        historicoEstoque["quantidade"] = quantidadeVenda;
        historicoEstoque["lote"] = item["lote"];
        historicoEstoque["cnpj"] = cnpj.value();
        historicoEstoque["validade"] = item["validade"];

        try {
          mEstoqueRepository->get_database()->transaction([&](Database &db) {
            if (quantidadeAtual == quantidadeVenda) {
              mEstoqueRepository->remove(item);
            } else {
              item["quantidade"] = quantidadeAtual - quantidadeVenda;

              mEstoqueRepository->update(item);
            }

            if (auto result = mHistoricoEstoqueRepository->save(historicoEstoque); !result) {
              throw std::move(result.error());
            }
          });
        } catch (std::runtime_error &e) {
          return e.what();
        }

        return {};
      })
      .or_else([]() -> std::optional<std::string> {
        return ErrorMsg::ID_ESTOQUE_INVALIDO;
      });
  }

  std::optional<EstoqueModel> load_estoque_by_id(Id id) {
    auto items = mEstoqueRepository->load_by<"id">(id.value());

    if (items.empty()) {
      return {};
    }

    return items[0];
  }

private:
  std::unique_ptr<EstoqueInteractorRepository> mEstoqueInteractorRepository;
  std::unique_ptr<EstoqueRepository> mEstoqueRepository;
  std::unique_ptr<HistoricoEstoqueRepository> mHistoricoEstoqueRepository;
};
