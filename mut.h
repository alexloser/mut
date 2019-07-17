/**
 * Micro unit test headers-only library for cpp, need c++11 supported
 * Copyright, AlexLoser, Sebastiao Salvador, all rights reserved.
 */
#pragma once
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <iostream>

// Linux terminator color code
#ifndef MUT_COLOR_CODE
#define MUT_COLOR_CODE
#define MUT_COLOR_GRAY   "\033[01;37m"
#define MUT_COLOR_GREEN  "\033[01;32m"
#define MUT_COLOR_RED    "\033[01;31m"
#define MUT_COLOR_YELLOW "\033[01;33m"
#define MUT_COLOR_PASS   "\033[01;36m"
#define MUT_COLOR_FAIL   "\033[01;35m"
#endif

// Macro for writing to the terminal an INFO-level log
#ifndef MUT_INFO
#define MUT_INFO        std::cerr << MUT_COLOR_YELLOW << "[INFO] " << MUT_COLOR_GRAY
#define MUT_LOG_INFO    MUT_INFO << __FILE__ << ":" << __LINE__ << ": "
#endif

// Macro for writing to the terminal a BAD-level log
#ifndef MUT_BAD
#define MUT_BAD         std::cerr << MUT_COLOR_RED << "[BAD!] " << MUT_COLOR_GRAY
#define MUT_LOG_BAD     MUT_BAD << __FILE__ << ":" << __LINE__ << ": "
#endif

// Macro for writing to the terminal a GOOD-level log
#ifndef MUT_GOOD
#define MUT_GOOD        std::cerr << MUT_COLOR_GREEN << "[GOOD] " << MUT_COLOR_GRAY
#define MUT_LOG_GOOD    MUT_GOOD << __FILE__ << ":" << __LINE__ << ": "
#endif

#ifndef MUT_LOG_SEP
#define MUT_LOG_SEP \
    std::cerr << "-------------------------------------------------------------------------------" << std::endl
#endif


namespace mut {

// Result of a unit test.
struct unit_function_result
{
    bool success { true };
};

// Unit test function type.
typedef void (*unit_fun_t)(unit_function_result*);

// Main class for unit test management. This class is a singleton.
class unit_tester
{
protected:
    unit_tester(){};

    static unit_tester& instance()
    {
        static unit_tester instance;
        return instance;
    }

public:
    ~unit_tester() {};

    // Run all the registered unit test cases, returns True if all tests pass, false otherwise.
    static bool run()
    {
        std::vector<std::string> failures, sucesses;
        MUT_INFO << "Will run " << instance().unit_functions.size() << " test cases" << std::endl;

        // Iterate all registered unit tests
        for (auto& unit : instance().unit_functions) {
            MUT_LOG_SEP;
            MUT_INFO << "Test case '" << unit.first << "'" << std::endl;

            // Run the unit test
            unit_function_result result;
            unit.second(&result);

            if (!result.success) {
                MUT_BAD << "Failed test" << std::endl;
                failures.push_back(unit.first);
            }
            else {
                MUT_GOOD << "Passed test" << std::endl;
                sucesses.push_back(unit.first);
            }
        }
        MUT_LOG_SEP;
        MUT_LOG_SEP;

        MUT_GOOD << MUT_COLOR_PASS << "Passed " << sucesses.size() << " test cases:" << MUT_COLOR_GRAY << std::endl;
        for (const auto& success_t : sucesses) {
            MUT_GOOD << success_t << std::endl;
        }
        MUT_LOG_SEP;

        // Output result summary
        if (failures.empty()) {
            MUT_GOOD << "All tests passed" << std::endl;
            MUT_LOG_SEP;
            return true;
        }
        else {
            MUT_BAD << MUT_COLOR_FAIL << "Failed " << failures.size() << " test cases:" << MUT_COLOR_GRAY << std::endl;
            for (const auto& failure : failures) {
                MUT_BAD << failure << std::endl;
            }
            MUT_LOG_SEP;
            return false;
        }
    }

    // Register a unit test case function.
    // returns True if all tests pass, false otherwise.
    static void register_function(const std::string &name, unit_fun_t function)
    {
        instance().unit_functions.emplace(name, function);
    }

    // Helper class to register a unit test in construction time. This is used to
    // call register_function in the construction of a static helper object.
    // Used by the MUT_REGISTER macro, which in turn is used by the MICRO_UNIT macro.
    // returns True if all tests pass, false otherwise.
    struct registrator
    {
        registrator(const std::string &name, unit_fun_t function)
        {
            unit_tester::register_function(name, function);
        };
    };

protected:
    std::map<std::string, unit_fun_t> unit_functions;
    unit_tester(const unit_tester&) = delete;
    unit_tester(unit_tester&&) = delete;
    unit_tester operator=(const unit_tester&) = delete;
};

} // end namespace mut

#ifndef MUT_USER_MACROS
#define MUT_CAT_NEXP(A, B) A ## B
#define MUT_MACRO_CAT(A, B) MUT_CAT_NEXP(A, B)

// Register a unit function using a helper static registrator object.
#define MUT_REGISTER(function)                                           \
      static mut::unit_tester::registrator                               \
      MUT_MACRO_CAT(UNIT_REGISTRATION, __COUNTER__)(#function, function);

#define MICRO_UNIT(function)                                             \
    void function(mut::unit_function_result*);                           \
    MUT_REGISTER(function);                                              \
    void function(mut::unit_function_result* __microunit_testresult)

// Pass the test and return from the test case.
#define PASS() \
    {                                                               \
        MUT_LOG_GOOD << "Test stopped: Pass" << std::endl;          \
        __microunit_testresult->success = true;                     \
        return;                                                     \
    }

// Fail the test and return from the test case.
#define FAIL() \
    {                                                               \
        MUT_LOG_BAD << "Test stopped: Fail" << std::endl;           \
        __microunit_testresult->success = false;                    \
        return;                                                     \
    }

// Check a particular test condition. If the condition does not hold, fail the test and return.
#define ASSERT_TRUE(condition) \
    if (!(condition)) {                                                 \
        MUT_LOG_BAD << "Assert-True failed: " #condition << std::endl;  \
        FAIL();                                                         \
    }

// Check a particular test condition. If the condition holds, fail the test and return.
#define ASSERT_FALSE(condition) \
    if((condition)) {                                                   \
        MUT_LOG_BAD << "Assert-False failed: " #condition << std::endl; \
        FAIL();                                                         \
    }

// Check a particular test condition, equal
#define ASSERT_EQ(X, Y) \
    if (!(X==Y)) {                                                          \
        MUT_LOG_BAD << "Assert-Equal failed: " #X " == " #Y << std::endl;   \
        FAIL();                                                             \
    }

// Check a particular test condition, not equal
// Check a particular test condition, equal
#define ASSERT_NE(X, Y) \
    if (!(X!=Y)) {                                                           \
        MUT_LOG_BAD << "Assert-NotEqual failed: " #X " != " #Y << std::endl; \
        FAIL();                                                              \
    }

#endif  // end MUT_USER_MACROS



