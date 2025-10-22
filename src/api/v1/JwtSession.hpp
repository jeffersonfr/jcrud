#pragma once

#include "jdb/database/DataClass.hpp"

#include <string>
#include <chrono>

#include "api/Session.hpp"

struct JwtSession : public Session {
  explicit JwtSession(std::set<Cargo> const &cargos, std::string id, std::string key, std::string refreshToken,
                      std::chrono::system_clock::time_point expires)
    : Session{cargos}, mId{std::move(id)}, mKey{std::move(key)}, mRefreshToken{std::move(refreshToken)}, mExpires{expires} {
  }

  std::string id() const {
    return mId;
  }

  std::string key() const {
    return mKey;
  }

  std::string refresh_token() const {
    return mRefreshToken;
  }

  std::chrono::system_clock::time_point expires() const {
    return mExpires;
  }

  bool valid() const {
    return mExpires > std::chrono::system_clock::now();
  }

private:
  std::string mId;
  std::string mKey;
  std::string mRefreshToken;
  std::chrono::time_point<std::chrono::system_clock> mExpires;
};
