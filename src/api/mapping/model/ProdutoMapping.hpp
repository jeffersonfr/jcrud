#pragma once

#include "model/produto/ProdutoModel.hpp"

inline std::string to_string(ProdutoModel const &model) {
  return std::format("Produto [id: {}, categoria_id: {}, nome: {}, descricao: {}]",
                     *model["id"].get_int(), *model["categoria_id"].get_int(), *model["nome"].get_text(),
                     *model["descricao"].get_text());
}
