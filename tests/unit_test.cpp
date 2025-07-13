#include "modules/Modules.hpp"

#include <fmt/format.h>
#include <gtest/gtest.h>
#include <crow.h>

#include <iostream>

struct Environment : public ::testing::Environment {
  Environment() = default;

  void SetUp() override {
  }

  void TearDown() override {
  }
};

static auto my_create_routes() {
  return create_routes(v1::LoginRoutes{}, v1::BasicRoutes{});;
}

struct CrudSuite : public ::testing::Test {
  CrudSuite()
    : mRoutes{my_create_routes()} {}

  void SetUp() override {
    mRoutes->start();
  }

  crow::SimpleApp & get_app() {
    return mRoutes->get_app();
  }

private:
  std::unique_ptr<Routes<v1::LoginRoutes, v1::BasicRoutes>> mRoutes;

};

TEST_F(CrudSuite, EmptyAuthorizationLogin) {
  crow::request req;
  crow::response res;

  req.url = "/api/v1/login";

  get_app().handle_full(req, res);

  int code = res.code;
  std::string msg = res.get_header_value("WWW-Authenticate");

  std::cout << ">>> code '" << code << "' => msg [" << msg << "]\n";

  ASSERT_EQ(code, 401);
  ASSERT_EQ(msg, R"(Bearer realm="jcrud", error="invalid_token", error_description="Invalid token")");
}

int main(int argc, char *argv[]) {
  Modules::load_modules();

  ::testing::InitGoogleTest(&argc, argv);

  ::testing::AddGlobalTestEnvironment(new Environment{});

  return RUN_ALL_TESTS();
}
