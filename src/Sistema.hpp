#pragma once

#include "Ambiente.hpp"
#include "Modules.hpp"
#include "control/admin/AdminController.hpp"
#include "control/produto/ProdutoController.hpp"
#include "ui/Form.hpp"
#include "utils/Log.hpp"

#include <fmt/format.h>

enum class SelecaoSistema {
  Administracao = 1,
  Produtos = 2,
};

struct Sistema {
  inline static std::string const Tag = "Sistema";

  Sistema() {
    Log::instance().level(LevelLog::Trace);
  }

  void execute() {
    using namespace jui;

    logd(TipoLog::Sistema, Tag, "executando o sistema");

    do {
      if (!Ambiente::usuario.has_value()) {
        do_login();
      } else {
        do_menu();
      }
    } while (true);
  }

private:
  std::unique_ptr<LoginController> mLoginController = get{};
  std::unique_ptr<AdminController> mAdminController = get{};
  std::unique_ptr<ProdutoController> mProdutoController = get{};
  std::set<int> selecaoSet;

  void do_login() {

    Ambiente::usuario = {};
    Ambiente::cargos = {};

    mLoginController->execute([&](UsuarioModel usuarioModel,
                                  std::vector<CargoModel> cargoModel) {
      Ambiente::usuario = usuarioModel;
      Ambiente::cargos = cargoModel;

      if (Ambiente::cargos.has_value()) {
        for (auto const &cargo : Ambiente::cargos.value()) {
          if (cargo["id"].get_int().value() ==
              static_cast<int>(Cargo::Administrador)) {
            selecaoSet.insert(
                static_cast<int>(SelecaoSistema::Administracao));
            selecaoSet.insert(static_cast<int>(SelecaoSistema::Produtos));
          } else if (cargo["id"].get_int().value() ==
                      static_cast<int>(Cargo::Operador)) {
            selecaoSet.insert(static_cast<int>(SelecaoSistema::Produtos));
          }
        }
      }
    });
  }

  void do_menu() {
    Form<Item<"opcao", "Selecione uma opcao do menu", TypeItem::Int>>{}
      .before([&]() {
        system("clear");

        fmt::print("Bem-vindo ao sistema da Farmácia Pague++ !\n\n");
        fmt::print("Escolha uma opção:\n");

        if (selecaoSet.count(
                static_cast<int>(SelecaoSistema::Administracao))) {
          fmt::print("\t{} - Administracao\n",
                      static_cast<int>(SelecaoSistema::Administracao));
        }

        if (selecaoSet.count(
                static_cast<int>(SelecaoSistema::Produtos))) {
          fmt::print("\t{} - Produtos\n",
                      static_cast<int>(SelecaoSistema::Produtos));
        }
      })
      .on_success([&](Input input) {
        auto opcao = input.get_int("opcao");

        if (!opcao.has_value() or selecaoSet.count(*opcao) == 0) {
          return;
        }

        if (*opcao == static_cast<int>(SelecaoSistema::Administracao)) {
          mAdminController->execute();
        } else if (*opcao == static_cast<int>(SelecaoSistema::Produtos)) {
          mProdutoController->execute();
        }
      })
      .on_failed(opcao_invalida)
      .interruptable()
      .show();
  }
};
