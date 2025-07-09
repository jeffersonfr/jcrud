#pragma once

#include "model/cargo/CargoRepository.hpp"
#include "model/cargoUsuario/CargoUsuarioRepository.hpp"
#include "model/filial/FilialRepository.hpp"
#include "model/historicoEstoque/HistoricoEstoqueRepository.hpp"
#include "model/levelLog/LevelLogRepository.hpp"
#include "model/preco/PrecoRepository.hpp"
#include "model/usuario/UsuarioRepository.hpp"
#include "model/tipoNegocio/TipoNegocioRepository.hpp"
#include "model/sessionCredential/SessionCredentialRepository.hpp"

void preco_module() {
  using namespace jinject;

  UNIQUE(PrecoRepository) {
    return new PrecoRepository{inject<std::shared_ptr<Database>>()};
  };
}
