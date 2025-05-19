#pragma once

#include <algorithm>
#include <optional>
#include <string>

class Rg {
public:
  static std::optional<Rg> from(std::optional<std::string> value) {
    return value
      .and_then([](std::string const &value) -> std::optional<Rg> {
        Rg rg{value};

        if (!rg.is_valid()) {
          return {};
        }

        return rg;
      });
  }

  std::string const &value() {
    return mValue;
  }

  std::optional<std::string> localtion() {
    char codigo = mValue[8];

    if (codigo == '0') {
      return "Rio Grande do Sul";
    }

    if (codigo == '1') {
      return "Distrito Federal, Goiás, Mato Grosso, Mato Grosso do Sul e Tocantins";
    }

    if (codigo == '2') {
      return "Amazonas, Pará, Roraima, Amapá, Acre e Rondônia";
    }

    if (codigo == '3') {
      return "Ceará, Maranhão e Piauí";
    }

    if (codigo == '4') {
      return "Paraíba, Pernambuco, Alagoas e Rio Grande do Norte";
    }

    if (codigo == '5') {
      return "Bahia e Sergipe";
    }

    if (codigo == '6') {
      return "Minas Gerais";
    }

    if (codigo == '7') {
      return "Rio de Janeiro e Espírito Santo";
    }

    if (codigo == '8') {
      return "São Paulo";
    }

    if (codigo == '9') {
      return "Paraná e Santa Catarina";
    }

    return {};
  }

private:
  std::string mValue;

  explicit Rg(std::string value) : mValue{std::move(value)} {
  }

  [[nodiscard]] bool is_valid() const {
    return is_cpf_valid(mValue);
  }

  bool is_cpf_valid(std::string cpf) const {
    // Remove todos os caracteres não numéricos
    cpf.erase(std::remove_if(cpf.begin(), cpf.end(), [](char c) {
        return !isdigit(c);
    }), cpf.end());

    // Verifica se tem 11 dígitos
    if (cpf.length() != 11) {
      return false;
    }

    // Verifica se todos os dígitos são iguais (CPF inválido)
    bool todos_iguais = true;
    for (size_t i = 1; i < cpf.length(); i++) {
      if (cpf[i] != cpf[0]) {
        todos_iguais = false;
        break;
      }
    }
    if (todos_iguais) {
      return false;
    }

    // Cálculo do primeiro dígito verificador
    int soma = 0;
    for (int i = 0; i < 9; i++) {
      soma += (cpf[i] - '0') * (10 - i);
    }
    int resto = soma % 11;
    int digito1 = (resto < 2) ? 0 : (11 - resto);

    // Verifica o primeiro dígito verificador
    if ((cpf[9] - '0') != digito1) {
      return false;
    }

    // Cálculo do segundo dígito verificador
    soma = 0;
    for (int i = 0; i < 10; i++) {
      soma += (cpf[i] - '0') * (11 - i);
    }
    resto = soma % 11;
    int digito2 = (resto < 2) ? 0 : (11 - resto);

    // Verifica o segundo dígito verificador
    if ((cpf[10] - '0') != digito2) {
      return false;
    }

    return true;
  }

};
