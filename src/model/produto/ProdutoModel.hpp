#pragma once

#include "jdb/database/DataClass.hpp"
#include "model/categoriaProduto/CategoriaProdutoModel.hpp"

using ProdutoModel = jdb::DataClass<"produto", jdb::Primary<"id">, jdb::Foreign<jdb::Refer<CategoriaProdutoModel, "categoria_id">>,
  jdb::Field<"id", jdb::FieldType::Serial, false>,
  jdb::Field<"categoria_id", jdb::FieldType::Int>,
  jdb::Field<"nome", jdb::FieldType::Text>,
  jdb::Field<"descricao", jdb::FieldType::Text>,
  jdb::Field<"excluido", jdb::FieldType::Bool, false, jdb::DefaultValue<"false">>>;
