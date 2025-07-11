#pragma once

#include "modules/nodes/LogModule.hpp"
#include "modules/nodes/SistemaModule.hpp"
#include "modules/nodes/HistoricoEstoqueModule.hpp"
#include "modules/nodes/StrategyModule.hpp"
#include "modules/nodes/ApiModule.hpp"

#include "modules/nodes/LoginModule.hpp"
#include "modules/nodes/AdminModule.hpp"
#include "modules/nodes/ProdutoModule.hpp"
#include "modules/nodes/EstoqueModule.hpp"
#include "modules/nodes/CargoModule.hpp"
#include "modules/nodes/CargoUsuarioModule.hpp"
#include "modules/nodes/CategoriaProdutoModule.hpp"
#include "modules/nodes/FilialModule.hpp"
#include "modules/nodes/PrecoModule.hpp"
#include "modules/nodes/TipoNegocioModule.hpp"
#include "modules/nodes/UsuarioModule.hpp"

struct Modules final {
  Modules() = delete;

  static void load_modules() {
    log_module();

    login_module();
    usuario_module();
    cargo_module();
    cargousuario_module();
    admin_module();

    categoriaproduto_module();
    produto_module();
    estoque_module();
    preco_module();
    filial_module();
    tiponegocio_module();
    historicoestoque_module();

    strategy_module();
    sistema_module();
    api_module();
  }
};

//#endif
