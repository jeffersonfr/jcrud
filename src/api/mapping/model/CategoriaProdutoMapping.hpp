#pragma once

#include "model/categoriaProduto/CategoriaProdutoModel.hpp"

inline std::string to_string(CategoriaProdutoModel const &model) {
  return std::format("CategoriaProduto [id: {}, descricao: {}]",
                     *model["id"].get_int(), *model["descricao"].get_text());
}
