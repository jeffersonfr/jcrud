#pragma once

#include "jdb/database/DataClass.hpp"
#include "model/levelLog/LevelLogModel.hpp"
#include "model/tipoLog/TipoLogModel.hpp"

using LogModel = jdb::DataClass<"log", jdb::Primary<"id">, jdb::Foreign<jdb::Refer<LevelLogModel, "level_log_id">, jdb::Refer<TipoLogModel, "tipo_log_id">>,
  jdb::Field<"id", jdb::FieldType::Serial, false>,
  jdb::Field<"level_log_id", jdb::FieldType::Int, false>,
  jdb::Field<"tipo_log_id", jdb::FieldType::Int, false>,
  jdb::Field<"localizacao", jdb::FieldType::Text, false>,
  jdb::Field<"tag", jdb::FieldType::Text, false>,
  jdb::Field<"descricao", jdb::FieldType::Text, false>,
  jdb::Field<"last", jdb::FieldType::Text, false, jdb::TimestampValue>>;
