#pragma once

#include "model/DataClass.hpp"

using LoginModel = DataClass<
    "login", Primary<"filial_id", "usuario_id">,
    Foreign<Refer<FilialModel, "filial_id">, Refer<UsuarioModel, "usuario_id">>,
    Field<"filial_id", FieldType::Int, false>,
    Field<"usuario_id", FieldType::Int, false>,
    Field<"logado", FieldType::Bool>, Field<"last", FieldType::Timestamp, false, TimestampValue>>;
