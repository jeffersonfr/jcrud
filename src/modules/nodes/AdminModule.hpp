#pragma once

#include "control/admin/AdminController.hpp"

void admin_module() {
  using namespace jinject;

  UNIQUE(AdminInteractor) {
    return new AdminInteractor{get{}, get{}, get{}};
  };

  UNIQUE(AdminController) { return new AdminController{get{}, get{}}; };
}
