#pragma once

#include "jdb/database/DataClass.hpp"

using PrecoModel = jdb::DataClass<"preco", jdb::Primary<"id">, jdb::Foreign<jdb::Refer<ProdutoModel, "produto_id">>,
  jdb::Field<"id", jdb::FieldType::Serial, false>,
  jdb::Field<"produto_id", jdb::FieldType::Int>,
  jdb::Field<"valor", jdb::FieldType::Decimal>,
  jdb::Field<"last", jdb::FieldType::Timestamp, false, jdb::TimestampValue>>;
