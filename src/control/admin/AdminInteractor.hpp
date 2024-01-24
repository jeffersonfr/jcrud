#pragma once

#include "model/cargo/CargoRepository.hpp"
#include "model/usuario/UsuarioRepository.hpp"
#include "model/cargo_usuario/CargoUsuarioRepository.hpp"

#include <expected>
#include <optional>
#include <ranges>
#include <string>
#include <utility>
#include <vector>

#include "jinject/jinject.h"

struct AdminInteractor
{
  AdminInteractor(std::unique_ptr<UsuarioRepository> usuarioRepository,
                  std::unique_ptr<CargoRepository> cargoRepository,
                  std::unique_ptr<CargoUsuarioRepository> cargoUsuarioRepository)
      : mUsuarioRepository{std::move(usuarioRepository)},
        mCargoRepository{std::move(cargoRepository)},
        mCargoUsuarioRepository{std::move(cargoUsuarioRepository)} {}

  template <StringLiteral... Fields>
  std::vector<UsuarioModel> load_usuario_by(auto... values) const
  {
    return mUsuarioRepository->load_by<Fields...>(values...);
  }

  void save_usuario(UsuarioModel const &item)
  {
    if (item["id"].is_null()) {
      auto e = mUsuarioRepository->save(item);

      if (!e.has_value()) {
        throw e.error();
      }
    } else {
      mUsuarioRepository->update(item);
    }
  }

  void remove_usuario(UsuarioModel const &item)
  {
    UsuarioModel other{std::move(item)};

    other["excluido"] = true;

    save_usuario(other);
  }

  void update_cargos(UsuarioModel const &usuario,
                     std::vector<CargoModel> const &cargos)
  {
    auto usuarioId = usuario["id"].get_int().value_or(-1);

    mCargoUsuarioRepository->get_database()->transaction([&](Database &db) {
      std::ranges::for_each(
          mCargoUsuarioRepository->load_by<"usuario_id">(usuarioId),
          [&](auto const &item) { mCargoUsuarioRepository->remove(item); });

      std::ranges::for_each(
          cargos,
          [&](auto const &item) {
            insert<CargoUsuarioModel, "cargo_id", "usuario_id">(db)
              .values(item["id"].get_int().value(), usuarioId)
              .without_transaction();
          });
    });
  }

  std::vector<CargoModel> load_all_cargos() const
  {
    return mCargoRepository->load_all();
  }

private:
  std::unique_ptr<UsuarioRepository> mUsuarioRepository;
  std::unique_ptr<CargoRepository> mCargoRepository;
  std::unique_ptr<CargoUsuarioRepository> mCargoUsuarioRepository;
};
