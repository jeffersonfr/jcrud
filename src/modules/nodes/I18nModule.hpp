#pragma once

enum i18n_module_enum {
  US_en
};

inline void i18n_module() {
  using namespace jinject;

  // error messages
  TYPED("RG invalido")
    .add(US_en, "Invalid ID");

  TYPED("CNPJ invalido")
    .add(US_en, "Invalid CNPJ");

  TYPED("Id do usuario invalido")
    .add(US_en, "Invalid user ID");

  TYPED("Id do produto invalido")
    .add(US_en, "Invalid product ID");

  TYPED("Id do estoque invalido")
    .add(US_en, "Invalid Inventory ID");

  TYPED("Lista de cargos vazia")
    .add(US_en, "Empty roles list");

  TYPED("Quantidade superior ao limite disponivel")
    .add(US_en, "Limit overflow");

  TYPED("Erro ao atualizar a senha")
  .add(US_en, "Unable to update password");

  // api error messages
  TYPED("Sem cabeçalho de autorização")
    .add(US_en, "No authorization header");

  TYPED("Credenciais inválidas")
    .add(US_en, "Invalid credentials");

  TYPED("Acesso negado")
    .add(US_en, "Forbidden access");

  TYPED("Token expirado")
    .add(US_en, "Token expired");

}
