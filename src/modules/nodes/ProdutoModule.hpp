#pragma once

#include "control/produto/ProdutoController.hpp"

void produto_module() {
  using namespace jinject;

  UNIQUE(ProdutoRepository) {
    return new ProdutoRepository{inject<std::shared_ptr<jdb::Database>>()};
  };

  UNIQUE(ProdutoInteractorRepository) {
    return new ProdutoInteractorRepository{};
  };

  UNIQUE(ProdutoInteractor) {
    return new ProdutoInteractor{get{}, get{}, get{}, get{}};
  };

  UNIQUE(ProdutoController) {
    return new ProdutoController{
        inject<std::unique_ptr<ProdutoInteractor>>()};
  };
}
