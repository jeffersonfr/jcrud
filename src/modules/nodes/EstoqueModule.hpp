#pragma once

#include "control/estoque/EstoqueController.hpp"

inline void estoque_module() {
  using namespace jinject;

  UNIQUE(EstoqueRepository) {
    return new EstoqueRepository{inject<std::shared_ptr<jdb::Database>>()};
  };

  UNIQUE(EstoqueInteractorRepository) {
    return new EstoqueInteractorRepository{};
  };

  UNIQUE(EstoqueInteractor) {
    return new EstoqueInteractor{get{}, get{}, get{}};
  };

  UNIQUE(EstoqueController) {
    return new EstoqueController{
        inject<std::unique_ptr<EstoqueInteractor>>()};
  };
}
