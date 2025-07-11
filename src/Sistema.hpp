#pragma once

#include "Ambiente.hpp"
#include "control/admin/AdminController.hpp"
#include "control/produto/ProdutoController.hpp"
#include "ui/Form.hpp"
#include "utils/Log.hpp"
#include "api/Routes.hpp"
#include "api/routes/v1/BasicRoutes.hpp"

#include <fmt/format.h>

#include "ftxui/component/captured_mouse.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/component_options.hpp"
#include "ftxui/component/screen_interactive.hpp"

enum class SelecaoSistema { Administracao = 1, Produtos, Estoque, Sair };

struct Sistema {
  inline static std::string const Tag = "Sistema";

  Sistema(std::unique_ptr<LoginController> loginController,
          std::unique_ptr<AdminController> adminController,
          std::unique_ptr<ProdutoController> produtoController,
          std::unique_ptr<EstoqueController> estoqueController)
      : mLoginController{std::move(loginController)},
        mAdminController{std::move(adminController)},
        mProdutoController{std::move(produtoController)},
        mEstoqueController{std::move(estoqueController)} {
    Log::instance().level(LevelLog::Trace);
  }

  void execute() {
    using namespace jui;

    auto routes = std::make_unique<Routes<
      v1::BasicRoutes
    >>(v1::BasicRoutes{});

    routes->start();

    do {
      if (!Ambiente::usuario.has_value()) {
        do_login();
      } else {
        do_menu();
      }
    } while (true);
  }

private:
  std::unique_ptr<LoginController> mLoginController;
  std::unique_ptr<AdminController> mAdminController;
  std::unique_ptr<ProdutoController> mProdutoController;
  std::unique_ptr<EstoqueController> mEstoqueController;
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
          if (cargo["id"].get_int() == Cargo::Administrador) {
            selecaoSet.insert(static_cast<int>(SelecaoSistema::Administracao));
            selecaoSet.insert(static_cast<int>(SelecaoSistema::Produtos));
            selecaoSet.insert(static_cast<int>(SelecaoSistema::Estoque));
          } else if (cargo["id"].get_int() == Cargo::Operador) {
            selecaoSet.insert(static_cast<int>(SelecaoSistema::Produtos));
            selecaoSet.insert(static_cast<int>(SelecaoSistema::Estoque));
          }
        }

        selecaoSet.insert(static_cast<int>(SelecaoSistema::Sair));

        logd(TipoLog::Sistema, Tag, "logado: {}", usuarioModel);
      }
    });
  }

  void do_menu() {
    /*
    auto screen = ftxui::ScreenInteractive::TerminalOutput();

    std::vector<std::string> entries = {
        "Administracao",
        "Produtos",
        "Estoque",
        "Sair",
    };
    int selected = 0;

    ftxui::MenuOption option;

    option.on_enter = screen.ExitLoopClosure();
    
    auto menu = ftxui::Menu(&entries, &selected, option);

    screen.Loop(menu);
    */

    Form<Item<"opcao", "Selecione uma opcao do menu", TypeItem::Int>>{}
        .title("Farmacia Pague+ - " + Ambiente::version)
        .before([&]() {
          fmt::print("{}", "Escolha uma opção:\n");

          if (selecaoSet.count(
                  static_cast<int>(SelecaoSistema::Administracao))) {
            fmt::print("\t{} - Administracao\n",
                       static_cast<int>(SelecaoSistema::Administracao));
          }

          if (selecaoSet.count(static_cast<int>(SelecaoSistema::Produtos))) {
            fmt::print("\t{} - Produtos\n",
                       static_cast<int>(SelecaoSistema::Produtos));
          }

          if (selecaoSet.count(static_cast<int>(SelecaoSistema::Estoque))) {
            fmt::print("\t{} - Estoque\n",
                       static_cast<int>(SelecaoSistema::Estoque));
          }

          if (selecaoSet.count(static_cast<int>(SelecaoSistema::Sair))) {
            fmt::print("\t{} - Sair\n",
                       static_cast<int>(SelecaoSistema::Sair));
          }
})
        .on_success([&](Input input) {
          auto opcao = input.get_int("opcao");

          if (!opcao.has_value() or selecaoSet.count(*opcao) == 0) {
            return;
          }

          if (opcao == SelecaoSistema::Administracao) {
            mAdminController->execute();
          } else if (opcao == SelecaoSistema::Produtos) {
            mProdutoController->execute();
          } else if (opcao == SelecaoSistema::Estoque) {
            mEstoqueController->execute();
          } else if (opcao == SelecaoSistema::Sair) {
            std::exit(0);
          }
        })
        .on_failed(opcao_invalida)
        .interruptable()
        .show();
  }
};
