#pragma once

#include "model/DataClass.hpp"

using LogModel = DataClass<"log", Primary<"id">, NoForeign,
  Field<"id", FieldType::Serial, false>,
  Field<"level", FieldType::Int, false>,
  Field<"tipo", FieldType::Int, false>,
  Field<"descricao", FieldType::Text, false>,
  Field<"last", FieldType::Text, false, TimestampValue>>;
