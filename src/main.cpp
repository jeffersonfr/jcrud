#include "Sistema.hpp"

int main() {
  try {
    Modules::load_modules();

    Sistema sistema;

    sistema.execute();
  } catch (std::runtime_error &e) {
    logf(TipoLog::Sistema, Sistema::Tag, e.what());

    fmt::print("{}\n", e.what());
  }
}
