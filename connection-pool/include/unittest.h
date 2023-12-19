#ifndef unit_test_h
#define unit_test_h

#ifndef UNITTEST_NOINCLUDE
#	ifdef __cplusplus
#		include <cstdio>
#		include <chrono>
#		include <cmath>
#		include <cfloat>
#	else
#		include <stdio.h>
#		include <time.h>
#		include <math.h>
#		include <float.h>
#	endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef UNITTEST_TIMEMICRO
#	ifdef __cplusplus
	inline unsigned long long unittest_timemicro() { return std::chrono::duration_cast<std::chrono::microseconds>( std::chrono::high_resolution_clock::now().time_since_epoch() ).count(); }
#	else
	inline unsigned long long unittest_timemicro() { struct timespec ts; timespec_get(&ts, TIME_UTC); return (ts.tv_sec * 1000000000L + ts.tv_nsec) * 0.001; }
#	endif
#endif

#ifndef UNITTEST_EPSILON
#	define UNITTEST_EPSILON_F FLT_EPSILON
#	define UNITTEST_EPSILON_D DBL_EPSILON
#endif

#ifdef __cplusplus
#	define UNITTEST_ABS_F abs
#	define UNITTEST_ABS_D abs
#else
#	define UNITTEST_ABS_F fabsf
#	define UNITTEST_ABS_D fabs
#endif

#define UNITTEST_XSTR(TXT) #TXT
#define UNITTEST_STR(TXT) UNITTEST_XSTR(TXT)

#define UNITTEST_PRINT(WHAT, ...) fprintf(stdout, WHAT, ##__VA_ARGS__)

#define UNITTEST_FILNUM __FILE__ ":" UNITTEST_STR(__LINE__)
#define UNITTEST_PRINTARG()

#define UNITTEST_SUCCESS() ++test_count_passed
#define UNITTEST_FAILURE() 

#define UNITTEST_CASE_BEGIN() ++test_count_all

/**
* @brief Initialize testing framework variables.
*/
#define UNITTEST_INIT() unsigned int unittest_all = 0, unittest_passed = 0;

// Traps

#ifdef UNITTEST
/**
 * @brief Create unit test trap
 */
#	define UNITTEST_TRAP_DEFINE(TRAPNAME) unsigned int unittest_trap_##TRAPNAME = false;

/**
 * @brief Create forward declaration of unit test trap for use in external files
 */
#	define UNITTEST_TRAP_LINK(TRAPNAME) extern unsigned int unittest_trap_##TRAPNAME;

/**
 * @brief Activate unit test trap
 */
#	define UNITTEST_TRAP_ACTIVATE(TRAPNAME) ++unittest_trap_##TRAPNAME;

#else

#	define UNITTEST_TRAP_DEFINE(TRAPNAME)
#	define UNITTEST_TRAP_LINK(TRAPNAME)
#	define UNITTEST_TRAP_ACTIVATE(TRAPNAME)

#endif

// Assertions

/**
 * @brief Assert that STATEMENT evaluates to true
 */
#define UNITTEST_ASSERT_TRUE(STATEMENT) \
	UNITTEST_CASE_BEGIN(); \
	if (STATEMENT) UNITTEST_SUCCESS(); \
	else { UNITTEST_FAILURE(); UNITTEST_PRINT(UNITTEST_FILNUM " -> test case FAILED: got FALSE instead of TRUE\r\n"); }

/**
* @brief Assert that STATEMENT evaluates to false
*/
#define UNITTEST_ASSERT_FALSE(STATEMENT) \
	UNITTEST_CASE_BEGIN(); \
	if (!(STATEMENT)) UNITTEST_SUCCESS(); \
	else { UNITTEST_FAILURE(); UNITTEST_PRINT(UNITTEST_FILNUM " -> test case FAILED: got TRUE instead of FALSE\r\n"); }

/**
* @brief Assert that STATEMENT is equal to EXPECTED using epsilon EPS to compare the numbers. This test is suited for single precision float values.
*/
#define UNITTEST_ASSERT_FLOAT_EQUAL_EPS(STATEMENT, EXPECTED, EPS) \
	UNITTEST_CASE_BEGIN(); \
	if (UNITTEST_ABS_F(STATEMENT - EXPECTED) <= EPS) UNITTEST_SUCCESS(); \
	else { UNITTEST_FAILURE(); UNITTEST_PRINT(UNITTEST_FILNUM " -> test case FAILED: got %f instead of %f \r\n", STATEMENT, EXPECTED); }

/**
* @brief Assert that STATEMENT is equal to EXPECTED using epsilon EPS to compare the numbers. This test is suited for double precision float values.
*/
#define UNITTEST_ASSERT_DOUBLE_EQUAL_EPS(STATEMENT, EXPECTED, EPS) \
	UNITTEST_CASE_BEGIN(); \
	if (UNITTEST_ABS_D(STATEMENT - EXPECTED) <= EPS) UNITTEST_SUCCESS(); \
	else { UNITTEST_FAILURE(); UNITTEST_PRINT(UNITTEST_FILNUM " -> test case FAILED: got %f instead of %f \r\n", STATEMENT, EXPECTED); }

/**
 * @brief Assert that STATEMENT is equal to EXPECTED using epsilon to compare the numbers. This test is suited for single precision float values.
 */
#define UNITTEST_ASSERT_FLOAT_EQUAL(STATEMENT, EXPECTED) UNITTEST_ASSERT_FLOAT_EQUAL_EPS(STATEMENT, EXPECTED, UNITTEST_EPSILON_F)

/**
* @brief Assert that STATEMENT is equal to EXPECTED using epsilon to compare the numbers. This test is suited for single precision float values.
*/
#define UNITTEST_ASSERT_DOUBLE_EQUAL(STATEMENT, EXPECTED) UNITTEST_ASSERT_DOUBLE_EQUAL_EPS(STATEMENT, EXPECTED, UNITTEST_EPSILON_D)

/**
* @brief Assert that STATEMENT is equal to EXPECTED using simple comparison operator.
*/
#define UNITTEST_ASSERT_EQUAL(STATEMENT, EXPECTED) \
	UNITTEST_CASE_BEGIN(); \
	if (STATEMENT == EXPECTED) UNITTEST_SUCCESS(); \
	else { UNITTEST_FAILURE(); UNITTEST_PRINT(UNITTEST_FILNUM " -> test case FAILED: arguments are different \r\n"); }

/**
* @brief Assert that integer STATEMENT is equal to integer EXPECTED.
*/
#define UNITTEST_ASSERT_INT_EQUAL(STATEMENT, EXPECTED) \
	UNITTEST_CASE_BEGIN(); \
	if (STATEMENT == EXPECTED) UNITTEST_SUCCESS(); \
	else { UNITTEST_FAILURE(); UNITTEST_PRINT(UNITTEST_FILNUM " -> test case FAILED: got %d instead of %d \r\n", STATEMENT, EXPECTED); }

/**
* @brief Assert that c-string STATEMENT is equal to c-string EXPECTED.
*/
#define UNITTEST_ASSERT_CSTR_EQUAL(STATEMENT, EXPECTED) \
	UNITTEST_CASE_BEGIN(); \
	if (strcmp(STATEMENT, EXPECTED) == 0) UNITTEST_SUCCESS(); \
	else { UNITTEST_FAILURE(); UNITTEST_PRINT(UNITTEST_FILNUM " -> test case FAILED: got %s instead of %s \r\n", STATEMENT, EXPECTED); }

/**
* @brief Begin time assertion block.
*/
#define UNITTEST_ASSERT_TIME_BEGIN() do { UNITTEST_CASE_BEGIN(); test_tim = unittest_timemicro(); } while(0)

/**
* @brief Finish time assertion block and assert that block execution time was not longer than MICROS microseconds.
*/
#define UNITTEST_ASSERT_TIME_NOTLONGER_END(MICROS) if (unittest_timemicro() - test_tim > MICROS) \
	{ UNITTEST_FAILURE(); UNITTEST_PRINT(UNITTEST_FILNUM " -> test case FAILED: code execution took %lluus while max was %lluus\r\n", unittest_timemicro() - test_tim, MICROS ) } else UNITTEST_SUCCESS(); test_tim = 0;

#ifdef UNITTEST
/**
 * @brief Begin trap hit assertion for TRAPNAME trap. Traps must be defined and activated using UNITTEST_TRAP_* macros
 */
#define UNITTEST_ASSERT_TRAP_START(TRAPNAME) do { unittest_trap_##TRAPNAME = false; } while(0)

/**
 * @brief Assert that trap TRAPNAME was hit
 */
#define UNITTEST_ASSERT_TRAP_HIT_END(TRAPNAME) \
	UNITTEST_CASE_BEGIN(); \
	if (unittest_trap_##TRAPNAME) UNITTEST_SUCCESS(); \
	else { UNITTEST_FAILURE(); UNITTEST_PRINT(UNITTEST_FILNUM " -> test case FAILED: trap \"" UNITTEST_STR(TRAPNAME) "\" was never hit\r\n"); }

 /**
  * @brief Assert that trap TRAPNAME was not hit
  */
#define UNITTEST_ASSERT_TRAP_NOT_HIT_END(TRAPNAME) \
	UNITTEST_CASE_BEGIN(); \
	if (unittest_trap_##TRAPNAME) { UNITTEST_FAILURE(); UNITTEST_PRINT(UNITTEST_FILNUM " -> test case FAILED: trap \"" UNITTEST_STR(TRAPNAME) "\" was hit\r\n"); } \
	else UNITTEST_SUCCESS();

/**
* @brief Assert that trap TRAPNAME was hit HITCOUNT times
*/
#define UNITTEST_ASSERT_TRAP_HIT_COUNT_END(TRAPNAME, HITCOUNT) \
	UNITTEST_CASE_BEGIN(); \
	if (unittest_trap_##TRAPNAME == HITCOUNT) UNITTEST_SUCCESS(); \
	else { UNITTEST_FAILURE(); UNITTEST_PRINT(UNITTEST_FILNUM " -> test case FAILED: trap \"" UNITTEST_STR(TRAPNAME) "\" was hit %d times, expected %d hits\r\n", unittest_trap_##TRAPNAME, HITCOUNT); }

/**
* @brief Assert that trap TRAPNAME was hit more than HITCOUNT times
*/
#define UNITTEST_ASSERT_TRAP_HIT_MORE_END(TRAPNAME, HITCOUNT) \
	UNITTEST_CASE_BEGIN(); \
	if (unittest_trap_##TRAPNAME > HITCOUNT) UNITTEST_SUCCESS(); \
	else { UNITTEST_FAILURE(); UNITTEST_PRINT(UNITTEST_FILNUM " -> test case FAILED: trap \"" UNITTEST_STR(TRAPNAME) "\" was hit %d times, expected at more than %d hits\r\n", unittest_trap_##TRAPNAME, HITCOUNT); }

/**
* @brief Assert that trap TRAPNAME was hit less than HITCOUNT times
*/
#define UNITTEST_ASSERT_TRAP_HIT_LESS_END(TRAPNAME, HITCOUNT) \
	UNITTEST_CASE_BEGIN(); \
	if (unittest_trap_##TRAPNAME < HITCOUNT) UNITTEST_SUCCESS(); \
	else { UNITTEST_FAILURE(); UNITTEST_PRINT(UNITTEST_FILNUM " -> test case FAILED: trap \"" UNITTEST_STR(TRAPNAME) "\" was hit %d times, expected less than %d hits\r\n", unittest_trap_##TRAPNAME, HITCOUNT); }

#else

#define UNITTEST_ASSERT_TRAP_START(TRAPNAME)
#define UNITTEST_ASSERT_TRAP_HIT_END(TRAPNAME)
#define UNITTEST_ASSERT_TRAP_NOT_HIT_END(TRAPNAME)
#define UNITTEST_ASSERT_TRAP_HIT_COUNT_END(TRAPNAME, HITCOUNT)
#define UNITTEST_ASSERT_TRAP_HIT_MORE_END(TRAPNAME, HITCOUNT)
#define UNITTEST_ASSERT_TRAP_HIT_LESS_END(TRAPNAME, HITCOUNT)

#endif

#ifdef __cplusplus
/**
* @brief Begin exception testing block.
*/
#	define UNITTEST_ASSERT_EXCEPTION_START UNITTEST_CASE_BEGIN(); try {

/**
* @brief Finish exception testing block and assert that no exception were thrown during block's execution.
*/
#	define UNITTEST_ASSERT_NOEXCEPTION_END } catch(std::exception &e) { test_exc = 1; UNITTEST_FAILURE(); UNITTEST_PRINT(UNITTEST_FILNUM " -> test case FAILED: an exception occurred: \"%s\"\r\n", e.what()); } \
											   catch(...) { test_exc = 1; UNITTEST_FAILURE(); UNITTEST_PRINT(UNITTEST_FILNUM " -> test case FAILED: an exception occurred\r\n"); } \
											   if ( test_exc == 0 ) UNITTEST_SUCCESS(); test_exc = 0;

/**
* @brief Finish exception testing block and assert that at least one exception was thrown during block's execution.
*/
#	define UNITTEST_ASSERT_THROWEXCEPTION_END } catch(...) { test_exc = 1; UNITTEST_SUCCESS(); } \
											   if ( test_exc == 0 ) { UNITTEST_FAILURE(); UNITTEST_PRINT(UNITTEST_FILNUM " -> test case FAILED: no exception was thrown\r\n"); } test_exc = 0;

/**
* @brief Finish exception testing block and assert that at least one exception was thrown during block's execution.
*/
#	define UNITTEST_ASSERT_EXCEPTIONTYPE_END(TYPE) } catch(TYPE) { test_exc = 1; UNITTEST_SUCCESS(); } catch(...) {} \
											   if ( test_exc == 0 ) { UNITTEST_FAILURE(); UNITTEST_PRINT(UNITTEST_FILNUM " -> test case FAILED: exception was thrown but none was of type \"" UNITTEST_STR(TYPE) "\"\r\n"); } test_exc = 0;

#endif

// Assertions end


// Unit test declaration

/**
* @brief Initialize unit test variables.
*/
#define UNITTEST_SETUP() unsigned long long test_tim = 0; int test_exc = 0; int test_count_all = 0; int test_count_passed = 0;

/**
* @brief Create unit test with standarized name
*/
#define UNITTEST_BEGIN(NAME) int test_##NAME()

/**
 * @brief Unit test forward declaration statement.
 * Should be used in header file containing test declaration, and the actual test should be defined later using UNITTEST_BEGIN macro.
 */
#define UNITTEST_DECLARE(NAME) UNITTEST_BEGIN(NAME)

/**
* @brief Finish unit test and display test results.
*/
#define UNITTEST_END() do { UNITTEST_PRINT("Test complete\r\n%d cases passed\r\n%d cases failed\r\n\r\n", test_count_passed, test_count_all - test_count_passed); return test_count_all == test_count_passed; } while (0)

// Unit test declaration end


// Unit test runblock

/**
* @brief Begin unit test execution block - all tests must run within this block.
*/
#define UNITTEST_RUNBLOCK_BEGIN() do { unittest_all = 0; unittest_passed = 0; UNITTEST_PRINT("\r\nBeginning tests\r\n\r\n"); } while(0)

/**
* @brief Execute unit test - this must be executed in unit test RUNBLOCK
*/
#define UNITTEST_EXEC(NAME) do { UNITTEST_PRINT("Running test \"" UNITTEST_STR(NAME) "\"\r\n"); ++unittest_all; unittest_passed += test_##NAME(); } while(0)

/**
* @brief Finish unit test execution block and display test results.
*/
#define UNITTEST_RUNBLOCK_END() do {\
		if (unittest_all == unittest_passed) { UNITTEST_PRINT("All %d tests complete SUCCESSFULLY\r\n\r\n", unittest_all); } \
		else { UNITTEST_PRINT("All %d tests complete\r\nSuccessful: %d\r\nFailed: %d\r\n\r\n", unittest_all, unittest_passed, unittest_all - unittest_passed); } \
	} while(0)

/**
 * @brief Get amount of failed tests
 */
#define UNITTEST_GET_FAILED() ( unittest_all - unittest_passed )

/**
 * @brief Get amount of passed tests
 */
#define UNITTEST_GET_PASSED() unittest_passed

/**
 * @brief Get amount of all executed tests
 */
#define UNITTEST_GET_ALL() unittest_all

/**
 * @brief Close program and return number of failed tests as the result code.
 */
#define UNITTEST_EXITPROG() do { exit(UNITTEST_GET_FAILED()); } while(0)

// Unit test runblock end

#ifdef __cplusplus
}
#endif

#endif