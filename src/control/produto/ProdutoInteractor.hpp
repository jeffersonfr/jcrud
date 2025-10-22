#pragma once

#include "control/produto/ProdutoInteractorModel.hpp"
#include "model/categoriaProduto/CategoriaProdutoRepository.hpp"
#include "model/preco/PrecoRepository.hpp"
#include "model/produto/ProdutoRepository.hpp"

#include <optional>
#include <ranges>
#include <string>
#include <utility>
#include <vector>

#include "ProdutoInteractorRepository.hpp"

struct ProdutoInteractor {
  ProdutoInteractor(
    std::unique_ptr<ProdutoInteractorRepository> produtoInteractorRepository,
    std::unique_ptr<CategoriaProdutoRepository> categoriaProdutoRepository,
    std::unique_ptr<ProdutoRepository> produtoRepository,
    std::unique_ptr<PrecoRepository> precoRepository)
    : mProdutoInteractorRepository{std::move(produtoInteractorRepository)},
      mCategoriaProdutoRepository{std::move(categoriaProdutoRepository)},
      mProdutoRepository{std::move(produtoRepository)},
      mPrecoRepository{std::move(precoRepository)} {
  }

  [[nodiscard]] std::vector<ProdutoInteractorModel> load_all() const {
    return mProdutoInteractorRepository->load_all();
  }

  [[nodiscard]] std::optional<std::string> update(ProdutoInteractorModel const &produto) const {
    return mProdutoInteractorRepository->update(produto);
  }

  [[nodiscard]] std::vector<CategoriaProdutoModel> load_all_categorias() const {
    return mCategoriaProdutoRepository->load_all();
  }

  [[nodiscard]] std::vector<ProdutoInteractorModel> load_all_produtos() const {
    auto items = mProdutoInteractorRepository->load_all() |
      std::ranges::views::filter([](auto const &item) {
      return !item.template get<ProdutoModel>("excluido")
        .get_bool()
        .value();
    });

    auto it = std::ranges::unique(items, [](auto &a, auto &b) {
      return a.template get<ProdutoModel>("id") ==
             b.template get<ProdutoModel>("id");
    });

    return {items.begin(), it.begin()};
  }

  [[nodiscard]] std::vector<PrecoModel> load_historico_precos(int64_t produtoId) const {
    return mPrecoRepository->load_by<"produto_id">(produtoId);
  }

  [[nodiscard]] std::optional<ProdutoInteractorModel> load_produto_by_id(int64_t id) const {
    auto produtos = load_all_produtos() |
                    std::ranges::views::filter([&](auto const &produto) {
                      return produto.template get<ProdutoModel>("id") == id;
                    });

    for (auto const &produto: produtos) {
      return produto;
    }

    return {};
  }

  [[nodiscard]] std::optional<std::string> save_produto(ProdutoInteractorModel const &item) const {
    if (item.get<ProdutoModel>("id").is_null()) {
      try {
        mProdutoRepository->get_database()->transaction([&](jdb::Database &db) {
          auto produto = mProdutoRepository->save(item.get<ProdutoModel>());

          if (produto.has_value()) {
            item.get<PrecoModel>("produto_id") = (*produto)["id"];

            auto e = mPrecoRepository->save(item.get<PrecoModel>());

            if (!e.has_value()) {
              throw std::move(e.error());
            }
          }
        });
      } catch (std::runtime_error &e) {
        return e.what();
      }

      return {};
    }

    return update(item);
  }

  [[nodiscard]] std::optional<std::string> remove_produto(ProdutoInteractorModel const &produtoInteractor) const {
    auto produto{std::move(produtoInteractor.get<ProdutoModel>())};

    produto["excluido"] = true;

    return mProdutoRepository->update(produto);
  }

private:
  std::unique_ptr<ProdutoInteractorRepository> mProdutoInteractorRepository;
  std::unique_ptr<CategoriaProdutoRepository> mCategoriaProdutoRepository;
  std::unique_ptr<ProdutoRepository> mProdutoRepository;
  std::unique_ptr<PrecoRepository> mPrecoRepository;
};
