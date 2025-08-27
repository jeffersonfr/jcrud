#pragma once

#include "jdb/database/DataClass.hpp"
#include "model/tipoNegocio/TipoNegocioModel.hpp"
#include "model/produto/ProdutoModel.hpp"

using EstoqueModel = jdb::DataClass<"estoque", jdb::Primary<"id">, jdb::Foreign<jdb::Refer<ProdutoModel, "produto_id">>,
  jdb::Field<"id", jdb::FieldType::Serial, false>,
  jdb::Field<"produto_id", jdb::FieldType::Int, false>,
  jdb::Field<"quantidade", jdb::FieldType::Int, false>,
  jdb::Field<"lote", jdb::FieldType::Text, false>,
  jdb::Field<"validade", jdb::FieldType::Timestamp>>;
