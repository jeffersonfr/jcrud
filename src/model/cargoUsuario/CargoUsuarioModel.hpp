#pragma once

#include "jdb/database/DataClass.hpp"

using CargoUsuarioModel = jdb::DataClass<"cargo_usuario", jdb::Primary<"cargo_id", "usuario_id">, jdb::Foreign<jdb::Refer<CargoModel, "cargo_id">, jdb::Refer<UsuarioModel, "usuario_id">>,
  jdb::Field<"cargo_id", jdb::FieldType::Int, false>,
  jdb::Field<"usuario_id", jdb::FieldType::Int, false>>;
