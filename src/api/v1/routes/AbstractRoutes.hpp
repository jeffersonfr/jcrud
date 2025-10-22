#pragma once

#include "api/Routes.hpp"
#include "api/v1/control/session/JwtSessionInteractor.hpp"
#include "api/v1/ApiErrorMsg.hpp"
#include "model/base/Token.hpp"

#define BaseUrl "/api/v1"

namespace v1 {
  struct AbstractRoutes {
    AbstractRoutes() = default;

  protected:
    static crow::response unauthorized_response(std::string const &error, std::string const &msg) {
      auto res = crow::response(crow::status::UNAUTHORIZED);

      res.add_header("WWW-Authenticate",
                     std::format(R"(Bearer realm="jcrud", error="{}", error_description="{}")", error, msg));

      return res;
    }

    static void unauthorized_response(crow::response &res, std::string const &error, std::string const &msg) {
      res.code = crow::status::UNAUTHORIZED;
      res.add_header("WWW-Authenticate",
                     std::format(R"(Bearer realm="jcrud", error="{}", error_description="{}")", error, msg));
      res.end();
    }

    template<typename Callback>
    auto validate_request(Callback &&callback, std::set<Cargo> const &cargos = {}) {
      return [callback, cargos, this] <typename... Args>(crow::request const &req, Args &&... args) -> crow::response {
        std::shared_ptr<JwtSessionInteractor> sessionInteractor = jinject::get{};
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

        if (!session.validate_cargos(cargos)) {
          return unauthorized_response("invalid_token", ApiErrorMsg::FORBIDDEN_ACCESS);
        }

        return std::invoke(callback, req, std::forward<Args>(args)...);
      };
    }

    template<typename Callback>
    auto validate_request_response(Callback &&callback, std::set<Cargo> const &cargos = {}) {
      return [callback, cargos, this] <typename... Args>(crow::request const &req, crow::response &res, Args &&... args) {
        std::shared_ptr<JwtSessionInteractor> sessionInteractor = jinject::get{};
        auto token = Token::from(req.get_header_value("Authorization"));

        if (!token) {
          unauthorized_response(res, "invalid_token", ApiErrorMsg::INVALID_CREDENTIALS);

          return;
        }

        auto result = sessionInteractor->get_session(token->token());

        if (!result) {
          unauthorized_response(res, "invalid_token", result.error());

          return;
        }

        auto &session = *result;

        if (!session.valid()) {
          unauthorized_response(res, "invalid_token", ApiErrorMsg::TOKEN_EXPIRED);

          return;
        }

        if (!session.validate_cargos(cargos)) {
          unauthorized_response(res, "invalid_token", ApiErrorMsg::FORBIDDEN_ACCESS);

          return;
        }

        std::invoke(callback, req, res, std::forward<Args>(args)...);
      };
    }
  };
}

