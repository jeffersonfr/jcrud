#pragma once

#include "model/DataClass.hpp"

using PrecoModel = DataClass<"preco", Primary<"id">, NoForeign,
  Field<"id", FieldType::Serial, false>,
  Field<"produto_id", FieldType::Int>,
  Field<"valor", FieldType::Decimal>,
  Field<"timestamp", FieldType::Text>>;
