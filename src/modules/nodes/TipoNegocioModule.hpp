#pragma once

#include "model/tipoNegocio/TipoNegocioRepository.hpp"

inline void tiponegocio_module() {
  using namespace jinject;

  UNIQUE(TipoNegocioRepository) {
    return new TipoNegocioRepository{inject<std::shared_ptr<jdb::Database>>()};
  };
}
