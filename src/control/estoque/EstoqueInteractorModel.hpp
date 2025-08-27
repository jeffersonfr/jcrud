#pragma once

#include "model/estoque/EstoqueModel.hpp"
#include "model/tipoNegocio/TipoNegocioModel.hpp"
#include "model/produto/ProdutoModel.hpp"
#include "jdb/database/CompoundModel.hpp"

using EstoqueInteractorModel = jdb::CompoundModel<EstoqueModel, ProdutoModel, CategoriaProdutoModel>;
