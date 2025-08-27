#pragma once

#include "jdb/database/DataClass.hpp"

enum class LevelLog {
  Trace = 1,
  Debug = 2,
  Info = 3,
  Warn = 4,
  Error = 5,
  Fatal = 6
};

using LevelLogModel = jdb::DataClass<"level_log", jdb::Primary<"id">, jdb::NoForeign,
  jdb::Field<"id", jdb::FieldType::Serial, false>,
  jdb::Field<"descricao", jdb::FieldType::Text, false>>;
