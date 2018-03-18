/**
 * @file unittest_main.c
 * @brief This file contains main() which calls all the unit test functions.
 *
 * @author Kovuru Nagarjuna Reddy
 * @date March 17, 2018
 *
 */

//***********************************************************************************
// Include files
//**********************************************************************************
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <pthread.h>
#include "main_task.h"
#include "logger_task.h"
#include "light_task.h"

//***********************************************************************************
//Function Prototypes
//***********************************************************************************
extern void test_light_regCheck(void **state);
extern void test_temp_regCheck(void **state);

//***********************************************************************************
//Function Definitions
//***********************************************************************************
/******************************************************************************
 * @brief main
 * this function calls all the unit testing functions
 *****************************************************************************/
int main(int argc, char **argv)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_light_regCheck),
    cmocka_unit_test(test_temp_regCheck),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
