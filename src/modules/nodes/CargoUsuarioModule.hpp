#pragma once

#include "model/cargoUsuario/CargoUsuarioRepository.hpp"

void cargousuario_module() {
  using namespace jinject;

  UNIQUE(CargoUsuarioRepository) {
    return new CargoUsuarioRepository{inject<std::shared_ptr<Database>>()};
  };
}
