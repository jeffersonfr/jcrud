#include "Modules.hpp"

int main() {
  using namespace jinject;

  try {
    Modules::load_modules();

    std::unique_ptr<Sistema> sistema = get{};

    sistema->execute();
  } catch (std::runtime_error &e) {
    logf(TipoLog::Sistema, Sistema::Tag, e.what());

    fmt::print("{}\n", e.what());
  }
}
