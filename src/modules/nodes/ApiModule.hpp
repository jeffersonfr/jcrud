#pragma once

#include "model/sessionCredential/SessionCredentialRepository.hpp"
#include "api/v1/control/session/JwtSessionInteractor.hpp"
#include "database/SqliteDatabase.hpp"

/*
 * Encoder e decoder de JWT online:
 *
 * https://10015.io/tools/jwt-encoder-decoder
 */
void api_module() {
  using namespace jinject;

  using MyDatabase = SqliteDatabase<SessionCredentialModel>;

  SINGLE(std::shared_ptr<Database>, SessionCredentialModel) {
    auto db = std::make_shared<MyDatabase>(":memory:");

    db->add_migration(Migration{
        1, [](Database &db) {
          insert<SessionCredentialModel, "id", "key", "usuario_id">(db)
            .values("48f8f811653246de9abf4940312b5ed2", "cb9aa2901bfd", 1)
            .values("487652f8f81166de9abf4940312b5e32", "cb56732cbb91", 2);
        }
      })
      .build();

    return db;
  };

  UNIQUE(SessionCredentialRepository) {
    return new SessionCredentialRepository{inject<std::shared_ptr<Database>, SessionCredentialModel>()};
  };

  SINGLE(std::shared_ptr<JwtSessionInteractor>) {
    return std::make_shared<JwtSessionInteractor>(get{}, get{});
  };
}
