#pragma once

#include "model/categoria_produto/CategoriaProdutoModel.hpp"
#include "model/produto/ProdutoModel.hpp"

#include <optional>
#include <string>

struct ProdutoInteractorModel {
  CategoriaProdutoModel categoria;
  ProdutoModel produto;
};

// using ProdutoInteractorModel = InteractorModel<CategoriaProdutoModel, ProdutoModel>;