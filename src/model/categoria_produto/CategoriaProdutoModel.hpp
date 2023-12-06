#pragma once

#include "model/DataClass.hpp"

using CategoriaProdutoModel = DataClass<"categoria_produto", PrimaryKeys<"id">,
  Field<"id", FieldType::Int, false>,
  Field<"description", FieldType::Text>>;
