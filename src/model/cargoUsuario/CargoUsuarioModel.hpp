#pragma once

#include "database/DataClass.hpp"

using CargoUsuarioModel = DataClass<"cargo_usuario", Primary<"cargo_id", "usuario_id">, Foreign<Refer<CargoModel, "cargo_id">, Refer<UsuarioModel, "usuario_id">>,
  Field<"cargo_id", FieldType::Int, false>,
  Field<"usuario_id", FieldType::Int, false>>;
