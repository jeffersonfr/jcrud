#pragma once

#include "control/login/LoginController.hpp"

void login_module() {
  using namespace jinject;

  UNIQUE(LoginRepository) {
    return new LoginRepository{inject<std::shared_ptr<Database>>()};
  };

  UNIQUE(LoginInteractorRepository) {
    return new LoginInteractorRepository{};
  };

  UNIQUE(LoginInteractor) {
    return new LoginInteractor{inject<std::unique_ptr<LoginInteractorRepository>>()};
  };

  UNIQUE(LoginController) {
    return new LoginController{inject<std::unique_ptr<LoginInteractor>>()};
  };
}
