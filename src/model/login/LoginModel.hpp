#pragma once

#include "jdb/database/DataClass.hpp"

using LoginModel = jdb::DataClass<
    "login", jdb::Primary<"filial_id", "usuario_id">,
    jdb::Foreign<jdb::Refer<FilialModel, "filial_id">, jdb::Refer<UsuarioModel, "usuario_id">>,
    jdb::Field<"filial_id", jdb::FieldType::Int, false>,
    jdb::Field<"usuario_id", jdb::FieldType::Int, false>,
    jdb::Field<"logado", jdb::FieldType::Bool>, jdb::Field<"last", jdb::FieldType::Timestamp, false, jdb::TimestampValue>>;
