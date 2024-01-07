#pragma once

#include "model/DataClass.hpp"

enum class Cargo {
  Administrador = 1,
  Operador = 2
};

using CargoModel = DataClass<"cargo", Primary<"id">, NoForeign,
  Field<"id", FieldType::Serial, false>,
  Field<"descricao", FieldType::Text>>;
