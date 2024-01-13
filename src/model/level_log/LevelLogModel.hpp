#pragma once

#include "model/DataClass.hpp"

enum class LevelLog {
  Debug,
  Info,
  Warn,
  Error,
  Fatal
};

using LevelLogModel = DataClass<"level_log", Primary<"id">, NoForeign,
  Field<"id", FieldType::Serial, false>,
  Field<"descricao", FieldType::Text, false>>;
