#pragma once

#include <string>

struct DecorateTitleStrategy {
  virtual std::string createFancyTitle(std::string const &title) {
    return title;
  }
};
