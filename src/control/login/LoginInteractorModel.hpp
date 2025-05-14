#pragma once

#include "database/CompoundModel.hpp"
#include "model/filial/FilialModel.hpp"
#include "model/cargo/CargoModel.hpp"
#include "model/usuario/UsuarioModel.hpp"
#include "model/cargoUsuario/CargoUsuarioModel.hpp"
#include "model/login/LoginModel.hpp"

using LoginInteractorModel = CompoundModel<FilialModel, CargoModel, UsuarioModel, LoginModel>;