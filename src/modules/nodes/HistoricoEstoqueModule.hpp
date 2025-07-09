#pragma once

#include "model/historicoEstoque/HistoricoEstoqueRepository.hpp"

void historicoestoque_module() {
  using namespace jinject;

  UNIQUE(HistoricoEstoqueRepository) {
    return new HistoricoEstoqueRepository{
        inject<std::shared_ptr<Database>>()};
  };
}
