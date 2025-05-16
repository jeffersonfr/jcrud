#pragma once

#include "control/login/LoginInteractorModel.hpp"

#include <optional>

struct Ambiente {
  inline static std::optional<UsuarioModel> usuario;
  inline static std::optional<std::vector<CargoModel> > cargos;
};
