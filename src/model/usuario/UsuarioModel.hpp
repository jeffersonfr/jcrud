#pragma once

#include "jdb/database/DataClass.hpp"
#include "model/cargo/CargoModel.hpp"

using UsuarioModel = jdb::DataClass<"usuario", jdb::Primary<"id">, jdb::NoForeign,
  jdb::Field<"id", jdb::FieldType::Serial, false>,
  jdb::Field<"nome", jdb::FieldType::Text>,
  jdb::Field<"cpf", jdb::FieldType::Text>,
  jdb::Field<"telefone", jdb::FieldType::Text>,
  jdb::Field<"email", jdb::FieldType::Text>,
  jdb::Field<"senha", jdb::FieldType::Text>,
  jdb::Field<"apelido", jdb::FieldType::Text>,
  jdb::Field<"excluido", jdb::FieldType::Bool, false, jdb::DefaultValue<"false">>>;
