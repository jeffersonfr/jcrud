#pragma once

#include "database/DataClass.hpp"
#include "model/levelLog/LevelLogModel.hpp"
#include "model/tipoLog/TipoLogModel.hpp"

using LogModel = DataClass<"log", Primary<"id">, Foreign<Refer<LevelLogModel, "level_log_id">, Refer<TipoLogModel, "tipo_log_id">>,
  Field<"id", FieldType::Serial, false>,
  Field<"level_log_id", FieldType::Int, false>,
  Field<"tipo_log_id", FieldType::Int, false>,
  Field<"localizacao", FieldType::Text, false>,
  Field<"tag", FieldType::Text, false>,
  Field<"descricao", FieldType::Text, false>,
  Field<"last", FieldType::Text, false, TimestampValue>>;
