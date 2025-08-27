#pragma once

#include "model/categoriaProduto/CategoriaProdutoRepository.hpp"

void categoriaproduto_module() {
  using namespace jinject;

  UNIQUE(CategoriaProdutoRepository) {
    return new CategoriaProdutoRepository{
        inject<std::shared_ptr<jdb::Database>>()};
  };
}
