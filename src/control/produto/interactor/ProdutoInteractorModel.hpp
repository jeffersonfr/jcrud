#pragma once

#include "control/InteractorModel.hpp"
#include "model/categoria_produto/CategoriaProdutoModel.hpp"
#include "model/produto/ProdutoModel.hpp"
#include "model/preco/PrecoModel.hpp"

#include <optional>
#include <string>

using ProdutoInteractorModel = InteractorModel<CategoriaProdutoModel, ProdutoModel, PrecoModel>;