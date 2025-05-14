#pragma once

#include "database/DataClass.hpp"
#include "model/tipoNegocio/TipoNegocioModel.hpp"
#include "model/produto/ProdutoModel.hpp"

using EstoqueModel = DataClass<"estoque", Primary<"id">, Foreign<Refer<ProdutoModel, "produto_id">>,
  Field<"id", FieldType::Serial, false>,
  Field<"produto_id", FieldType::Int, false>,
  Field<"quantidade", FieldType::Int, false>,
  Field<"lote", FieldType::Text, false>,
  Field<"validade", FieldType::Timestamp>>;
