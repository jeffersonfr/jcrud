#pragma once

#include "jdb/database/DataClass.hpp"

enum class Cargo {
  Administrador = 1,
  Operador = 2
};

using CargoModel = jdb::DataClass<"cargo", jdb::Primary<"id">, jdb::NoForeign,
  jdb::Field<"id", jdb::FieldType::Serial, false>,
  jdb::Field<"descricao", jdb::FieldType::Text>>;
