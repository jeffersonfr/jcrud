#pragma once

#include "jdb/database/DataClass.hpp"
#include "model/tipoNegocio/TipoNegocioModel.hpp"

using HistoricoEstoqueModel = jdb::DataClass<"historico_estoque", jdb::Primary<"id">, jdb::Foreign<jdb::Refer<ProdutoModel, "produto_id">, jdb::Refer<TipoNegocioModel, "tipo_negocio_id">>,
  jdb::Field<"id", jdb::FieldType::Serial, false>,
  jdb::Field<"produto_id", jdb::FieldType::Int, false>,
  jdb::Field<"tipo_negocio_id", jdb::FieldType::Int, false>,
  jdb::Field<"quantidade", jdb::FieldType::Int>,
  jdb::Field<"lote", jdb::FieldType::Text, false>,
  jdb::Field<"validade", jdb::FieldType::Timestamp>,
  jdb::Field<"cnpj", jdb::FieldType::Text>>;
