//
// Created by jeff on 5/19/25.
//

#pragma once

#define __MSG inline static char const *

class ErrorMsg {

public:
  __MSG RG_INVALIDO = "RG invalido.";
  __MSG CNPJ_INVALIDO = "CNPJ invalido.";

  __MSG ID_USUARIO_INVALIDO = "Id do usuario invalido.";
  __MSG ID_PRODUTO_INVALIDO = "Id do produto invalido.";

  __MSG LISTA_CARGOS_VAZIA = "Lista de cargos vazia.";
};
