#pragma once

#include "model/historicoEstoque/HistoricoEstoqueRepository.hpp"

inline void historicoestoque_module() {
  using namespace jinject;

  UNIQUE(HistoricoEstoqueRepository) {
    return new HistoricoEstoqueRepository{
        inject<std::shared_ptr<jdb::Database>>()};
  };
}
