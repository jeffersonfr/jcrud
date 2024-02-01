#pragma once

#include "strategy/DecorateTitleStrategy.hpp"

#include <string>

#include "fmt/format.h"

struct DecorateTitleStrategyAmericanas : public DecorateTitleStrategy {
  std::string createFancyTitle(std::string const &title) override {
    return fmt::format(":: --- {} --- ::", title);
  }
};
