#pragma once

#include "model/DataClass.hpp"

using LogModel = DataClass<"log", Primary<"id">, NoForeign,
  Field<"id", FieldType::Int, false>,
  Field<"descricao", FieldType::Text, false>>;
