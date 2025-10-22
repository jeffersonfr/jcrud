#pragma once

#include "model/levelLog/LevelLogRepository.hpp"
#include "model/preco/PrecoRepository.hpp"

inline void preco_module() {
  using namespace jinject;

  UNIQUE(PrecoRepository) {
    return new PrecoRepository{inject<std::shared_ptr<jdb::Database>>()};
  };
}
