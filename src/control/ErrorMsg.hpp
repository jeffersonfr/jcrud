#pragma once

#define __MSG inline static std::string

class ErrorMsg {

public:
  __MSG RG_INVALIDO = "RG invalido.";
  __MSG CNPJ_INVALIDO = "CNPJ invalido.";

  __MSG ID_USUARIO_INVALIDO = "Id do usuario invalido.";
  __MSG ID_PRODUTO_INVALIDO = "Id do produto invalido.";

  __MSG LISTA_CARGOS_VAZIA = "Lista de cargos vazia.";
};
