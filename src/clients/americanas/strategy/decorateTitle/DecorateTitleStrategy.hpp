#pragma once

#include "strategy/decorateTitle/DecorateTitleStrategy.hpp"

#include <string>

#include "fmt/format.h"

struct DecorateTitleStrategyAmericanas : public DecorateTitleStrategy {
  std::string createFancyTitle(std::string const &title) override {
    return fmt::format(":: --- {} --- ::", title);
  }
};
