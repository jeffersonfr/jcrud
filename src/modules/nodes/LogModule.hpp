#pragma once

#include "model/log/LogRepository.hpp"
#include "model/tipoLog/TipoLogRepository.hpp"
#include "database/SqliteDatabase.hpp"

void log_module() {
  using namespace jinject;

  using MyDatabase = SqliteDatabase<LevelLogModel, TipoLogModel, LogModel>;

  SINGLE(std::shared_ptr<Database>, LogModel) {
    auto db = std::make_shared<MyDatabase>("log.db");

    db->add_migration(Migration{
          1, [](Database &db) {
            insert<LevelLogModel, "descricao">(db)
                .values("Trace")
                .values("Debug")
                .values("Info")
                .values("Warn")
                .values("Error")
                .values("Fatal");

            insert<TipoLogModel, "descricao">(db).values("Sistema").values(
              "Modelo");
          }
        })
        .build();

    return db;
  };

  UNIQUE(LogRepository) {
    return new LogRepository{inject<std::shared_ptr<Database>, LogModel>()};
  };
}
