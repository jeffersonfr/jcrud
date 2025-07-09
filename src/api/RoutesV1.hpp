#pragma once

#include "api/Routes.hpp"
#include "api/Mapping.hpp"

#define BASEURL "/api/v1"

struct RoutesV1 : public Routes {
    RoutesV1()
        : Routes{1} {
    }

    virtual ~RoutesV1() = default;

private:
    std::unique_ptr<ProdutoInteractor> mProdutoInteractor = jinject::get{};

    void init_routes() override {
        blackhole_route();

        login();
        lista_produtos();
        lista_categorias();
    }

    void blackhole_route() {
        CROW_CATCHALL_ROUTE(mApp)
                ([this]() {
                    return crow::response(crow::status::FORBIDDEN, "Sad :(");
                });
    }

    void login() {
        CROW_ROUTE(mApp, BASEURL"/login")
            .methods(crow::HTTPMethod::POST)
                ([this]() {
                    // process JWT
                    return crow::response(crow::status::FORBIDDEN);
                });
    }

    void lista_produtos() {
        CROW_ROUTE(mApp, BASEURL"/produtos")
            .methods(crow::HTTPMethod::GET)
                ([this]() {
                    auto produtoList = mProdutoInteractor->load_all();

                    auto const &items = std::ranges::unique(
                        produtoList,
                        [](auto const &a, auto const &b) {
                            return a == b;
                        },
                        [](auto const &a) {
                            return a.template get<ProdutoModel>()["id"].get_int();
                        });

                    return crow::json::wvalue {
                        {"produtos", Mapping::range_to_json<ProdutoModel>(produtoList.begin(), items.begin())},
                    };
                });
    }

    void lista_categorias() {
        CROW_ROUTE(mApp, BASEURL"/categorias")
            .methods(crow::HTTPMethod::GET)
                ([this]() {
                    auto categoriasList = mProdutoInteractor->load_all_categorias();

                    return crow::json::wvalue {
                        {"categorias", Mapping::range_to_json(categoriasList)}
                    };
                });
    }
};
