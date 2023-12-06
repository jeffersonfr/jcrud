#pragma once

#include "model/DataClass.hpp"

using LogModel = DataClass<"log", PrimaryKeys<"id">,
  Field<"id", FieldType::Int, false>,
  Field<"description", FieldType::Text, false>>;
