#pragma once

#include <optional>
#include <string>

class Id {
public:
  static std::optional<Id> from(std::optional<int64_t> value) {
    return value
      .and_then([](auto value) -> std::optional<Id> {
        Id id{value};

        if (!id.is_valid()) {
          return {};
        }

        return id;
      });
  }

  [[nodiscard]] int64_t value() const {
    return mValue;
  }

private:
  int64_t mValue;

  explicit Id(int64_t value) : mValue{value} {
  }

  [[nodiscard]] bool is_valid() const {
    return mValue > 0;
  }
};
