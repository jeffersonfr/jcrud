#pragma once

#include "database/DataClass.hpp"
#include "model/categoriaProduto/CategoriaProdutoModel.hpp"

using ProdutoModel = DataClass<"produto", Primary<"id">, Foreign<Refer<CategoriaProdutoModel, "categoria_id">>,
  Field<"id", FieldType::Serial, false>,
  Field<"categoria_id", FieldType::Int>,
  Field<"nome", FieldType::Text>,
  Field<"descricao", FieldType::Text>,
  Field<"excluido", FieldType::Bool, false, DefaultValue<"false">>>;
