#pragma once

#include "model/DataClass.hpp"

#include <fmt/format.h>

std::string to_string(Data const &value)
{
  std::string result;

  value.get_value(
      overloaded{[&](nullptr_t arg)
                 { },
                 [&](bool arg)
                 { result = (arg?"true":"false"); },
                 [&](int64_t arg)
                 { result = std::to_string(arg); },
                 [&](double arg)
                 { result = std::to_string(arg); },
                 [&](std::string arg)
                 { result = arg; }});

  return result;
}
