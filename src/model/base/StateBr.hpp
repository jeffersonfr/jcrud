#pragma once

#include "jmixin/jstring.h"

#include <algorithm>
#include <optional>
#include <string>

struct StateBr {
  static std::optional<StateBr> from(std::optional<std::string> value) {
    return value
      .and_then([](std::string const &value) -> std::optional<StateBr> {
        StateBr stateBr{value};

        if (!stateBr.is_valid()) {
          return {};
        }

        return stateBr;
      });
  }

  std::string const &value() {
    return mValue;
  }

  std::string const &location() {
    return mLocation;
  }

private:
  std::string mValue, mLocation;

  explicit StateBr(std::string value)
    : mValue{jmixin::String{value}.upper_case()}
  {
  }

  [[nodiscard]] bool is_valid() {
    return is_stateBr_valid(mValue);
  }

  bool is_stateBr_valid(std::string value) {
    if (value.size() != 2) {
      return false;
    }

    // Remove todos os caracteres não numéricos
    std::erase_if(value, [](char c) {
        return !isalpha(c);
    });

    if (value == "AC") {
      mLocation = "Acre";
    } else if (value == "AL") {
      mLocation = "Alagoas";
    } else if (value == "AP") {
      mLocation = "Amapá";
    } else if (value == "AM") {
      mLocation = "Amazonas";
    } else if (value == "BA") {
      mLocation = "Bahia";
    } else if (value == "CE") {
      mLocation = "Ceará";
    } else if (value == "DF") {
      mLocation = "Distrito Federal";
    } else if (value == "ES") {
      mLocation = "Espírito Santo";
    } else if (value == "GO") {
      mLocation = "Goiás";
    } else if (value == "MA") {
      mLocation = "Maranhão";
    } else if (value == "MG") {
      mLocation = "Minas Gerais";
    } else if (value == "MS") {
      mLocation = "Mato Grosso do Sul";
    } else if (value == "MT") {
      mLocation = "Mato Grosso";
    } else if (value == "PA") {
      mLocation = "Pará";
    } else if (value == "PB") {
      mLocation = "Paraíba";
    } else if (value == "PR") {
      mLocation = "Paraná";
    } else if (value == "PE") {
      mLocation = "Pernambuco";
    } else if (value == "PI") {
      mLocation = "Piauí";
    } else if (value == "RJ") {
      mLocation = "Rio de Janeiro";
    } else if (value == "RN") {
      mLocation = "Rio Grande do Norte";
    } else if (value == "RS") {
      mLocation = "Rio Grande do Sul";
    } else if (value == "RO") {
      mLocation = "Rondônia";
    } else if (value == "RR") {
      mLocation = "Roraima";
    } else if (value == "SC") {
      mLocation = "Santa Catarina";
    } else if (value == "SP") {
      mLocation = "São Paulo";
    } else if (value == "SE") {
      mLocation = "Sergipe";
    } else if (value == "TO") {
      mLocation = "Tocantins";
    }

    return mLocation.empty();
  }
};
