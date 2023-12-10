#pragma once

#include "model/DataClass.hpp"

using CategoriaProdutoModel = DataClass<"categoria_produto", PrimaryKeys<"id">,
  Field<"id", FieldType::Serial, false>,
  Field<"descricao", FieldType::Text>>;
