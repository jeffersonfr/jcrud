#pragma once

#include "database/DataClass.hpp"

#include <string>
#include <chrono>

#include "api/Session.hpp"

struct JwtSession : public Session {
  explicit JwtSession(std::set<Cargo> const &cargos, std::string const &id, std::string const &key,
                      std::string const &refreshToken,
                      std::chrono::system_clock::time_point expires)
    : Session{cargos}, mId{id}, mKey{key}, mRefreshToken{refreshToken}, mExpires{expires} {
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
