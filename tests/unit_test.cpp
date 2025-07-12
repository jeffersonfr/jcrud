#include <iostream>

#include <gtest/gtest.h>

class Environment : public ::testing::Environment {
public:
    ~Environment() override {
    }

    void SetUp() override {
        LoadModules();
    }

    void TearDown() override {
    }

private:
    void LoadModules() {
    }
};

TEST(CrudSuite, EmptyAuthorizationLogin) {
    // ASSERT_EQ(value, "https://google.com");
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);

    ::testing::AddGlobalTestEnvironment(new Environment);

    return RUN_ALL_TESTS();
}
