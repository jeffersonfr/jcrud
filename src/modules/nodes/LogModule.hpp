#pragma once

#include "model/log/LogRepository.hpp"
#include "model/tipoLog/TipoLogRepository.hpp"
#include "jdb/database/SqliteDatabase.hpp"

inline void log_module() {
  using namespace jinject;

  using MyDatabase = jdb::SqliteDatabase<LevelLogModel, TipoLogModel, LogModel>;

  SINGLE(std::shared_ptr<jdb::Database>, LogModel) {
    auto db = std::make_shared<MyDatabase>("log.db");

    db->add_migration(jdb::Migration{
          1, [](jdb::Database &db) {
            insert<LevelLogModel, "descricao">(db)
                .values("Trace")
                .values("Debug")
                .values("Info")
                .values("Warn")
                .values("Error")
                .values("Fatal");

            insert<TipoLogModel, "descricao">(db)
              .values("Sistema")
              .values("Modelo");
          }
        })
        .build();

    return db;
  };

  UNIQUE(LogRepository) {
    return new LogRepository{inject<std::shared_ptr<jdb::Database>, LogModel>()};
  };
}
