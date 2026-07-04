#pragma once

#define __MSG inline static std::string

using namespace jinject;

struct ApiErrorMsg {
  __MSG NO_CREDENTIALS = _T("Sem cabeçalho de autorização");
  __MSG INVALID_CREDENTIALS = _T("Credenciais inválidas");
  __MSG FORBIDDEN_ACCESS = _T("Acesso negado");
  __MSG TOKEN_EXPIRED = _T("Token expirado");
};
