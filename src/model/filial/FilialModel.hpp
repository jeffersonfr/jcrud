#pragma once

#include "database/DataClass.hpp"

using FilialModel = DataClass<"filial", Primary<"id">, NoForeign,
  Field<"id", FieldType::Serial, false>,
  Field<"nome", FieldType::Text>,
  Field<"endereco", FieldType::Text>>;
