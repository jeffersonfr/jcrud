#pragma once

#include "control/produto/interactor/ProdutoInteractorModel.hpp"
#include "model/categoria_produto/CategoriaProdutoRepository.hpp"
#include "model/produto/ProdutoRepository.hpp"

#include <optional>
#include <ranges>
#include <string>
#include <vector>

#include "jinject/jinject.h"

struct ProdutoInteractor {
  ProdutoInteractor(
      std::unique_ptr<CategoriaProdutoRepository> categoriaProdutoRepository,
      std::unique_ptr<ProdutoRepository> produtoRepository)
      : mCategoriaProdutoRepository{std::move(categoriaProdutoRepository)},
        mProdutoRepository{std::move(produtoRepository)} {}

  std::vector<CategoriaProdutoModel> load_all_categorias() {
    return mCategoriaProdutoRepository->load_all();
  }

  std::vector<ProdutoInteractorModel> load_all_produtos() {
    auto items =
        mProdutoRepository->load_all() |
        std::views::transform([&](auto const &item) {
          auto categoriaProduto =
              mCategoriaProdutoRepository->find(item["category"].get_int().value()).or_else([]() {
                CategoriaProdutoModel model;
                
                model["id"] = -1;
                model["description"] = "<invalid>";

                return std::optional{model};
              }).value();

          return ProdutoInteractorModel{categoriaProduto, item};
        });

    return {items.begin(), items.end()};
  }

  std::optional<ProdutoModel> load_produto_by_name(std::string name) {
    auto produtoModelList = mProdutoRepository->load_by<"name">(name);

    if (!produtoModelList.empty()) {
      return produtoModelList[0];
    }

    return {};
  }

  void save_produto(ProdutoModel const &item) { mProdutoRepository->save(item); }

  void save_all_produtos(std::vector<ProdutoModel> const &items) {
    mProdutoRepository->save_all(items);
  }

  void remove_produto(ProdutoModel const &item) {
    return mProdutoRepository->remove(item);
  }

private:
  std::unique_ptr<CategoriaProdutoRepository> mCategoriaProdutoRepository;
  std::unique_ptr<ProdutoRepository> mProdutoRepository;
};
