#pragma once

#include "database/DataClass.hpp"

using SessionCredentialModel = DataClass<"session_credential", Primary<"id">, NoForeign,
  Field<"id", FieldType::Text, false>, // JWT ID
  Field<"key", FieldType::Text, false>,
  Field<"token", FieldType::Text>,
  Field<"refresh_token", FieldType::Text>,
  Field<"expires", FieldType::Timestamp>>;
