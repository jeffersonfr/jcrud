#pragma once

#include "model/CompoundModel.hpp"
#include "model/categoria_produto/CategoriaProdutoModel.hpp"
#include "model/produto/ProdutoModel.hpp"
#include "model/preco/PrecoModel.hpp"

using ProdutoInteractorModel = CompoundModel<CategoriaProdutoModel, ProdutoModel, PrecoModel>;