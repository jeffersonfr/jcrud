#pragma once

#include "database/DataClass.hpp"

using CategoriaProdutoModel = DataClass<"categoria_produto", Primary<"id">, NoForeign,
  Field<"id", FieldType::Serial, false>,
  Field<"descricao", FieldType::Text>>;
