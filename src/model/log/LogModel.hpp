#pragma once

#include "model/DataClass.hpp"
#include "model/level_log/LevelLogModel.hpp"
#include "model/tipo_log/TipoLogModel.hpp"

using LogModel = DataClass<"log", Primary<"id">, Foreign<Refer<LevelLogModel, "level_log_id">, Refer<TipoLogModel, "tipo_log_id">>,
  Field<"id", FieldType::Serial, false>,
  Field<"level_log_id", FieldType::Int, false>,
  Field<"tipo_log_id", FieldType::Int, false>,
  Field<"localizacao", FieldType::Text, false>,
  Field<"descricao", FieldType::Text, false>,
  Field<"last", FieldType::Text, false, TimestampValue>>;
