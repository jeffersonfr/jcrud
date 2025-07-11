#pragma once

#include "model/base/Token.hpp"
#include "model/sessionCredential/SessionCredentialRepository.hpp"
#include "model/usuario/UsuarioRepository.hpp"
#include "api/Session.hpp"

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

struct SessionInteractor final {
  SessionInteractor(std::unique_ptr<SessionCredentialRepository> sessionCredentialRepository,
                    std::unique_ptr<UsuarioRepository> usuarioRepository)
    : mSessionCredentialRepository(std::move(sessionCredentialRepository)),
      mUsuarioRepository(std::move(usuarioRepository)) {
  }

  std::expected<std::pair<std::string, Session>, std::string> create_session(Token const &token) {
    try {
      auto decodedToken = jwt::decode(token.token());
      auto payload = decodedToken.get_payload_json();

      auto exp = payload["exp"].to_str();
      auto jti = payload["jti"].to_str();

      auto items = mSessionCredentialRepository->load_by<"id">(jti);

      if (items.empty()) {
        return std::unexpected{"Invalid credentials"};
      }

      auto key = items[0]["key"].get_text().value();
      auto verify = jwt::verify().allow_algorithm(jwt::algorithm::hs256{key});

      verify.verify(decodedToken);

      std::string sessionToken = generate_random_token(512);
      std::string refreshToken = generate_random_token(16);

      std::lock_guard lock(mMutex);

      // TODO:: criar uma thread para chamar clear_invalid_sessions() a cada intervalo de tempo para limpar sessoes invalidas
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

      auto session = Session{jti, key, refreshToken, expires};

      mSessions.insert({sessionToken, session});

      return std::pair{sessionToken, session};
    } catch (std::exception const &e) {
      return std::unexpected{"Invalid credentials"};
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

  std::expected<Session, std::string> refresh_session(Token const &token) {
    try {
      /*
      auto decodedToken = jwt::decode(token.token());
      auto payload = decodedToken.get_payload_json();

      std::cout << "## PAYLOAD:iss: " << payload["iss"] << "\n";
      std::cout << "## PAYLOAD:iat: " << payload["sub"] << "\n";
      std::cout << "## PAYLOAD:jti: " << payload["jti"] << "\n";

      std::string id = payload["jti"].to_str();
      auto items = mSessionCredentialRepository->load_by<"id">(id);

      if (items.empty()) {
        return std::unexpected{"Invalid credentials"};
      }

      auto key = items[0]["key"].get_text().value();
      auto verify = jwt::verify().allow_algorithm(jwt::algorithm::hs256{key});

      verify.verify(decodedToken);

      boost::uuids::uuid uuid = boost::uuids::random_generator{}();
      std::string sessionToken = boost::uuids::to_string(uuid);
      std::string refreshToken = generate_random_token(16);

      std::lock_guard lock(mMutex);

      auto session = Session{id, key, refreshToken, std::chrono::seconds{3600}};

      mSessions.insert({sessionToken, session});

      return std::pair{sessionToken, session};
      */
    } catch (std::exception const &e) {
      return std::unexpected{"Invalid credentials"};
    }
  }

  std::expected<Session, std::string> get_session(std::string const &token) {
    std::lock_guard lock(mMutex);

    auto session = mSessions.find(token);

    if (session == mSessions.end()) {
      return std::unexpected{"forbidden"};
    }

    if (!session->second.valid()) {
      destroy_session(token);

      return std::unexpected{"forbidden"};
    }

    return session->second;
  }

  std::expected<Session, std::string> update_session(std::string const &token, Session const &updateSession) {
    std::lock_guard lock(mMutex);

    auto session = mSessions.find(token);

    if (session == mSessions.end()) {
      return std::unexpected{"forbidden"};
    }

    if (session->second.id() != updateSession.id()) {
      return std::unexpected{"forbidden"};
    }

    if (!session->second.valid()) {
      destroy_session(token);

      return std::unexpected{"forbidden"};
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
  std::unique_ptr<UsuarioRepository> mUsuarioRepository;
  std::unordered_map<std::string, Session> mSessions;
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
};
