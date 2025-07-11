#pragma once

#include <algorithm>
#include <optional>
#include <string>

#include "jmixin/jstring.h"

struct Token {
  static std::optional<Token> from(std::optional<std::string> value) {
    return value
      .and_then([](std::string const &value) -> std::optional<Token> {
        Token token{value};

        if (!token.is_valid()) {
          return {};
        }

        return token;
      });
  }

  std::string const & command() const {
    return mCommand;
  }

  std::string const & token() const {
    return mToken;
  }

private:
  std::string mCommand, mToken;

  explicit Token(std::string value) {
    parse_token(std::move(value));
  }

  void parse_token(std::string value) {
    if (value.empty()) {
      return;
    }

    auto split = jmixin::String{value}.split();

    if (split.size() != 2) { // <Command> <Token>
      return;
    }

    mCommand = split[0].trim();
    mToken = split[1].trim();
  }

  [[nodiscard]] bool is_valid() const {
    return !mCommand.empty() and !mToken.empty();
  }
};
