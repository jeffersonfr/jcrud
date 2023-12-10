#pragma once

#include "model/DataClass.hpp"

using PrecoModel = DataClass<"preco", PrimaryKeys<"id">,
  Field<"id", FieldType::Serial, false>,
  Field<"produto_id", FieldType::Int>,
  Field<"valor", FieldType::Decimal>>;
