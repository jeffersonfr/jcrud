#pragma once

#include "model/cargo/CargoRepository.hpp"

inline void cargo_module() {
  using namespace jinject;

  UNIQUE(CargoRepository) {
    return new CargoRepository{inject<std::shared_ptr<jdb::Database>>()};
  };
}
