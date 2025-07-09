#pragma once

#include "model/tipoNegocio/TipoNegocioRepository.hpp"

void tiponegocio_module() {
  using namespace jinject;

  UNIQUE(TipoNegocioRepository) {
    return new TipoNegocioRepository{inject<std::shared_ptr<Database>>()};
  };
}
