#pragma once

#include "crow.h"

#include <thread>

struct Routes {
    virtual ~Routes() {
        mApp.stop();
        mThread.join();
    }

    void run() {
        mThread = std::thread{
            [&] {
                init_routes();

                mApp
                    .port(3000)
                    .multithreaded()
                    .run();
            }
        };

        mApp.wait_for_server_start();
    }

protected:
    crow::SimpleApp mApp;

    explicit Routes(std::uint32_t version)
        : mVersion{version} {
    }

    virtual void init_routes() = 0;

private:
    std::thread mThread;
    std::uint32_t mVersion;
};
