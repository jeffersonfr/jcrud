#pragma once

#include "model/cargo/CargoRepository.hpp"

void cargo_module() {
  using namespace jinject;

  UNIQUE(CargoRepository) {
    return new CargoRepository{inject<std::shared_ptr<Database>>()};
  };
}
