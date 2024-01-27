#pragma once

#include "model/DataClass.hpp"
#include "model/tipoNegocio/TipoNegocioModel.hpp"

using EstoqueModel = DataClass<"estoque", Primary<"produto_id", "lote">, Foreign<Refer<ProdutoModel, "produto_id">>,
  Field<"produto_id", FieldType::Int, false>,
  Field<"quantidade", FieldType::Int>,
  Field<"lote", FieldType::Int, false>,
  Field<"validade", FieldType::Timestamp>>;
