#pragma once

#include <string>

struct DecorateTitleStrategy {
  virtual ~DecorateTitleStrategy() = default;

  virtual std::string createFancyTitle(std::string const &title) {
    return title;
  }
};
