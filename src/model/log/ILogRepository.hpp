#pragma once

#include "model/log/LogModel.hpp"

#include <optional>
#include <vector>
#include <algorithm>

struct ILogRepository {
  virtual void insert(std::string const &description) = 0;
};
