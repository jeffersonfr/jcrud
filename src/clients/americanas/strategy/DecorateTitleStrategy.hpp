#pragma once

#include "strategy/decorateTitle/DecorateTitleStrategy.hpp"

#include <string>
#include <format>

struct DecorateTitleStrategyAmericanas : public DecorateTitleStrategy {
  std::string createFancyTitle(std::string const &title) override {
    return std::format(":: --- {} --- ::", title);
  }
};
