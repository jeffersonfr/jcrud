#pragma once

#include "control/produto/ProdutoInteractorModel.hpp"
#include "model/categoria_produto/CategoriaProdutoRepository.hpp"
#include "model/preco/PrecoRepository.hpp"
#include "model/produto/ProdutoRepository.hpp"

#include <optional>
#include <ranges>
#include <string>
#include <vector>
#include <expected>

#include "jinject/jinject.h"

struct ProdutoInteractor
{
  ProdutoInteractor(
      std::unique_ptr<CategoriaProdutoRepository> categoriaProdutoRepository,
      std::unique_ptr<ProdutoRepository> produtoRepository,
      std::unique_ptr<PrecoRepository> precoRepository)
      : mCategoriaProdutoRepository{std::move(categoriaProdutoRepository)},
        mProdutoRepository{std::move(produtoRepository)},
        mPrecoRepository{std::move(precoRepository)} {}

  std::vector<CategoriaProdutoModel> load_all_categorias()
  {
    return mCategoriaProdutoRepository->load_all();
  }

  std::vector<ProdutoInteractorModel> load_all_produtos()
  {
    auto items = mProdutoRepository->load_all() |
                 std::views::transform([&](auto const &item)
                                       {
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
                       mPrecoRepository
                           ->last_by<"produto_id">(item["id"].get_int().value())
                           .or_else([]() {
                             PrecoModel model;

                             model["valor"] = 0.0;

                             return std::optional{model};
                           })
                           .value();

                   return ProdutoInteractorModel{categoriaProduto, item, preco}; });

    return {items.begin(), items.end()};
  }

  std::vector<PrecoModel> load_historico_precos(int64_t produtoId)
  {
    return mPrecoRepository->load_by<"produto_id">(produtoId);
  }

  std::optional<ProdutoInteractorModel> load_produto_by_id(int64_t id)
  {
    auto produtos = load_all_produtos()
     | std::ranges::views::filter([&](auto const &produto) {
          return produto.template get<ProdutoModel>("id").get_int().value() == id; });

    for (auto const &produto : produtos)
    {
      return produto;
    }

    return {};
  }

  void save_produto(ProdutoInteractorModel const &item)
  {
    if (item.get<ProdutoModel>("id").is_null())
    {
      auto produto = mProdutoRepository->save(item.get<ProdutoModel>());

      if (produto)
      {
        item.get<PrecoModel>("produto_id") = produto.value()["id"];

        mPrecoRepository->save(item.get<PrecoModel>());
      }
    }
    else
    {
      mProdutoRepository->update(item.get<ProdutoModel>());

      item.get<PrecoModel>("produto_id") = item.get<ProdutoModel>("id");

      mPrecoRepository->save(item.get<PrecoModel>());
    }
  }

  void remove_produto(ProdutoInteractorModel const &item)
  {
    // TODO:: remover preco tambem
    return mProdutoRepository->remove(item);
  }

private:
  std::unique_ptr<CategoriaProdutoRepository> mCategoriaProdutoRepository;
  std::unique_ptr<ProdutoRepository> mProdutoRepository;
  std::unique_ptr<PrecoRepository> mPrecoRepository;
};
