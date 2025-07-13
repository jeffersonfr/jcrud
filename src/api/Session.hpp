#pragma once

#include "database/DataClass.hpp"

#include <string>
#include <chrono>

struct Session {
  explicit Session(std::set<Cargo> const &cargos, std::string const &id, std::string const &key, std::string const &refreshToken,
                   std::chrono::system_clock::time_point expires)
    : mCargos{cargos}, mId{id}, mKey{key}, mRefreshToken{refreshToken}, mExpires{expires} {
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

  bool validate_cargos(std::set<Cargo> const &cargos) const {
    return has_any_common_element(cargos, mCargos);
  }

  std::unordered_map<std::string, Data> params() {
    return mParams;
  }

private:
  std::unordered_map<std::string, Data> mParams;
  std::set<Cargo> mCargos;
  std::string mId;
  std::string mKey;
  std::string mRefreshToken;
  std::chrono::time_point<std::chrono::system_clock> mExpires;

  bool has_any_common_element(std::set<Cargo> const &a, std::set<Cargo> const &b) const {
    for (Cargo item : a) {
      if (b.contains(item)) {
        return true;
      }
    }

    return false;
  }
};
