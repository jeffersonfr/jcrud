#pragma once

#include "model/CompoundModel.hpp"
#include "model/estoque/EstoqueModel.hpp"
#include "model/tipoNegocio/TipoNegocioModel.hpp"
#include "model/produto/ProdutoModel.hpp"

using EstoqueInteractorModel = CompoundModel<EstoqueModel, ProdutoModel, CategoriaProdutoModel>;