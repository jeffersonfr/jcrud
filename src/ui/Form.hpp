#pragma once

#include "strategy/decorateTitle/DecorateTitleStrategy.hpp"
#include "ui/Input.hpp"

#include <algorithm>
#include <charconv>
#include <chrono>
#include <functional>
#include <iostream>
#include <map>
#include <string>

#include <signal.h>
#include <unistd.h>

#include <fmt/format.h>

#include "jinject/jinject.h"
#include "jmixin/jstring.h"

static bool cancelled = false;

namespace jui {
  template<StringLiteral Name, StringLiteral Description, TypeItem Type,
    bool Nullable = false>
  struct Item {
    static constexpr std::string get_name() { return Name.to_string(); }

    static constexpr std::string get_description() {
      return Description.to_string();
    }

    static constexpr TypeItem get_type() { return Type; }

    static constexpr bool is_nullable() { return Nullable; }
  };

  template<typename... Items>
  struct Form {
    Form() {
      std::array<std::string_view, sizeof...(Items)> items;
      int i = 0;

      (static_cast<void>(items[i++] = Items::get_name()), ...);

      std::sort(items.begin(), items.end());

      if (auto it = std::unique(items.begin(), items.end());
        items.size() != 0 and it != items.end()) {
        throw std::runtime_error("Duplicated itens in form");
      }
    }

    Form &title(std::string title) {
      mTitle = title;

      return *this;
    }

    Form &before(std::function<void()> callback) {
      mBefore = callback;

      return *this;
    }

    Form &on_success(std::function<void(Input)> callback) {
      mOnSuccess = callback;

      return *this;
    }

    Form &on_failed(std::function<void()> callback) {
      mOnFailed = callback;

      return *this;
    }

    Form &on_cancelled(std::function<void()> callback) {
      mOnCancelled = callback;

      return *this;
    }

    Form &after(std::function<void()> callback) {
      mAfter = callback;

      return *this;
    }

    template<typename T>
    Form &set(std::string const &key, T const &value) {
      using namespace std;

      mValues[key] = to_string(value);

      return *this;
    }

    Form &interruptable() {
      mInterruptable = true;
      return *this;
    }

    void show() {
      struct sigaction new_action, old_action;

      if (mTitle.has_value()) {
        std::unique_ptr<DecorateTitleStrategy> decorateTitleStrategy =
          jinject::get{};

        system("clear");

        fmt::print("{}\n\n", decorateTitleStrategy->createFancyTitle(*mTitle));
      }

      mBefore();

      if (!mInterruptable) {
        new_action.sa_handler = [](int) { cancelled = true; };
        sigemptyset(&new_action.sa_mask);
        new_action.sa_flags = 0;

        sigaction(SIGINT, &new_action, &old_action);
      }

      try {
        (execute(Items{}), ...);

        if (mInterruptable or !cancelled) {
          mOnSuccess(Input(mValues));
        }
      } catch (std::runtime_error &e) {
        fmt::print("[runtime error]: {}\n", e.what());

        mOnFailed();
      }

      if (!mInterruptable) {
        if (cancelled) {
          std::cin.clear();

          mOnCancelled();
        }

        cancelled = false;

        sigaction(SIGINT, &old_action, NULL);
      }

      mAfter();
    }

  private:
    std::map<std::string, std::optional<std::string> > mValues;

    std::function<void()> mBefore = []() {
    };
    std::function<void()> mAfter = []() {
    };
    std::function<void(Input)> mOnSuccess = [](Input) {
    };
    std::function<void()> mOnFailed = []() {
    };
    std::function<void()> mOnCancelled = []() {
    };

    std::optional<std::string> mTitle;
    bool mInterruptable = false;

    template<StringLiteral Name, StringLiteral Description, TypeItem Type,
      bool Nullable>
    void execute(Item<Name, Description, Type, Nullable> item) {
      if (!mInterruptable) {
        if (cancelled) {
          return;
        }
      }

      auto defaultValue = mValues.find(Name.to_string());
      std::string line, help;

      if (Type == TypeItem::Text) {
        help = "[ texto livre ]";
      } else if (Type == TypeItem::Int) {
        help = "[ valor inteiro ]";
      } else if (Type == TypeItem::Decimal) {
        help = "[ valor decimal ]";
      } else if (Type == TypeItem::Date) {
        help = "[ data no formato: ddmmaaaa ]";
      }

      fmt::print("{} <default: {}> {}\n: ", Description.to_string(),
                 defaultValue->second.value_or("--"), help);

      mValues[Name.to_string()] =
        read_value<Type>().or_else([&]() -> std::optional<std::string> {
          if (defaultValue != mValues.end()) {
            return defaultValue->second;
          } else if (Nullable) {
            return {};
          }

          throw std::runtime_error("item sem valor");
        });
    }

    template<TypeItem Type>
    std::optional<std::string> read_value() {
      std::string line;

      std::getline(std::cin, line);

      line = jmixin::String(line).trim();

      if (line.empty()) {
        return {};
      }

      if (Type == TypeItem::Text) {
        return line;
      } else if (Type == TypeItem::Int) {
        try {
          std::stoi(line);

          return line;
        } catch (std::invalid_argument &e) {
          // logt
        } catch (std::out_of_range &e) {
          // logt
        }

        return {};
      } else if (Type == TypeItem::Decimal) {
        try {
          std::stof(line);

          return line;
        } catch (std::invalid_argument &e) {
          // logt
        } catch (std::out_of_range &e) {
          // logt
        }

        return {};
      } else if (Type == TypeItem::Date) {
        try {
          auto d = std::chrono::year{std::stoi(line.substr(0, 2))};
          auto m = std::chrono::month{
            static_cast<unsigned>(std::stoi(line.substr(2, 2)))
          };
          auto y = std::chrono::day{
            static_cast<unsigned>(std::stoi(line.substr(4, 4)))
          };

          std::chrono::year_month_day ymd{d, m, y};

          if (ymd.ok()) {
            return line.substr(0, 8);
          }
        } catch (std::out_of_range &e) {
          // logt
        }
      }

      return {};
    }
  };
} // namespace jui

static void opcao_invalida() { fmt::print("Opcao Invalida !\n"); }
