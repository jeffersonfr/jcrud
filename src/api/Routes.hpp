#pragma once

#include "crow.h"

#include <thread>
#include <tuple>

template<typename... Args>
struct Routes {
  explicit Routes(Args... args)
    : mRoutesRefs{std::move(std::make_tuple<Args...>(std::move(args)...))} {
  }

  virtual ~Routes() {
    mApp.stop();
    mThread.join();
  }

  void start() {
    mThread = std::thread{
      [&] {
        blackhole(mApp);

        std::apply([&](auto &... args) {
          (args.init(mApp), ...);
        }, mRoutesRefs);

        mApp
            .port(3000)
            .multithreaded()
            .run();
      }
    };

    mApp.wait_for_server_start();
  }

private:
  crow::SimpleApp mApp;
  std::thread mThread;
  std::tuple<Args...> mRoutesRefs;

  void blackhole(crow::SimpleApp &app) {
    CROW_CATCHALL_ROUTE(app)
    ([this]() {
      return crow::response(crow::status::FORBIDDEN);
    });
  }
};
