#pragma once

#include "control/produto/interactor/ProdutoInteractorModel.hpp"
#include "model/categoria_produto/CategoriaProdutoRepository.hpp"
#include "model/produto/ProdutoRepository.hpp"
#include "model/preco/PrecoRepository.hpp"

#include <optional>
#include <ranges>
#include <string>
#include <vector>

#include "jinject/jinject.h"

struct ProdutoInteractor {
  ProdutoInteractor(
      std::unique_ptr<CategoriaProdutoRepository> categoriaProdutoRepository,
      std::unique_ptr<ProdutoRepository> produtoRepository,
      std::unique_ptr<PrecoRepository> precoRepository)
      : mCategoriaProdutoRepository{std::move(categoriaProdutoRepository)},
        mProdutoRepository{std::move(produtoRepository)},
        mPrecoRepository{std::move(precoRepository)} {}

  std::vector<CategoriaProdutoModel> load_all_categorias() {
    return mCategoriaProdutoRepository->load_all();
  }

  std::vector<ProdutoInteractorModel> load_all_produtos() {
    auto items =
        mProdutoRepository->load_all() |
        std::views::transform([&](auto const &item) {
          auto categoriaProduto =
              mCategoriaProdutoRepository
                  ->find(item["categoria_id"].get_int().value())
                  .or_else([]() {
                    CategoriaProdutoModel model;

                    model["id"] = -1;
                    model["descricao"] = "<invalid>";

                    return std::optional{model};
                  })
                  .value();

          auto preco =
              mPrecoRepository->find(item["preco_id"].get_int().value())
                  .or_else([]() {
                    PrecoModel model;

                    model["id"] = -1;
                    model["valor"] = 0.0;

                    return std::optional{model};
                  })
                  .value();

          return ProdutoInteractorModel{categoriaProduto, item, preco};
        });

    return {items.begin(), items.end()};
  }

  std::optional<ProdutoModel> load_produto_by_name(std::string name) {
    auto produtoModelList = mProdutoRepository->load_by<"nome">(name);

    if (!produtoModelList.empty()) {
      return produtoModelList[0];
    }

    return {};
  }

  void save_produto(ProdutoModel const &item) {
    mProdutoRepository->save(item);
  }

  void save_all_produtos(std::vector<ProdutoModel> const &items) {
    mProdutoRepository->save_all(items);
  }

  void remove_produto(ProdutoModel const &item) {
    return mProdutoRepository->remove(item);
  }

private:
  std::unique_ptr<CategoriaProdutoRepository> mCategoriaProdutoRepository;
  std::unique_ptr<ProdutoRepository> mProdutoRepository;
  std::unique_ptr<PrecoRepository> mPrecoRepository;
};
