#pragma once

#include "database/DataClass.hpp"

using SessionCredentialModel = DataClass<"session_credential", Primary<"id">, Foreign<Refer<UsuarioModel, "usuario_id">>,
  Field<"id", FieldType::Text, false>, // JWT ID
  Field<"key", FieldType::Text, false>, // JWT KEY
  Field<"usuario_id", FieldType::Int, false>>;
