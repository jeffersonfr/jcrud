#pragma once

#include "database/DataClass.hpp"

#include <string>
#include <chrono>

struct Session {
  explicit Session(std::string const &id, std::string const &key, std::string const &refreshToken,
                   std::chrono::system_clock::time_point expires)
    : mId{id}, mKey{key}, mRefreshToken{refreshToken}, mTimeout{expires} {
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

  bool valid() const {
    return mTimeout > std::chrono::system_clock::now();
  }

  std::unordered_map<std::string, Data> params() {
    return mParams;
  }

private:
  std::unordered_map<std::string, Data> mParams;
  std::string mId;
  std::string mKey;
  std::string mRefreshToken;
  std::chrono::time_point<std::chrono::system_clock> mTimeout;
};
