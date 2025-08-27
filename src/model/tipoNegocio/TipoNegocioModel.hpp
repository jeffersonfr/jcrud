#pragma once

#include "jdb/database/DataClass.hpp"

enum class TipoNegocio {
  Compra = 1,
  Venda = 2
};

using TipoNegocioModel = jdb::DataClass<"tipo_negocio", jdb::Primary<"id">, jdb::NoForeign,
  jdb::Field<"id", jdb::FieldType::Serial, false>,
  jdb::Field<"descricao", jdb::FieldType::Text, false>>;
