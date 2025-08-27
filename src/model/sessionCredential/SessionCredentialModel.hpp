#pragma once

#include "jdb/database/DataClass.hpp"

using SessionCredentialModel = jdb::DataClass<"session_credential", jdb::Primary<"id">, jdb::Foreign<jdb::Refer<UsuarioModel, "usuario_id">>,
  jdb::Field<"id", jdb::FieldType::Text, false>, // JWT ID
  jdb::Field<"key", jdb::FieldType::Text, false>, // JWT KEY
  jdb::Field<"usuario_id", jdb::FieldType::Int, false>>;
