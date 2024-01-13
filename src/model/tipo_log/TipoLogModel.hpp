#pragma once

#include "model/DataClass.hpp"

enum class TipoLog {
  System,
  Model
};

using TipoLogModel = DataClass<"tipo_log", Primary<"id">, NoForeign,
  Field<"id", FieldType::Serial, false>,
  Field<"descricao", FieldType::Text, false>>;
