#pragma once

#include "model/filial/FilialRepository.hpp"

void filial_module() {
  using namespace jinject;

  UNIQUE(FilialRepository) {
    return new FilialRepository{inject<std::shared_ptr<jdb::Database>>()};
  };
}
