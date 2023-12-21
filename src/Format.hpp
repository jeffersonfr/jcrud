#pragma once

#include "model/DataClass.hpp"

#include <fmt/format.h>

std::string format_date(Data const &value)
{
  if (value.is_null())
  {
    return "<null>";
  }

  std::ostringstream o;
  std::string date = value.get_text().value();

  o << date.substr(0, 2) << "/" << date.substr(2, 2) << "/" << date.substr(4);

  return o.str();
}

std::string format_currency(Data const &value)
{
  if (value.is_null())
  {
    return "<null>";
  }

  return fmt::format("{:>10.2F}", value.get_decimal().value());
}

std::string format_timestamp(std::chrono::seconds secs)
{
  std::time_t time = secs.count();
  char timeString[std::size("yyyy-mm-dd hh:mm:ss.000")];
  std::strftime(std::data(timeString), std::size(timeString),
                "%F %T.000", std::gmtime(&time));

  return timeString;
}
