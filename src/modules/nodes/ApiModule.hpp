#pragma once

#include "model/sessionCredential/SessionCredentialRepository.hpp"
#include "database/SqliteDatabase.hpp"
#include "api/RoutesV1.hpp"

void api_module() {
  using namespace jinject;

  using MyDatabase = SqliteDatabase<SessionCredentialModel>;

  SINGLE(std::shared_ptr<Database>, SessionCredentialModel) {
    auto db = std::make_shared<MyDatabase>(":memory:");

    db->add_migration(Migration{
        1, [](Database &db) {
          insert<SessionCredentialModel, "id", "key">(db)
            .values("48f8f811653246de9abf4940312b5ed2", "cb9aa2901bfd"); // JWT-ID, JWT-KEY
        }
      })
      .build();

    return db;
  };

  UNIQUE(SessionCredentialRepository) {
    return new SessionCredentialRepository{inject<std::shared_ptr<Database>, SessionCredentialModel>()};
  };

  UNIQUE(Routes) {
    return new RoutesV1{};
  };
}
