#pragma once

#include <functional>

class Database;

struct Migration {
  Migration(int id, std::function<void(Database &)> callback)
    : mCallback{callback}, mId{id}
  {
  }

  virtual ~Migration() = default;

  int get_id() const {
    return mId;
  }

  void execute(Database &db) const {
    mCallback(db);
  }

private:
  std::function<void(Database &)> mCallback;
  int mId;
};
