#pragma once

#include "jdb/database/DataClass.hpp"

enum class TipoLog {
  Sistema = 1,
  Modelo = 2
};

using TipoLogModel = jdb::DataClass<"tipo_log", jdb::Primary<"id">, jdb::NoForeign,
  jdb::Field<"id", jdb::FieldType::Serial, false>,
  jdb::Field<"descricao", jdb::FieldType::Text, false>>;
