#pragma once

#include "control/login/LoginInteractorModel.hpp"
#include "model/filial/FilialRepository.hpp"
#include "model/login/LoginRepository.hpp"
#include "model/usuario/UsuarioRepository.hpp"

#include <expected>
#include <optional>
#include <ranges>
#include <string>
#include <utility>
#include <vector>

#include "jinject/jinject.h"

struct LoginInteractor : public Repository<LoginInteractorModel> {
  LoginInteractor(std::unique_ptr<FilialRepository> filialRepository,
                  std::unique_ptr<UsuarioRepository> usuarioRepository,
                  std::unique_ptr<LoginRepository> loginRepository)
      : mFilialRepository{std::move(filialRepository)},
        mUsuarioRepository{std::move(usuarioRepository)},
        mLoginRepository{std::move(loginRepository)} {}

  std::optional<LoginInteractorModel> login(std::string const &nome,
                                            std::string const &senha) {
    auto items = load_all() | std::ranges::views::filter([&](auto const &item) {
                   char senhaOriginal[128];
                   
                   strncpy(senhaOriginal, senha.data(), senha.size());

                   senhaOriginal[senha.size()] = '\0';

                   char *senhaEncriptada = crypt(senhaOriginal, "aa");

                   return item.template get<UsuarioModel>("apelido") == nome and
                          item.template get<UsuarioModel>("senha") == senhaEncriptada;
                 });

    for (auto const &item : items) {
      return item;
    }

    return {};
  }

private:
  std::unique_ptr<FilialRepository> mFilialRepository;
  std::unique_ptr<UsuarioRepository> mUsuarioRepository;
  std::unique_ptr<LoginRepository> mLoginRepository;
};
