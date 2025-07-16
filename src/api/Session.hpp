#pragma once

#include "database/DataClass.hpp"

#include <string>
#include <chrono>

struct Session {
  explicit Session(std::set<Cargo> const &cargos)
    : mCargos{cargos} {
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

  bool has_any_common_element(std::set<Cargo> const &a, std::set<Cargo> const &b) const {
    for (Cargo item : a) {
      if (b.contains(item)) {
        return true;
      }
    }

    return false;
  }
};
