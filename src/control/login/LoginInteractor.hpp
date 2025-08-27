#pragma once

#include "control/login/LoginInteractorModel.hpp"
#include "control/login//LoginInteractorRepository.hpp"
#include "model/filial/FilialRepository.hpp"

#include <optional>
#include <ranges>
#include <string>
#include <vector>

#include "jinject/jinject.h"

struct LoginInteractor : public jdb::Repository<LoginInteractorModel> {
  LoginInteractor(
    std::unique_ptr<LoginInteractorRepository> loginInteractorRepository)
    : mLoginInteractorRepository{std::move(loginInteractorRepository)} {
  }

  std::optional<UsuarioModel> login(std::string const &nome,
                                    std::string const &senha) {
    auto items =
      load_all() | std::ranges::views::filter([&](auto const &item) {
        char senhaOriginal[128];

        strncpy(senhaOriginal, senha.data(), senha.size());

        senhaOriginal[senha.size()] = '\0';

        char *senhaEncriptada = crypt(senhaOriginal, "aa");

        return item.template get<UsuarioModel>("apelido") == nome and
               item.template get<UsuarioModel>("senha") == senhaEncriptada;
      }) | std::ranges::views::take(1);

    for (auto const &item: items) {
      return {item.template get<UsuarioModel>()};
    }

    return {};
  }

  std::vector<CargoModel> load_cargos(UsuarioModel usuario) {
    auto items =
      load_all() | std::ranges::views::filter([&](auto const &item) {
        return item.template get<UsuarioModel>("id") == usuario["id"];
      });

    return {items.begin(), items.end()};
  }

private:
  std::unique_ptr<LoginInteractorRepository> mLoginInteractorRepository;
};
