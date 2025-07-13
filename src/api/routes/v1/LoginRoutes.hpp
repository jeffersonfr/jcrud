#pragma once

#include "api/Routes.hpp"
#include "api/Mapping.hpp"
#include "api/routes/control/session/SessionInteractor.hpp"
#include "model/base/Token.hpp"

#define BaseUrl "/api/v1"

namespace v1 {
  struct LoginRoutes : public AbstractRoutes {
    LoginRoutes() = default;

    LoginRoutes(LoginRoutes &&other) {
    }

    void init(crow::SimpleApp &app) {
      login(app);
      refresh(app);
      logout(app);
    }

  private:
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
          (validate_refresh_token([&](crow::request const &req) {
            std::shared_ptr<SessionInteractor> sessionInteractor = jinject::get{};

            auto token = Token::from(req.get_header_value("Authorization"));
            auto result = sessionInteractor->refresh_session(*token);

            auto [sessionToken, session] = result.value();

            return crow::response(crow::status::ACCEPTED,
                                  crow::json::wvalue{
                                    {"sessiontoken", sessionToken},
                                    {"refreshToken", session.refresh_token()}
                                  });
          }));
    }

    void logout(crow::SimpleApp &app) {
      CROW_ROUTE(app, BaseUrl"/logout")
          .methods(crow::HTTPMethod::GET)
          (validate_request([&](crow::request const &req) {
            std::shared_ptr<SessionInteractor> sessionInteractor = jinject::get{};

            auto token = Token::from(req.get_header_value("Authorization"));

            if (!token) {
              return unauthorized_response("invalid_token", "Invalid token");
            }

            sessionInteractor->destroy_session(token->token());

            return crow::response(crow::status::OK);
          }, {Cargo::Operador, Cargo::Administrador}));
    }
  };
}
