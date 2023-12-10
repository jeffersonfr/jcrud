#include "Sistema.hpp"
#include "model/produto/ProdutoModel.hpp"
#include "model/categoria_produto/CategoriaProdutoModel.hpp"

template <typename ...Models>
struct InteractorModel : public Models... {
  template <typename T>
  void set(T const &model) {
    *this = model;
  }

  template <typename T>
  T & get() const {
    return (T &)(*this);
  }

  template <typename T>
  Data & get(std::string const &id) const {
    return ((T &)(*this))[id];
  }
};

int main() {
  InteractorModel<CategoriaProdutoModel, ProdutoModel> model;

  model.get<ProdutoModel>();
  model.get<ProdutoModel>()["id"];
  model.get<ProdutoModel>("id");

  // Sistema sistema;

  // sistema.execute();
}
