#pragma once

#include "control/login/LoginInteractor.hpp"
#include "ui/Form.hpp"
#include "ui/Table.hpp"
#include "utils/Format.hpp"

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
  LoginController(std::unique_ptr<LoginInteractor> loginInteractor)
      : mLoginInteractor{std::move(loginInteractor)} {}

  bool execute(std::function<void(std::optional<LoginInteractorModel>, std::vector<CargoModel>)> callback) {
    Form<Item<"nome", "Nome do usuario", TypeItem::Text>,
         Item<"senha", "Senha do usuario", TypeItem::Text>>{}
        .before([&]() {
          system("clear");

          fmt::print("Login\n\n");
        })
        .on_success([&](Input input) {
          auto nome = input.get_text("nome").value_or("");
          auto senha = input.get_text("senha").value_or("");
          
          // TODO:: carregar os cargos do local correto
          std::vector<CargoModel> cargos;
          CargoModel cargoModel;
          
          cargoModel["id"] = 1;
          cargoModel["descricao"] = "Administrador";
          cargos.push_back(cargoModel);
          cargoModel["id"] = 2;
          cargoModel["descricao"] = "Operador";
          cargos.push_back(cargoModel);

          callback(mLoginInteractor->login(nome, senha), cargos);
        })
        .on_failed(abort)
        .interruptable()
        .show();

    return true;
  }

private:
  std::unique_ptr<LoginInteractor> mLoginInteractor;
};
