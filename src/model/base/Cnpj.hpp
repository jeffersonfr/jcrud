#pragma once

#include <optional>
#include <string>

struct Cnpj {
  static std::optional<Cnpj> from(std::optional<std::string> value) {
    return value
      .and_then([](std::string const &value) -> std::optional<Cnpj> {
        Cnpj cnpj{value};

        if (!cnpj.is_valid()) {
          return {};
        }

        return cnpj;
      });
  }

  std::string const &value() {
    return mValue;
  }

private:
  std::string mValue;

  explicit Cnpj(std::string value) : mValue{std::move(value)} {
  }

  [[nodiscard]] bool is_valid() const {
    return is_cnpj_valid(mValue);
  }

  [[nodiscard]] static bool is_cnpj_valid(std::string cnpj) {
    // Remove todos os caracteres não numéricos
    cnpj.erase(std::ranges::remove_if(cnpj, [](char c) {
      return !isdigit(c);
    }).begin(), cnpj.end());

    // Verifica se tem 14 dígitos
    if (cnpj.length() != 14) {
      return false;
    }

    // Verifica se todos os dígitos são iguais (CNPJ inválido)
    bool todos_iguais = true;
    for (size_t i = 1; i < cnpj.length(); i++) {
      if (cnpj[i] != cnpj[0]) {
        todos_iguais = false;
        break;
      }
    }
    if (todos_iguais) {
      return false;
    }

    // Cálculo do primeiro dígito verificador
    int pesos1[12] = {5, 4, 3, 2, 9, 8, 7, 6, 5, 4, 3, 2};
    int soma = 0;
    for (int i = 0; i < 12; i++) {
      soma += (cnpj[i] - '0') * pesos1[i];
    }
    int resto = soma % 11;
    int digito1 = (resto < 2) ? 0 : (11 - resto);

    // Verifica o primeiro dígito verificador
    if ((cnpj[12] - '0') != digito1) {
      return false;
    }

    // Cálculo do segundo dígito verificador
    int pesos2[13] = {6, 5, 4, 3, 2, 9, 8, 7, 6, 5, 4, 3, 2};
    soma = 0;
    for (int i = 0; i < 13; i++) {
      soma += (cnpj[i] - '0') * pesos2[i];
    }
    resto = soma % 11;
    int digito2 = (resto < 2) ? 0 : (11 - resto);

    // Verifica o segundo dígito verificador
    if ((cnpj[13] - '0') != digito2) {
      return false;
    }

    return true;
  }

};
