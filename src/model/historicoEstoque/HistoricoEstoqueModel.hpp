#pragma once

#include "model/DataClass.hpp"
#include "model/tipoNegocio/TipoNegocioModel.hpp"

using HistoricoEstoqueModel = DataClass<"historico_estoque", Primary<"id">, Foreign<Refer<ProdutoModel, "produto_id">, Refer<TipoNegocioModel, "tipo_negocio_id">>,
  Field<"id", FieldType::Serial, false>,
  Field<"produto_id", FieldType::Int, false>,
  Field<"tipo_negocio_id", FieldType::Int>,
  Field<"quantidade", FieldType::Int>,
  Field<"lote", FieldType::Int, false>,
  Field<"cnpj", FieldType::Int>>;
