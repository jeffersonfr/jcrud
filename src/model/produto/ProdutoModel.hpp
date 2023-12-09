#pragma once

#include "model/DataClass.hpp"

using ProdutoModel = DataClass<"produto", PrimaryKeys<"id">,
  Field<"id", FieldType::Serial, false>,
  Field<"category", FieldType::Int>,
  Field<"name", FieldType::Text>,
  Field<"description", FieldType::Text>,
  Field<"validate", FieldType::Text>,
  Field<"price", FieldType::Decimal>>;
