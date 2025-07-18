#pragma once

#include "control/admin/AdminInteractor.hpp"
#include "control/login/LoginInteractor.hpp"
#include "ui/Form.hpp"
#include "ui/Table.hpp"
#include "utils/Format.hpp"
#include "utils/Log.hpp"

#include <cstdlib>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <fmt/format.h>

#include "control/ErrorMsg.hpp"
#include "jinject/jinject.h"
#include "model/base/Id.hpp"

enum class SelecaoAdmin {
  BuscarUsuario = 1,
  AdicionarUsuario,
  RemoverUsuario,
  AtualizarSenha,
  AtualizarCargos,
  Sair
};

using namespace jui;

struct AdminController {
  inline static const std::string Tag = "AdminController";

  AdminController(std::unique_ptr<AdminInteractor> adminInteractor,
                  std::unique_ptr<LoginInteractor> loginInteractor)
    : mAdminInteractor{std::move(adminInteractor)},
      mLoginInteractor{std::move(loginInteractor)} {
  }

  bool execute() {
    using namespace jui;

    bool result = true;

    Form<Item<"opcao", "Selecione uma opcao do menu", TypeItem::Int> >{}
      .title("Administracao")
      .before([&]() {
        fmt::print("Escolha uma opção:\n");
        fmt::print("\t{} - Buscar usuario\n",
                   static_cast<int>(SelecaoAdmin::BuscarUsuario));
        fmt::print("\t{} - Adicionar usuario\n",
                   static_cast<int>(SelecaoAdmin::AdicionarUsuario));
        fmt::print("\t{} - Remover usuario\n",
                   static_cast<int>(SelecaoAdmin::RemoverUsuario));
        fmt::print("\t{} - Atualizar senha\n",
                   static_cast<int>(SelecaoAdmin::AtualizarSenha));
        fmt::print("\t{} - Atualizar cargos\n",
                   static_cast<int>(SelecaoAdmin::AtualizarCargos));
      })
      .on_success([&](Input input) {
        try {
          input.get_int("opcao").and_then([&](auto value) {
            auto opcao = static_cast<SelecaoAdmin>(value);

            if (opcao == SelecaoAdmin::BuscarUsuario) {
              listar_usuario();
            } else if (opcao == SelecaoAdmin::AdicionarUsuario) {
              adicionar_usuario();
            } else if (opcao == SelecaoAdmin::RemoverUsuario) {
              remover_usuario();
            } else if (opcao == SelecaoAdmin::AtualizarSenha) {
              atualizar_senha();
            } else if (opcao == SelecaoAdmin::AtualizarCargos) {
              atualizar_cargos();
            } else if (opcao == SelecaoAdmin::Sair) {
              result = false;
            }

            return std::optional<bool>{true};
          });
        } catch (std::runtime_error &e) {
          loge(TipoLog::Sistema, Tag, e.what());
        }

        fmt::print("Pressione qualquer tecla para continuar ...");

        std::cin.ignore();
      })
      .on_failed(opcao_invalida)
      .show();

    return result;
  }

  void listar_usuario() {
    Form<Item<"nome", "Nome do usuario", TypeItem::Text> >{}
      .on_success([&](Input input) {
        Table{
            mAdminInteractor->load_usuario_by<"nome">(
              input.get_text("nome").value())
          }
          .title("Usuarios encontrados")
          .head([]() {
            return std::vector<Col>{Col{"id", 15}, Col{"nome", 80}};
          })
          .data([&](auto const &item) {
            return Row{item["id"], item["nome"]};
          })
          .show();
      })
      .on_failed(opcao_invalida)
      .show();
  }

  void adicionar_usuario() const {
    Form<Item<"nome", "Nome do usuario", TypeItem::Text>,
        Item<"cpf", "CPF", TypeItem::Text>,
        Item<"telefone", "Telefone", TypeItem::Text>,
        Item<"email", "e-mail", TypeItem::Text>,
        Item<"apelido", "Apelido (usado no login)", TypeItem::Text> >{}
      .on_success([&](Input input) {
        UsuarioModel item;

        item["nome"] = input.get_text("nome");
        item["cpf"] = input.get_text("cpf");
        item["telefone"] = input.get_text("telefone");
        item["email"] = input.get_text("email");
        item["apelido"] = input.get_text("apelido");

        logopt(TipoLog::Sistema, Tag, mAdminInteractor->save_usuario(item));
      })
      .on_failed(opcao_invalida)
      .show();
  }

  void remover_usuario() const {
    Form<Item<"id", "Id do usuario", TypeItem::Int> >{}
      .on_success([&](Input input) {
        auto usuarioId = Id::from(input.get_int("id"));

        if (!usuarioId) {
          logt(TipoLog::Sistema, Tag, ErrorMsg::ID_USUARIO_INVALIDO);

          return;
        }

        logopt(TipoLog::Sistema, Tag, mAdminInteractor->remove_usuario(*usuarioId));
      })
      .on_failed(opcao_invalida)
      .show();
  }

  void atualizar_senha() const {
    Form<Item<"id", "Id do usuario", TypeItem::Int>,
        Item<"senhaAntiga", "Senha atual", TypeItem::Text>,
        Item<"senhaNova", "Senha nova", TypeItem::Text> >{}
      .on_success([&](Input input) {
        auto usuarioId = Id::from(input.get_int("id"));
        auto senhaAtual = input.get_text("senhaAtual").value_or("");
        auto senhaNova = input.get_text("senhaNovaa").value_or("");

        if (!usuarioId) {
          logt(TipoLog::Sistema, Tag, ErrorMsg::ID_USUARIO_INVALIDO);

          return;
        }

        logopt(TipoLog::Sistema, Tag, mAdminInteractor->update_password_usuario(*usuarioId,
          senhaAtual, senhaNova));
      })
      .on_failed(opcao_invalida)
      .show();
  }

  void atualizar_cargos() {
    Table{mAdminInteractor->load_all_cargos()}
      .title("Listagem de cargos")
      .head([]() {
        return std::vector<Col>{Col{"id", 15}, Col{"descricao", 32}};
      })
      .data([&](auto const &item) {
        return Row{item["id"], item["descricao"]};
      })
      .show();

    Form<Item<"id", "Id do usuario", TypeItem::Int>,
        Item<"cargos", "Lista de cargos separados por virgula [1,2,3,...]",
          TypeItem::Text> >{}
      .on_success([&](Input input) {
        auto usuarioId = Id::from(input.get_int("id"));
        auto cargoIds = input.get_text("cargos")
          .and_then([&](std::string const &value) -> std::optional<std::set<int> > {
            auto items = jmixin::String{value}.split(",");
            std::set<int> cargoSet;

            for (auto const &cargo: items) {
              cargoSet.insert(std::stoi(cargo));
            }

            if (cargoSet.empty()) {
              return {};
            }

            return cargoSet;
          });

        if (!usuarioId) {
          logt(TipoLog::Sistema, Tag, ErrorMsg::ID_USUARIO_INVALIDO);

          return;
        }

        if (!cargoIds) {
          logt(TipoLog::Sistema, Tag, ErrorMsg::LISTA_CARGOS_VAZIA);

          return;
        }

        logopt(TipoLog::Sistema, Tag, mAdminInteractor->update_cargos(*usuarioId, *cargoIds));
      })
      .on_failed(opcao_invalida)
      .show();
  }

private:
  std::unique_ptr<AdminInteractor> mAdminInteractor;
  std::unique_ptr<LoginInteractor> mLoginInteractor;
};
