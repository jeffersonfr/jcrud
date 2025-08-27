#pragma once

#include "model/categoriaProduto/CategoriaProdutoModel.hpp"
#include "model/produto/ProdutoModel.hpp"
#include "model/preco/PrecoModel.hpp"
#include "jdb/database/CompoundModel.hpp"

using ProdutoInteractorModel = jdb::CompoundModel<CategoriaProdutoModel, ProdutoModel, PrecoModel>;
