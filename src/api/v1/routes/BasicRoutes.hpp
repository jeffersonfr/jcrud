#pragma once

#include "AbstractRoutes.hpp"
#include "api/Routes.hpp"
#include "api/mapping/Mapping.hpp"
#include "api/v1/control/session/JwtSessionInteractor.hpp"
#include "model/base/Token.hpp"

#define BaseUrl "/api/v1"

namespace v1 {
  struct BasicRoutes : public AbstractRoutes {
    BasicRoutes() = default;

    BasicRoutes(BasicRoutes &&other)
      : mProdutoInteractor{std::move(other.mProdutoInteractor)} {
    }

    void init(crow::SimpleApp &app) {
      lista_produtos(app);
      lista_categorias(app);
    }

  private:
    std::unique_ptr<ProdutoInteractor> mProdutoInteractor = jinject::get{};

    void lista_produtos(crow::SimpleApp &app) {
      CROW_ROUTE(app, BaseUrl"/produtos")
          .methods(crow::HTTPMethod::GET)
          (validate_request([&](crow::request const &req) {
            auto produtoList = mProdutoInteractor->load_all();

            auto const &items = std::ranges::unique(
              produtoList,
              [](auto const &a, auto const &b) {
                return a == b;
              },
              [](auto const &a) {
                return a.template get<ProdutoModel>()["id"].get_int();
              });

            return crow::json::wvalue{
              {"produtos", Mapping::range_to_json<ProdutoModel>(produtoList.begin(), items.begin())},
            };
          }, {Cargo::Operador, Cargo::Administrador}));
    }

    void lista_categorias(crow::SimpleApp &app) {
      CROW_ROUTE(app, BaseUrl"/categorias")
          .methods(crow::HTTPMethod::GET)
          (validate_request([&](crow::request const &req) {
            auto categoriasList = mProdutoInteractor->load_all_categorias();

            return crow::json::wvalue{
              {"categorias", Mapping::range_to_json(categoriasList)}
            };
          }, {Cargo::Operador, Cargo::Administrador}));
    }
  };
}
