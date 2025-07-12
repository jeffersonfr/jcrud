#pragma once

#include "api/Routes.hpp"
#include "api/Mapping.hpp"
#include "api/routes/control/session/SessionInteractor.hpp"
#include "model/base/Token.hpp"

#define BaseUrl "/api/v1"

namespace v1 {
  struct AbstractRoutes {
    AbstractRoutes() = default;

  protected:
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
  };
}
