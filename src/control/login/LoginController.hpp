#pragma once

#include "control/login/LoginInteractor.hpp"
#include "ui/Form.hpp"
#include "utils/Log.hpp"

#include <cstdlib>
#include <functional>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include <fmt/format.h>

#include "jinject/jinject.h"

using namespace jui;

struct LoginController {
  inline static const std::string Tag = "LoginController";

  explicit LoginController(std::unique_ptr<LoginInteractor> loginInteractor)
    : mLoginInteractor{std::move(loginInteractor)} {
  }

  bool execute(std::function<void(UsuarioModel, std::vector<CargoModel>)> callback) {
    Form<Item<"nome", "Nome do usuario", TypeItem::Text>,
        Item<"senha", "Senha do usuario", TypeItem::Text> >{}
      .before([&]() {
        fmt::print("\x1b[H\x1b[2JLogin\n\n");
      })
      .on_success([&](Input input) {
        auto nome = input.get_text("nome").value_or("");
        auto senha = input.get_text("senha").value_or("");

        mLoginInteractor->login(nome, senha)
          .and_then([&](UsuarioModel const &value) {
            logt(TipoLog::Sistema, Tag, "logando: {}", value); // TODO:: adicionar o tempo

            callback(value, mLoginInteractor->load_cargos(value));

            return std::optional{true};
          });
      })
      .on_failed(abort)
      .interruptable()
      .show();

    return true;
  }

private:
  std::unique_ptr<LoginInteractor> mLoginInteractor;
};
