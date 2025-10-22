#pragma once

#include "jdb/database/DataClass.hpp"

#include <string>

struct Session {
  explicit Session(std::set<Cargo> const &cargos)
    : mCargos{cargos} {
  }

  bool validate_cargos(std::set<Cargo> const &cargos) const {
    return has_any_common_element(cargos, mCargos);
  }

  std::unordered_map<std::string, jdb::Data> params() {
    return mParams;
  }

private:
  std::unordered_map<std::string, jdb::Data> mParams;
  std::set<Cargo> mCargos;

  static bool has_any_common_element(std::set<Cargo> const &a, std::set<Cargo> const &b) {
    return std::ranges::any_of(a, [&b](auto const &item) {
      return b.contains(item);
    });
  }
};
