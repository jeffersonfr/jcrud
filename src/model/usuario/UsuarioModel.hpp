#pragma once

#include "database/DataClass.hpp"
#include "model/cargo/CargoModel.hpp"

using UsuarioModel = DataClass<"usuario", Primary<"id">, NoForeign,
  Field<"id", FieldType::Serial, false>,
  Field<"nome", FieldType::Text>,
  Field<"cpf", FieldType::Text>,
  Field<"telefone", FieldType::Text>,
  Field<"email", FieldType::Text>,
  Field<"senha", FieldType::Text>,
  Field<"apelido", FieldType::Text>,
  Field<"excluido", FieldType::Bool, false, DefaultValue<"false">>>;
