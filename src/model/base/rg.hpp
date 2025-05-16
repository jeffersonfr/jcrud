#pragma once

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

private:
  std::string mValue;

  explicit Rg(std::string value) : mValue{std::move(value)} {
  }

  [[nodiscard]] bool is_valid() const {
    // TODO::
    return true;
  }
};
