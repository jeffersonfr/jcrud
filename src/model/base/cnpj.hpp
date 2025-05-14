#pragma once

#include <optional>
#include <string>

class Cnpj {

public:
  static std::optional<Cnpj> from(std::optional<std::string> value) {
    return value
      .and_then([](auto value) -> std::optional<Cnpj> {
        Cnpj cnpj{value};

        if (!cnpj.is_valid()) {
          return {};
        }

        return cnpj;
      });
  }

  std::string to_string() {
    return mValue;
  }

private:
  std::string mValue;

  Cnpj(std::string const &value) : mValue{value} {}

  bool is_valid() {
    // TODO::
    return true;
  }
};