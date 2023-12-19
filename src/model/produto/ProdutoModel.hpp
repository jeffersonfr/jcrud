#pragma once

#include "model/DataClass.hpp"

using ProdutoModel = DataClass<"produto", PrimaryKeys<"id">,
  Field<"id", FieldType::Serial, false>,
  Field<"categoria_id", FieldType::Int>,
  Field<"nome", FieldType::Text>,
  Field<"descricao", FieldType::Text>,
  Field<"validade", FieldType::Text>>;
