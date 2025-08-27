#pragma once

#include "jdb/database/DataClass.hpp"

using CategoriaProdutoModel = jdb::DataClass<"categoria_produto", jdb::Primary<"id">, jdb::NoForeign,
  jdb::Field<"id", jdb::FieldType::Serial, false>,
  jdb::Field<"descricao", jdb::FieldType::Text>>;
