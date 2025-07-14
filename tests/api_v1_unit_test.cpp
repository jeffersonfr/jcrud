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

  static void set_session_token(std::string const &token) {
    mSessionToken = token;
  }

  static std::string get_session_token() {
    return mSessionToken;
  }

  static void set_refresh_token(std::string const &token) {
    mRefreshToken = token;
  }

  static std::string get_refresh_token() {
    return mRefreshToken;
  }

private:
  inline static std::string mSessionToken = {};
  inline static std::string mRefreshToken = {};
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

TEST_F(CrudSuite, NoAuthorizationHeaderLogin) {
  crow::request req;
  crow::response res;

  req.url = "/api/v1/login";

  get_app().handle_full(req, res);

  int code = res.code;
  std::string msg = res.get_header_value("WWW-Authenticate");

  // std::cout << ">>> code '" << code << "' => msg [" << msg << "]\n";

  ASSERT_EQ(code, crow::UNAUTHORIZED);
  ASSERT_EQ(msg, R"(Bearer realm="jcrud", error="invalid_token", error_description="no authorization header")");
}

TEST_F(CrudSuite, EmptyAuthorizationHeaderLogin) {
  crow::request req;
  crow::response res;

  req.url = "/api/v1/login";
  req.add_header("Authorization", "");

  get_app().handle_full(req, res);

  int code = res.code;
  std::string msg = res.get_header_value("WWW-Authenticate");

  // std::cout << ">>> code '" << code << "' => msg [" << msg << "]\n";

  ASSERT_EQ(code, crow::UNAUTHORIZED);
  ASSERT_EQ(msg, R"(Bearer realm="jcrud", error="invalid_token", error_description="no authorization header")");
}

TEST_F(CrudSuite, WrongAuthorizationFormatLogin) {
  crow::request req;
  crow::response res;

  req.url = "/api/v1/login";
  req.add_header("Authorization", "1234567890");

  get_app().handle_full(req, res);

  int code = res.code;
  std::string msg = res.get_header_value("WWW-Authenticate");

  // std::cout << ">>> code '" << code << "' => msg [" << msg << "]\n";

  ASSERT_EQ(code, crow::UNAUTHORIZED);
  ASSERT_EQ(msg, R"(Bearer realm="jcrud", error="invalid_token", error_description="no authorization header")");
}

TEST_F(CrudSuite, InexistentAuthorizationLogin) {
  crow::request req;
  crow::response res;

  req.url = "/api/v1/login";
  req.add_header("Authorization", "Bearer 1234567890");

  get_app().handle_full(req, res);

  int code = res.code;
  std::string msg = res.get_header_value("WWW-Authenticate");

  // std::cout << ">>> code '" << code << "' => msg [" << msg << "]\n";

  ASSERT_EQ(code, crow::UNAUTHORIZED);
  ASSERT_EQ(msg, R"(Bearer realm="jcrud", error="invalid_token", error_description="invalid credentials")");
}

TEST_F(CrudSuite, ExpiredAuthorizationLogin) {
  crow::request req;
  crow::response res;

  req.url = "/api/v1/login";
  req.add_header("Authorization", "Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpYXQiOjE3NTIzMjE3ODQsImV4cCI6MTc1MjMyNTM4NCwiaXNzIjoiamNydWQiLCJzdWIiOiJhdXRoIiwianRpIjoiNDhmOGY4MTE2NTMyNDZkZTlhYmY0OTQwMzEyYjVlZDIifQ.frDLMib6im_24kiOLX6nCEinzObzdCbURL_Sy5oXiT0");

  get_app().handle_full(req, res);

  int code = res.code;
  std::string msg = res.get_header_value("WWW-Authenticate");

  // std::cout << ">>> code '" << code << "' => msg [" << msg << "]\n";

  ASSERT_EQ(code, crow::UNAUTHORIZED);
  ASSERT_EQ(msg, R"(Bearer realm="jcrud", error="invalid_token", error_description="token expired")");
}

TEST_F(CrudSuite, ValidAuthorizationLogin) {
  std::unique_ptr<SessionCredentialRepository> sessionCredentialRepository = jinject::get{};

  auto session= sessionCredentialRepository->load_all() | std::ranges::views::take(1);
  auto sessionCredential = session.front();

  auto jti = sessionCredential["id"].get_text().value();
  auto key = sessionCredential["key"].get_text().value();

  auto token = jwt::create()
    .set_issuer("jcrud")
    .set_subject("auth0")
    .set_expires_in(std::chrono::seconds(30))
    .set_payload_claim("jti", picojson::value(jti))
    .set_type("JWS")
    .sign(jwt::algorithm::hs256{key});

  crow::request req;
  crow::response res;

  req.url = "/api/v1/login";
  req.add_header("Authorization", std::format("Bearer {}", token));

  get_app().handle_full(req, res);

  int code = res.code;
  crow::json::rvalue msg = crow::json::load(res.body);

  // std::cout << ">>> code '" << code << "' => msg [" << msg << "]\n";

  std::string sessionToken = static_cast<std::string>(msg["sessionToken"]);
  std::string refreshToken = static_cast<std::string>(msg["refreshToken"]);

  ASSERT_EQ(code, crow::ACCEPTED);
  ASSERT_EQ(sessionToken.empty(), false);
  ASSERT_EQ(refreshToken.empty(), false);

  Environment::set_session_token(sessionToken);
  Environment::set_refresh_token(refreshToken);
}

TEST_F(CrudSuite, InvalidRefreshAuthorization) {
  crow::request req;
  crow::response res;

  req.url = "/api/v1/refresh";
  req.add_header("Authorization", std::format("Bearer 1234567890"));

  get_app().handle_full(req, res);

  int code = res.code;
  std::string  msg = res.body;

  // std::cout << ">>> code '" << code << "' => msg [" << msg << "]\n";

  ASSERT_EQ(code, crow::UNAUTHORIZED);
}

TEST_F(CrudSuite, ValidRefreshAuthorization) {
  crow::request req;
  crow::response res;

  req.url = "/api/v1/refresh";
  req.method = crow::HTTPMethod::POST;
  req.add_header("Authorization", std::format("Bearer {}", Environment::get_session_token()));
  req.body = crow::json::wvalue{{"refreshToken", Environment::get_refresh_token()}}.dump();

  get_app().handle_full(req, res);

  int code = res.code;
  std::string msg = res.body;

  std::cout << ">>> code '" << code << "' => msg [" << msg << "]\n";

  ASSERT_EQ(code, crow::ACCEPTED);
}

TEST_F(CrudSuite, InvalidLogout) {
  crow::request req;
  crow::response res;

  req.url = "/api/v1/logout";
  req.add_header("Authorization", std::format("Bearer 1234567890"));

  get_app().handle_full(req, res);

  int code = res.code;
  std::string  msg = res.body;

  // std::cout << ">>> code '" << code << "' => msg [" << msg << "]\n";

  ASSERT_EQ(code, crow::UNAUTHORIZED);
}

TEST_F(CrudSuite, ValidLogout) {
  crow::request req;
  crow::response res;

  req.url = "/api/v1/logout";
  req.add_header("Authorization", std::format("Bearer {}", Environment::get_session_token()));

  get_app().handle_full(req, res);

  int code = res.code;
  std::string  msg = res.body;

  // std::cout << ">>> code '" << code << "' => msg [" << msg << "]\n";

  ASSERT_EQ(code, crow::OK);
}

int main(int argc, char *argv[]) {
  Modules::load_modules();

  ::testing::InitGoogleTest(&argc, argv);

  ::testing::AddGlobalTestEnvironment(new Environment{});

  return RUN_ALL_TESTS();
}
