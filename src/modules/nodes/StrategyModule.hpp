#pragma once

#if JCRUD_CLIENT == Americanas
#include "clients/americanas/strategy/decorateTitle/DecorateTitleStrategy.hpp"
#else
#include "strategy//DecorateTitleStrategy.hpp"
#endif

void strategy_module() {
  using namespace jinject;

#if JCRUD_CLIENT == Americanas
  UNIQUE(DecorateTitleStrategy) { return new DecorateTitleStrategyAmericanas{}; };
#else
  UNIQUE(DecorateTitleStrategy) { return new DecorateTitleStrategy{}; };
#endif

}
