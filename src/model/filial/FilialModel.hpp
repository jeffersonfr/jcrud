#pragma once

#include "jdb/database/DataClass.hpp"

using FilialModel = jdb::DataClass<"filial", jdb::Primary<"id">, jdb::NoForeign,
  jdb::Field<"id", jdb::FieldType::Serial, false>,
  jdb::Field<"nome", jdb::FieldType::Text>,
  jdb::Field<"endereco", jdb::FieldType::Text>>;
