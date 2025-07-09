#pragma once

#include "model/usuario/UsuarioRepository.hpp"

void usuario_module() {
  using namespace jinject;

  UNIQUE(UsuarioRepository) {
    return new UsuarioRepository{inject<std::shared_ptr<Database>>()};
  };
}
