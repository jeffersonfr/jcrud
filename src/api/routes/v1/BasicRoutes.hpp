#pragma once

#include "api/Routes.hpp"
#include "api/Mapping.hpp"
#include "api/routes/control/session/SessionInteractor.hpp"
#include "model/base/Token.hpp"

#define BaseUrl "/api/v1"

namespace v1 {

  struct BasicRoutes {
    BasicRoutes() = default;

    BasicRoutes(BasicRoutes &&other)
      : mProdutoInteractor{std::move(other.mProdutoInteractor)} {
    }

    void init(crow::SimpleApp &app) {
      login(app);
      refresh(app);

      lista_produtos(app);
      lista_categorias(app);
    }

  private:
    std::unique_ptr<ProdutoInteractor> mProdutoInteractor = jinject::get{};

    crow::response unauthorized_response(std::string const &error, std::string const &msg) {
      auto res = crow::response(crow::status::UNAUTHORIZED);

      res.add_header("WWW-Authenticate",
                     std::format(R"(Bearer realm="jcrud", error="{}", error_description="{}")", error, msg));

      return res;
    }

    template<typename Callback>
    auto validate_request(Callback &&callback, std::set<Cargo> &&cargos = {}) {

      return [callback, this] <typename... Args>(crow::request const &req, Args &&... args) -> crow::response {
        std::shared_ptr<SessionInteractor> sessionInteractor = jinject::get{};

        auto token = Token::from(req.get_header_value("Authorization"));

        if (!token) {
          return unauthorized_response("invalid_token", "Invalid token");
        }

        auto result = sessionInteractor->get_session(token->token());

        if (!result) {
          return unauthorized_response("invalid_token", result.error());
        }

        auto &session = *result;

        if (!session.valid()) {
          return unauthorized_response("invalid_token", "Token expired");
        }

        // INFO:: validate user roles

        return std::invoke(callback, req, std::forward<Args>(args)...);
      };
    }

    void login(crow::SimpleApp &app) {
      CROW_ROUTE(app, BaseUrl"/login")
          .methods(crow::HTTPMethod::GET)
          ([&](crow::request const &req) {
            std::shared_ptr<SessionInteractor> sessionInteractor = jinject::get{};

            auto token = Token::from(req.get_header_value("Authorization"));

            if (!token) {
              return unauthorized_response("invalid_token", "Invalid token");
            }

            auto result = sessionInteractor->create_session(*token);

            if (!result) {
              return unauthorized_response("invalid_token", result.error());
            }

            auto [sessionToken, session] = result.value();

            return crow::response(crow::status::ACCEPTED,
                                  crow::json::wvalue{
                                    {"sessiontoken", sessionToken},
                                    {"refreshToken", session.refresh_token()}
                                  });
          });
    }

    // TODO:: implement refresh token
    void refresh(crow::SimpleApp &app) {
      CROW_ROUTE(app, BaseUrl"/refresh")
          .methods(crow::HTTPMethod::POST)
          ([&](crow::request const &req) {
            std::shared_ptr<SessionInteractor> sessionInteractor = jinject::get{};

            auto token = Token::from(req.get_header_value("Authorization"));

            if (!token) {
              return unauthorized_response("invalid_token", "Invalid token");
            }

            auto result = sessionInteractor->create_session(*token);

            if (!result) {
              return unauthorized_response("invalid_token", result.error());
            }

            auto [sessionToken, session] = result.value();

            return crow::response(crow::status::ACCEPTED,
                                  crow::json::wvalue{
                                    {"sessiontoken", sessionToken},
                                    {"refreshToken", session.refresh_token()}
                                  });
          });
    }

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
          }, {Cargo::Operador}));
    }

    void lista_categorias(crow::SimpleApp &app) {
      CROW_ROUTE(app, BaseUrl"/categorias")
          .methods(crow::HTTPMethod::GET)
          (validate_request([&](crow::request const &req) {
            auto categoriasList = mProdutoInteractor->load_all_categorias();

            return crow::json::wvalue{
              {"categorias", Mapping::range_to_json(categoriasList)}
            };
          }, {Cargo::Operador}));
    }
  };
}
