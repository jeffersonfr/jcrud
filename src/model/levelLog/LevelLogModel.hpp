#pragma once

#include "database/DataClass.hpp"

enum class LevelLog {
  Trace = 1,
  Debug = 2,
  Info = 3,
  Warn = 4,
  Error = 5,
  Fatal = 6
};

using LevelLogModel = DataClass<"level_log", Primary<"id">, NoForeign,
  Field<"id", FieldType::Serial, false>,
  Field<"descricao", FieldType::Text, false>>;
