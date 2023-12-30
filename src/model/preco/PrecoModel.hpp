#pragma once

#include "model/DataClass.hpp"

using PrecoModel = DataClass<"preco", Primary<"id">, Foreign<Refer<ProdutoModel, "produto_id">>,
  Field<"id", FieldType::Serial, false>,
  Field<"produto_id", FieldType::Int>,
  Field<"valor", FieldType::Decimal>,
  Field<"last", FieldType::Timestamp, false, TimestampValue>>;
