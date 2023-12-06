#pragma once

#include "model/categoria_produto/CategoriaProdutoModel.hpp"

#include <optional>
#include <vector>
#include <algorithm>

struct ICategoriaProdutoRepository {
  virtual std::vector<CategoriaProdutoModel> load_all() const = 0;

  virtual std::optional<CategoriaProdutoModel> load_by_id(int id) const {
    auto items = load_all();
    auto it = std::find_if(items.begin(), items.end(),
                           [&](auto const &item) { return item["id"].get_int() == id; });

    if (it != items.end()) {
      return *it;
    }

    return {};
  }
};
