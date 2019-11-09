/**
 * Micro unit test headers-only library for cpp, need c++11 supported
 * Copyright, AlexLoser, Sebastiao Salvador, all rights reserved.
 * See https://github.com/alexloser/mut for more details.
 * Example:
 *
 * MICRO_UNIT(TestAdd)
 * {
 *   int a = 2, b = 3;
 *   ASSERT_EQ(a + b, 5);
 * }
 *
 * MICRO_UNIT_TEST_MAIN
 * {
 *   bool pass = mut::unit_tester::run(true);
 *   return pass ? EXIT_SUCCESS : EXIT_FAILURE;
 * }
 */
#pragma once
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <iomanip>

// Linux terminator color code
namespace mut {
static const char* MUT_COLOR_GRAY0  = "";
static const char* MUT_COLOR_GRAY   = "";
static const char* MUT_COLOR_GREEN  = "";
static const char* MUT_COLOR_RED    = "";
static const char* MUT_COLOR_YELLOW = "";
static const char* MUT_COLOR_PASS   = "";
static const char* MUT_COLOR_FAIL   = "";
}

#ifndef MUT_STREAM
#define MUT_STREAM std::cerr << mut::MUT_COLOR_GRAY << std::setw(6) << clock() << " "
#endif

// Macro for writing to the terminal an INFO-level log
#ifndef MUT_INFO
#define MUT_INFO        MUT_STREAM << mut::MUT_COLOR_YELLOW << "[INFO]  " << mut::MUT_COLOR_GRAY << " "
#define MUT_LOG_INFO    MUT_INFO << __FILE__ << ":" << __LINE__ << ": "
#endif

// Macro for writing to the terminal a BAD-level log
#ifndef MUT_BAD
#define MUT_BAD         MUT_STREAM << mut::MUT_COLOR_RED << "[BAD!]  " << mut::MUT_COLOR_GRAY << " "
#define MUT_LOG_BAD     MUT_BAD << __FILE__ << ":" << __LINE__ << ": "
#endif

// Macro for writing to the terminal a GOOD-level log
#ifndef MUT_GOOD
#define MUT_GOOD        MUT_STREAM << mut::MUT_COLOR_GREEN << "[GOOD]  " << mut::MUT_COLOR_GRAY << " "
#define MUT_LOG_GOOD    MUT_GOOD << __FILE__ << ":" << __LINE__ << ": "
#endif

// Macro for writing to the terminal an DEBUG info from tested program
#ifndef MUT_DEBUG
#define MUT_DEBUG(var)  MUT_STREAM << mut::MUT_COLOR_GRAY0 << "[DEBUG]  " #var " " << var << std::endl;
#endif

#ifndef MUT_LOG_SEP
#define MUT_LOG_SEP \
std::cerr << "-----------------------------------------------------------------------------------------" << std::endl
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
public:
    ~unit_tester() {};

    static void set_clr(bool color_mode)
    {
        if (color_mode) {
            MUT_COLOR_GRAY0 = "\033[00;37m";
            MUT_COLOR_GRAY  = "\033[01;37m";
            MUT_COLOR_GREEN = "\033[01;32m";
            MUT_COLOR_RED   = "\033[01;31m";
            MUT_COLOR_YELLOW= "\033[01;33m";
            MUT_COLOR_PASS  = "\033[01;36m";
            MUT_COLOR_FAIL  = "\033[01;35m";
        }
    }

    static bool run(int argc, char* argv[])
    {
        bool color_mode = false;
        for (int i = 1; i < argc; ++i) {
            if (strstr(argv[i], "color_mode")) {
                color_mode = true;
            }
        }
        return unit_tester::run(color_mode);
    }

    // Run all the registered unit test cases, returns True if all tests pass, false otherwise.
    static bool run(bool color_mode = false)
    {
        set_clr(color_mode);
        std::cerr << MUT_COLOR_YELLOW
                  << "\t\t\tWill Run " << instance().unit_functions.size() << " Test Cases"
                  << MUT_COLOR_GRAY << std::endl;

        std::vector<std::string> failures, sucesses;
        // Starts unit tests
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
            MUT_GOOD << MUT_COLOR_YELLOW << "All tests passed!" << MUT_COLOR_GRAY << std::endl;
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
    // Returns True if all tests pass, false otherwise.
    struct registrator
    {
        registrator(const std::string &name, unit_fun_t function)
        {
            unit_tester::register_function(name, function);
        };
    };

protected:
    unit_tester() { }
    static unit_tester& instance()
    {
        static unit_tester instance;
        return instance;
    }

protected:
    std::map<std::string, unit_fun_t> unit_functions;
    unit_tester(const unit_tester&) = delete;
    unit_tester(unit_tester&&) = delete;
    unit_tester operator=(const unit_tester&) = delete;
};

} // end namespace mut

#ifndef MUT_USER_MACROS
#define MUT_USER_MACROS

#ifndef MUT_CAT_NEXP
#define MUT_CAT_NEXP(A, B) A ## B
#endif

#ifndef MUT_MACRO_CAT
#define MUT_MACRO_CAT(A, B) MUT_CAT_NEXP(A, B)
#endif

// Register a unit function using a helper static registrator object.
#ifndef MUT_REGISTER
#define MUT_REGISTER(function)                                           \
      static mut::unit_tester::registrator                               \
      MUT_MACRO_CAT(UNIT_REGISTRATION, __COUNTER__)(#function, function);
#endif

// Start a new unit test case block
#ifndef MICRO_UNIT
#define MICRO_UNIT(function)                                             \
    void function(mut::unit_function_result*);                           \
    MUT_REGISTER(function);                                              \
    void function(mut::unit_function_result* __mut_result)
#endif

// Pass the test and return from the test case.
#ifndef MUT_PASS
#define MUT_PASS() \
    {                                                               \
        MUT_LOG_GOOD << "Test stopped: Pass" << std::endl;          \
        __mut_result->success = true;                               \
        return;                                                     \
    }
#endif

// Fail the test and return from the test case.
#ifndef MUT_FAIL
#define MUT_FAIL() \
    {                                                               \
        MUT_LOG_BAD << "Test stopped: Fail" << std::endl;           \
        __mut_result->success = false;                              \
        return;                                                     \
    }
#endif

// Check a particular test condition. If the condition does not hold, fail the test and return.
#ifndef ASSERT_TRUE
#define ASSERT_TRUE(condition) \
    if (!(condition)) {                                                 \
        MUT_LOG_BAD << "Assert-True failed: " #condition << std::endl;  \
        MUT_FAIL();                                                     \
    }
#endif

// Check a particular test condition. If the condition holds, fail the test and return.
#ifndef ASSERT_FALSE
#define ASSERT_FALSE(condition) \
    if((condition)) {                                                   \
        MUT_LOG_BAD << "Assert-False failed: " #condition << std::endl; \
        MUT_FAIL();                                                     \
    }
#endif

// Check a particular test condition, equal
#ifndef ASSERT_EQ
#define ASSERT_EQ(X, Y) \
    if (!(X==Y)) {                                                          \
        MUT_LOG_BAD << "Assert-Equal failed: " #X " == " #Y << std::endl;   \
        MUT_FAIL();                                                         \
    }
#endif

// Check a particular test condition, not equal
#ifndef ASSERT_NE
#define ASSERT_NE(X, Y) \
    if (!(X!=Y)) {                                                           \
        MUT_LOG_BAD << "Assert-NotEqual failed: " #X " != " #Y << std::endl; \
        MUT_FAIL();                                                          \
    }
#endif

// Check a particular test condition, x less than y
#ifndef ASSERT_LT
#define ASSERT_LT(X, Y) \
    if (!(X<Y)) {                                                            \
        MUT_LOG_BAD << "Assert-LessThan failed: " #X " < " #Y << std::endl;  \
        MUT_FAIL();                                                          \
    }
#endif

// Check a particular test condition, x greater than y
#ifndef ASSERT_GT
#define ASSERT_GT(X, Y) \
    if (!(X>Y)) {                                                            \
        MUT_LOG_BAD << "Assert-GreatThan failed: " #X " > " #Y << std::endl; \
        MUT_FAIL();                                                          \
    }
#endif


#ifndef MICRO_UNIT_TEST_MAIN
#define MICRO_UNIT_TEST_MAIN int main(int argc, char* argv[], char* env[])
#endif



#endif  // end MUT_USER_MACROS



