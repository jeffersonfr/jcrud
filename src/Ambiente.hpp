#pragma once

#include "control/login/LoginInteractorModel.hpp"
#include "config.h"

#include <optional>

struct Ambiente {
  inline static std::string version = GIT_SHORT_COMMIT_HASH;

  inline static std::optional<UsuarioModel> usuario;
  inline static std::optional<std::vector<CargoModel> > cargos;
};
