#pragma once

#include "control/produto/ProdutoInteractorModel.hpp"
#include "model/categoriaProduto/CategoriaProdutoRepository.hpp"
#include "model/preco/PrecoRepository.hpp"
#include "model/produto/ProdutoRepository.hpp"

#include <expected>
#include <optional>
#include <ranges>
#include <string>
#include <utility>
#include <vector>

#include "jinject/jinject.h"

struct ProdutoInteractor : public Repository<ProdutoInteractorModel> {
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
    auto items = load_all() | std::ranges::views::filter([](auto const &item) {
                   return !item.template get<ProdutoModel>("excluido")
                               .get_bool()
                               .value();
                 });

    auto it = std::ranges::unique(items, [](auto a, auto b) {
      return a.template get<ProdutoModel>("id") ==
             b.template get<ProdutoModel>("id");
    });

    return {items.begin(), it.begin()};
  }

  std::vector<PrecoModel> load_historico_precos(int64_t produtoId) {
    return mPrecoRepository->load_by<"produto_id">(produtoId);
  }

  std::optional<ProdutoInteractorModel> load_produto_by_id(int64_t id) {
    auto produtos = load_all_produtos() |
                    std::ranges::views::filter([&](auto const &produto) {
                      return produto.template get<ProdutoModel>("id") == id;
                    });

    for (auto const &produto : produtos) {
      return produto;
    }

    return {};
  }

  void save_produto(ProdutoInteractorModel const &item) {
    if (item.get<ProdutoModel>("id").is_null()) {
      mProdutoRepository->get_database()->transaction([&](Database &db) {
        auto produto = mProdutoRepository->save(item.get<ProdutoModel>());

        if (produto.has_value()) {
          item.get<PrecoModel>("produto_id") = (*produto)["id"];

          auto e = mPrecoRepository->save(item.get<PrecoModel>());

          if (!e.has_value()) {
            throw e.error();
          }
        }
      });
    } else {
      update(item);
    }
  }

  void remove_produto(ProdutoInteractorModel const &item) {
    auto produtoId = item.get<ProdutoModel>("id").get_int();

    if (!produtoId.has_value()) {
      return;
    }

    auto itemOther{std::move(item.get<ProdutoModel>())};

    itemOther["excluido"] = true;

    mProdutoRepository->update(itemOther);

    /*
    mProdutoRepository->get_database()->transaction([&](Database &db) {
      auto produtoId = item.get<ProdutoModel>("id").get_int();

      if (!produtoId.has_value()) {
        return;
      }

      std::ranges::for_each(
          mPrecoRepository->load_by<"produto_id">(id),
          [&](auto const &item) { mPrecoRepository->remove(item); });

      mProdutoRepository->remove(item);
    });
     */
  }

private:
  std::unique_ptr<CategoriaProdutoRepository> mCategoriaProdutoRepository;
  std::unique_ptr<ProdutoRepository> mProdutoRepository;
  std::unique_ptr<PrecoRepository> mPrecoRepository;
};
