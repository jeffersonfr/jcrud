#pragma once

#include "control/admin/AdminInteractor.hpp"
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

  AdminController(std::unique_ptr<AdminInteractor> adminInteractor, std::unique_ptr<LoginInteractor> loginInteractor)
      : mAdminInteractor{std::move(adminInteractor)}, mLoginInteractor{std::move(loginInteractor)} {}

  bool execute() {
    using namespace jui;

    bool result = true;

    Form<Item<"opcao", "Selecione uma opcao do menu", TypeItem::Int>>{}
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
          auto opcao = input.get_int("opcao");

          if (!opcao.has_value()) {
            return;
          }

          try {
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

              return;
            }
          } catch (std::runtime_error &e) {
            loge(TipoLog::Sistema, Tag, "{}", e.what());

            fmt::print("{}", e.what());
          }

          fmt::print("Pressione qualquer tecla para continuar ...");

          std::cin.ignore();
        })
        .on_failed(opcao_invalida)
        .show();

    return result;
  }

  void listar_usuario() {
    Form<Item<"nome", "Nome do usuario", TypeItem::Text>>{}
        .on_success([&](Input input) {
          Table{mAdminInteractor->load_usuario_by<"nome">(
                    input.get_text("nome").value())}
              .title("Usuarios encontrados")
              .head([]() {
                return std::vector<Col>{Col{"id", 15}, Col{"nome", 80}};
              })
              .data([&](auto const &item) {
                return Row{item["id"], item["nome"]};
              })
              .show();

          logt(TipoLog::Sistema, Tag, "listando usuario por nome:[{}]", input.get_text("nome").value());
        })
        .on_failed(opcao_invalida)
        .show();
  }

  void adicionar_usuario() {
    Form<Item<"nome", "Nome do usuario", TypeItem::Text>,
         Item<"cpf", "CPF", TypeItem::Text>,
         Item<"telefone", "Telefone", TypeItem::Text>,
         Item<"email", "e-mail", TypeItem::Text>,
         Item<"apelido", "Apelido (usado no login)", TypeItem::Text>>{}
        .on_success([&](Input input) {
          UsuarioModel item;

          item["nome"] = input.get_text("nome");
          item["cpf"] = input.get_text("cpf");
          item["telefone"] = input.get_text("telefone");
          item["email"] = input.get_text("email");
          item["apelido"] = input.get_text("apelido");

          mAdminInteractor->save_usuario(item);

          logt(TipoLog::Sistema, Tag, "adicionando usuario:[{}]", item);
        })
        .on_failed(opcao_invalida)
        .show();
  }

  void remover_usuario() {
    Form<Item<"id", "Id do usuario", TypeItem::Int>>{}
        .on_success([&](Input input) {
          auto usuarioId = input.get_int("id");

          auto items =
              mAdminInteractor->load_usuario_by<"id">(usuarioId.value());

          for (auto const &item : items) {
            mAdminInteractor->remove_usuario(item);

            logt(TipoLog::Sistema, Tag, "removendo usuario:[{}]", item);
  
            return;
          }
        })
        .on_failed(opcao_invalida)
        .show();
  }

  void atualizar_senha() {
    Form<Item<"id", "Id do usuario", TypeItem::Int>,
         Item<"senhaAntiga", "Senha atual", TypeItem::Text>,
         Item<"senhaNova", "Senha nova", TypeItem::Text>>{}
        .on_success([&](Input input) {
          auto usuarioId = input.get_int("id");
          auto senhaAtual = input.get_text("senhaAtual").value_or("");
          auto senhaNova = input.get_text("senhaNovaa").value_or("");

          auto items =
              mAdminInteractor->load_usuario_by<"id">(usuarioId.value());

          for (auto &usuario : items) {
            std::string senhaAtualEncriptada = crypt(senhaAtual.c_str(), "aa");
            std::string senhaNovaEncriptada = crypt(senhaNova.c_str(), "aa");

            if (senhaAtualEncriptada != usuario["senha"]) {
              fmt::print("Senhas diferentes\n");

              return;
            }

            usuario["senha"] = senhaNovaEncriptada;

            mAdminInteractor->save_usuario(usuario);

            logt(TipoLog::Sistema, Tag, "atualizando senha do usuario:[{}]", usuario);
          }
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
         Item<"cargos", "Lista de cargos separados por virgula [1,2,3,...]", TypeItem::Text>>{}
        .on_success([&](Input input) {
          auto usuarioId = input.get_int("id");
          auto usuarioList = mAdminInteractor->load_usuario_by<"id">(usuarioId.value());

          auto cargoList = jmixin::String{input.get_text("cargos").value_or("")}.split(",");
          std::set<int> cargoSet;

          for (auto const &cargo : cargoList) {
            cargoSet.insert(std::atoi(cargo.c_str()));
          }

          auto items = mAdminInteractor->load_all_cargos() 
            | std::ranges::views::filter([&](auto const &item) {
              return cargoSet.count(item["id"].get_int().value_or(-1)) != 0;
            });

          for (auto const &usuario : usuarioList) {
            mAdminInteractor->update_cargos(usuario, std::vector<CargoModel>{items.begin(), items.end()});

            logt(TipoLog::Sistema, Tag, "atualizando cargos do usuario:[{}]", usuario);

            return;
          }
        })
        .on_failed(opcao_invalida)
        .show();
  }

private:
  std::unique_ptr<AdminInteractor> mAdminInteractor;
  std::unique_ptr<LoginInteractor> mLoginInteractor;
};
