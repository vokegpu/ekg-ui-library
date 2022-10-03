#include "ekg/ekg.hpp"

#ifndef TEST_FEATURE_H
#define TEST_FEATURE_H

namespace test {
    extern std::string name;

    enum class result {
        a, b, f
    };

    void run(const std::string &name) {
        ekg::log("running test..." + name);
        test::name = name;
    }

    void message(const std::string &test_message) {
        ekg::log("[" + test::name + "] " + test_message);
    }

    void check(const std::string& tag, const test::result &result) {
        switch (result) {
            case test::result::a: {
                test::message("A: " + tag + " - passed, no issues");
                break;
            }

            case test::result::b: {
                test::message("B: " + tag + " - passed, unfinished");
                break;
            }

            case test::result::f: {
                test::message("B: " + tag + " - not passed, with issues");
                break;
            }
        }
    }

    void end() {
        ekg::log("end of - " + test::name + " - test");
        test::name.clear();
    }
};

#endif