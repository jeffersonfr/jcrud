#pragma once

#include "database/DataClass.hpp"

enum class TipoLog {
  Sistema = 1,
  Modelo = 2
};

using TipoLogModel = DataClass<"tipo_log", Primary<"id">, NoForeign,
  Field<"id", FieldType::Serial, false>,
  Field<"descricao", FieldType::Text, false>>;
