#pragma once

#include "model/cargo/CargoRepository.hpp"
#include "model/usuario/UsuarioRepository.hpp"
#include "model/cargoUsuario/CargoUsuarioRepository.hpp"
#include "control/ErrorMsg.hpp"
#include "model/base/Id.hpp"

#include <optional>
#include <ranges>
#include <utility>
#include <vector>

struct AdminInteractor {
  AdminInteractor(std::unique_ptr<UsuarioRepository> usuarioRepository,
                  std::unique_ptr<CargoRepository> cargoRepository,
                  std::unique_ptr<CargoUsuarioRepository> cargoUsuarioRepository)
    : mUsuarioRepository{std::move(usuarioRepository)},
      mCargoRepository{std::move(cargoRepository)},
      mCargoUsuarioRepository{std::move(cargoUsuarioRepository)} {
  }

  template<jmixin::StringLiteral... Fields>
  [[nodiscard]] std::vector<UsuarioModel> load_usuario_by(auto... values) const {
    return mUsuarioRepository->load_by<Fields...>(values...);
  }

  [[nodiscard]] std::optional<UsuarioModel> load_usuario_by_id(Id id) const {
    auto usuarioList = load_usuario_by<"id">(id.value());

    if (usuarioList.empty()) {
      return {};
    }

    return usuarioList[0];
  }

  std::optional<std::string> save_usuario(UsuarioModel const &item) {
    if (item["id"].is_null()) {
      if (auto result = mUsuarioRepository->save(item); !result.has_value()) {
        return result.error().what();
      }

      return {};
    }

    return mUsuarioRepository->update(item);
  }

  std::optional<std::string> update_password_usuario(Id usuarioId, std::string const &senhaAtual,
                                                     std::string const &senhaNova) {
    return load_usuario_by_id(usuarioId)
      .and_then([&](UsuarioModel value) -> std::optional<std::string> {
        std::string senhaAtualEncriptada = crypt(senhaAtual.c_str(), "aa");
        std::string senhaNovaEncriptada = crypt(senhaNova.c_str(), "aa");

        if (senhaAtualEncriptada != value["senha"]) {
          return ErrorMsg::ATUALIZACAO_SENHA_INVALIDA;
        }

        value["senha"] = senhaNovaEncriptada;

        return save_usuario(value);
      })
      .value_or("");
  }

  std::optional<std::string> remove_usuario(Id usuarioId) {
    return load_usuario_by_id(usuarioId)
      .and_then([&](UsuarioModel value) {
        value["excluido"] = true;

        return save_usuario(value);
      })
      .value_or("");
  }

  std::optional<std::string> update_cargos(Id usuarioId, std::set<int> const &cargoIds) {
    auto items = load_all_cargos() |
                 std::ranges::views::filter([&](auto const &item) {
                   return cargoIds.contains(static_cast<int>(item["id"].get_int().value_or(-1L)));
                 });
    std::vector<CargoModel> cargos{items.begin(), items.end()};

    try {
      mCargoUsuarioRepository->get_database()->transaction([&](jdb::Database &db) {
        std::ranges::for_each(
          mCargoUsuarioRepository->load_by<"usuario_id">(usuarioId.value()),
          [&](auto const &item) { mCargoUsuarioRepository->remove(item); });

        std::ranges::for_each(
          cargos,
          [&](auto const &item) {
            insert<CargoUsuarioModel, "cargo_id", "usuario_id">(db)
              .values(item["id"].get_int().value(), usuarioId.value());
          });
      });
    } catch (std::runtime_error &e) {
      return e.what();
    }

    return {};
  }

  [[nodiscard]] std::vector<CargoModel> load_all_cargos() const {
    return mCargoRepository->load_all();
  }

private:
  std::unique_ptr<UsuarioRepository> mUsuarioRepository;
  std::unique_ptr<CargoRepository> mCargoRepository;
  std::unique_ptr<CargoUsuarioRepository> mCargoUsuarioRepository;
};
