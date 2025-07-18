#pragma once

#include "model/base/Token.hpp"
#include "model/sessionCredential/SessionCredentialRepository.hpp"
#include "model/usuario/UsuarioRepository.hpp"
#include "api/v1/JwtSession.hpp"
#include "api/v1/ApiErrorMsg.hpp"

#include <jwt-cpp/jwt.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <string>
#include <chrono>
#include <expected>
#include <unordered_map>
#include <utility>
#include <mutex>
#include <ranges>

struct JwtSessionInteractor final {
  JwtSessionInteractor(std::unique_ptr<SessionCredentialRepository> sessionCredentialRepository,
                    std::unique_ptr<CargoUsuarioRepository> cargoUsuarioRepository)
    : mSessionCredentialRepository{std::move(sessionCredentialRepository)},
      mCargoUsuarioRepository(std::move(cargoUsuarioRepository)) {
  }

  std::expected<std::pair<std::string, JwtSession>, std::string> create_session(Token const &token) {
    try {
      auto decodedToken = jwt::decode(token.token());
      auto payload = decodedToken.get_payload_json();

      auto exp = payload["exp"].to_str();
      auto jti = payload["jti"].to_str();

      auto items = mSessionCredentialRepository->load_by<"id">(jti);

      if (items.empty()) {
        return std::unexpected{ApiErrorMsg::INVALID_CREDENTIALS};
      }

      auto key = items[0]["key"].get_text().value();
      auto verify = jwt::verify().allow_algorithm(jwt::algorithm::hs256{key});

      verify.verify(decodedToken);

      std::string sessionToken = generate_random_token(512);
      std::string refreshToken = generate_random_token(16);

      std::lock_guard lock(mMutex);

      for (auto &it: mSessions) {
        if (it.second.id() == jti) {
          if (it.second.valid()) {
            return it;
          }

          destroy_session(it.first);

          break;
        }
      }

      const auto expires = std::chrono::system_clock::from_time_t(std::stoi(exp));
      auto usuarioId = items[0]["usuario_id"].get_int().value();
      auto session = JwtSession{load_cargos(usuarioId), jti, key, refreshToken, expires};

      mSessions.insert({sessionToken, session});

      return std::pair{sessionToken, session};
    } catch (std::runtime_error const &e) {
      return std::unexpected{e.what()};
    } catch (std::exception const &e) {
      return std::unexpected{ApiErrorMsg::INVALID_CREDENTIALS};
    }
  }

  void clear_invalid_sessions() {
    std::lock_guard lock(mMutex);

    for (auto it = mSessions.begin(); it != mSessions.end();) {
      if (!it->second.valid()) {
        it = mSessions.erase(it);
      } else {
        ++it;
      }
    }
  }

  std::expected<std::pair<std::string, JwtSession>, std::string> refresh_session(
    Token const &token, std::string const &oldRefreshToken) {
    try {
      std::lock_guard lock(mMutex);

      auto session = mSessions.find(token.token())->second;

      if (session.refresh_token() != oldRefreshToken) {
        return std::unexpected{ApiErrorMsg::INVALID_CREDENTIALS};
      }

      auto credentials = mSessionCredentialRepository->first_by<"id">(session.id());

      if (!credentials) {
        return std::unexpected{ApiErrorMsg::INVALID_CREDENTIALS};
      }

      std::string sessionToken = generate_random_token(512);
      std::string refreshToken = generate_random_token(16);

      auto usuarioId = credentials.value()["usuario_id"].get_int().value();
      auto newSession = JwtSession{load_cargos(usuarioId), session.id(), session.key(), refreshToken, session.expires()};

      destroy_session(token.token());

      mSessions.insert({sessionToken, newSession});

      return std::pair{sessionToken, newSession};
    } catch (std::runtime_error const &e) {
      return std::unexpected{e.what()};
    } catch (std::exception const &e) {
      return std::unexpected{ApiErrorMsg::INVALID_CREDENTIALS};
    }
  }

  std::expected<JwtSession, std::string> get_session(std::string const &token) {
    std::lock_guard lock(mMutex);

    auto session = mSessions.find(token);

    if (session == mSessions.end()) {
      return std::unexpected{ApiErrorMsg::FORBIDDEN_ACCESS};
    }

    if (!session->second.valid()) {
      destroy_session(token);

      return std::unexpected{ApiErrorMsg::FORBIDDEN_ACCESS};
    }

    return session->second;
  }

  std::expected<JwtSession, std::string> update_session(std::string const &token, JwtSession const &updateSession) {
    std::lock_guard lock(mMutex);

    auto session = mSessions.find(token);

    if (session == mSessions.end()) {
      return std::unexpected{ApiErrorMsg::FORBIDDEN_ACCESS};
    }

    if (session->second.id() != updateSession.id()) {
      return std::unexpected{ApiErrorMsg::FORBIDDEN_ACCESS};
    }

    if (!session->second.valid()) {
      destroy_session(token);

      return std::unexpected{ApiErrorMsg::FORBIDDEN_ACCESS};
    }

    destroy_session(token);

    mSessions.insert({token, updateSession});

    return updateSession;
  }

  void destroy_session(const std::string &token) {
    std::lock_guard lock(mMutex);

    mSessions.erase(token);
  }

private:
  std::unique_ptr<SessionCredentialRepository> mSessionCredentialRepository;
  std::unique_ptr<CargoUsuarioRepository> mCargoUsuarioRepository;
  std::unique_ptr<UsuarioRepository> mUsuarioRepository;
  std::unordered_map<std::string, JwtSession> mSessions;
  std::recursive_mutex mMutex;

  std::string generate_random_token(std::size_t length) {
    const std::string CHARACTERS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> distribution(0, CHARACTERS.size() - 1);
    std::string token;

    token.reserve(length);

    for (size_t i = 0; i < length; ++i) {
      token += CHARACTERS[distribution(generator)];
    }

    return token;
  }

  std::set<Cargo> load_cargos(int64_t usuarioId) {
    auto items = mCargoUsuarioRepository->load_by<"usuario_id">(usuarioId)
                 | std::views::transform([&](auto const &item) {
                   return static_cast<Cargo>(item["cargo_id"].get_int().value());
                 });

    return {items.begin(), items.end()};
  }
};
