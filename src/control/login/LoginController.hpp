#pragma once

#include "control/login/LoginInteractor.hpp"
#include "ui/Form.hpp"
#include "ui/Table.hpp"
#include "utils/Format.hpp"
#include "utils/Log.hpp"

#include <cstdlib>
#include <fstream>
#include <functional>
#include <iostream>
#include <optional>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

#include <fmt/format.h>

#include "jinject/jinject.h"

using namespace jui;

struct LoginController {
  inline static const std::string Tag = "LoginController";

  LoginController(std::unique_ptr<LoginInteractor> loginInteractor)
      : mLoginInteractor{std::move(loginInteractor)} {}

  bool execute(std::function<void(UsuarioModel, std::vector<CargoModel>)> callback) {
    Form<Item<"nome", "Nome do usuario", TypeItem::Text>,
         Item<"senha", "Senha do usuario", TypeItem::Text>>{}
        .before([&]() {
          system("clear");

          fmt::print("Login\n\n");
        })
        .on_success([&](Input input) {
          auto nome = input.get_text("nome").value_or("");
          auto senha = input.get_text("senha").value_or("");
          auto usuario = mLoginInteractor->login(nome, senha);

          logt(TipoLog::Sistema, Tag, "logando: {}", *usuario);

          if (usuario.has_value()) {
            auto cargos = mLoginInteractor->load_cargos(*usuario);

            callback(*usuario, cargos);
          }
        })
        .on_failed(abort)
        .interruptable()
        .show();

    return true;
  }

private:
  std::unique_ptr<LoginInteractor> mLoginInteractor;
};
