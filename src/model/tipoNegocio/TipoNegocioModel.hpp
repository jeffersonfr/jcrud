#pragma once

#include "model/DataClass.hpp"

enum class TipoNegocio {
  Compra = 1,
  Venda = 2
};

using TipoNegocioModel = DataClass<"tipo_negocio", Primary<"id">, NoForeign,
  Field<"id", FieldType::Serial, false>,
  Field<"descricao", FieldType::Text, false>>;
