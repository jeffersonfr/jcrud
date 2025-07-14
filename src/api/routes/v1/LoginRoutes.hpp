#pragma once

#include "api/Routes.hpp"
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
              return unauthorized_response("invalid_token", ApiErrorMsg::NO_CREDENTIALS);
            }

            auto result = sessionInteractor->create_session(*token);

            if (!result) {
              return unauthorized_response("invalid_token", result.error());
            }

            auto [sessionToken, session] = result.value();

            return crow::response(crow::status::ACCEPTED,
                                  crow::json::wvalue{
                                    {"sessionToken", sessionToken},
                                    {"refreshToken", session.refresh_token()}
                                  });
          });
    }




    template<typename Callback>
    auto validate_refresh_token(Callback &&callback, std::set<Cargo> &&cargos = {}) {
      return [callback, this] <typename... Args>(crow::request const &req, Args &&... args) -> crow::response {

        return std::invoke(callback, req, std::forward<Args>(args)...);
      };
    }



    void refresh(crow::SimpleApp &app) {
      CROW_ROUTE(app, BaseUrl"/refresh")
          .methods(crow::HTTPMethod::POST)
          ([&](crow::request const &req) {
            std::shared_ptr<SessionInteractor> sessionInteractor = jinject::get{};

            auto token = Token::from(req.get_header_value("Authorization"));

            if (!token) {
              return unauthorized_response("invalid_token", ApiErrorMsg::INVALID_CREDENTIALS);
            }

            auto result = sessionInteractor->get_session(token->token());

            if (!result) {
              return unauthorized_response("invalid_token", result.error());
            }

            auto &session = *result;

            if (!session.valid()) {
              return unauthorized_response("invalid_token", ApiErrorMsg::TOKEN_EXPIRED);
            }

            crow::json::rvalue msg = crow::json::load(req.body);

            if (!msg["refreshToken"]) {
              return unauthorized_response("invalid_token", ApiErrorMsg::INVALID_CREDENTIALS);
            }

            std::string oldRefreshToken = static_cast<std::string>(msg["refreshToken"]);

            if (session.refresh_token() != oldRefreshToken) {
              return unauthorized_response("invalid_token", ApiErrorMsg::INVALID_CREDENTIALS);
            }

            auto newRefreshToken = sessionInteractor->refresh_session(*token, oldRefreshToken);

            auto [newSessionToken, newSession] = newRefreshToken.value();

            return crow::response(crow::status::ACCEPTED,
                                  crow::json::wvalue{
                                    {"sessionToken", newSessionToken},
                                    {"refreshToken", newSession.refresh_token()}
                                  });
          });
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
