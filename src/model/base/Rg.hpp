#pragma once

#include <algorithm>
#include <optional>
#include <string>

struct Rg {
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

  std::string get_location() {
    return mLocation;
  }

private:
  std::string mValue, mLocation;

  explicit Rg(std::string value)
    : mValue{std::move(value)}
  {
    mLocation = process_location().value_or("desconhecido");
  }

  [[nodiscard]] bool is_valid() const {
    return is_rg_valid(mValue);
  }

  static bool is_rg_valid(std::string cpf) {
    // Remove todos os caracteres não numéricos
    std::erase_if(cpf, [](char c) {
        return !isdigit(c);
    });

    // Verifica se tem 7 dígitos
    if (cpf.length() != 7) {
      return false;
    }

    return true;
  }
};
