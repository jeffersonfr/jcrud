#pragma once

#include "database/CompoundModel.hpp"
#include "model/categoriaProduto/CategoriaProdutoModel.hpp"
#include "model/produto/ProdutoModel.hpp"
#include "model/preco/PrecoModel.hpp"

using ProdutoInteractorModel = CompoundModel<CategoriaProdutoModel, ProdutoModel, PrecoModel>;
