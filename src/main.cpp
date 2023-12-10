#include "Sistema.hpp"

int main() {
  try {
    Sistema sistema;

    sistema.execute();
  } catch (std::runtime_error &e) {
    fmt::print("{}\n", e.what());
  }
}
