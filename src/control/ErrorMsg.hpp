#pragma once

#define __MSG inline static std::string

using namespace jinject;

struct ErrorMsg {
  __MSG RG_INVALIDO = _T("RG invalido");
  __MSG CNPJ_INVALIDO = _T("CNPJ invalido");

  __MSG ID_USUARIO_INVALIDO = _T("Id do usuario invalido");
  __MSG ID_PRODUTO_INVALIDO = _T("Id do produto invalido");
  __MSG ID_ESTOQUE_INVALIDO = _T("Id do estoque invalido");

  __MSG LISTA_CARGOS_VAZIA = _T("Lista de cargos vazia");

  __MSG QUANTIDADE_INVALIDA = _T("Quantidade superior ao limite disponivel");

  __MSG ATUALIZACAO_SENHA_INVALIDA = _T("Erro ao atualizar a senha");
};
