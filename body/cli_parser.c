/*> Description ******************************************************************************************************/ 
/** 
* @brief Defines functions to parse CLI arguments.
* @file cli_parser.c
*/

/*> Includes *********************************************************************************************************/
#include <string.h>
#include <sys/time.h>

#include "globals.h"
#include "path_util.h"
#include "time_util.h"

/*> Defines **********************************************************************************************************/
#define PRODUCT_STRING "Product"
#define TEST_STRING "Test"
#define UNKOWN_STRING "Unknown"

/*> Type Declarations ************************************************************************************************/

/*> Global Constant Definitions **************************************************************************************/

/*> Global Variable Definitions **************************************************************************************/

/*> Local Constant Definitions ***************************************************************************************/

/*> Local Variable Definitions ***************************************************************************************/

/*> Local Function Declarations **************************************************************************************/
char* get_build_mode_string(BuildModeE buildMode);

/*> Local Function Definitions ***************************************************************************************/
char* get_build_mode_string(BuildModeE buildMode)
{
  switch (buildMode)
  {
  case BUILD_PRODUCT:
    return PRODUCT_STRING;
  case BUILD_TEST:
    return TEST_STRING;
  default:
    return UNKOWN_STRING;
  }
}

/*> Global Function Definitions **************************************************************************************/
void parse_arguments(int argumentCount, char** arguments_pp)
{
  struct timeval start, end; 
  gettimeofday(&start, 0);

  if (argumentCount > 1 && strcmp(arguments_pp[1], "--test") == 0)
  {
    compilerArgumentCount = argumentCount - 2;
    compilerArguments_pp = &arguments_pp[2];
    buildMode = BUILD_TEST;
    sprintf(cachePath, ".%ctest_build%c.cBuildCache", PATH_SEPERATOR, PATH_SEPERATOR);
  }
  else
  {
    compilerArgumentCount = argumentCount - 1;
    compilerArguments_pp = &arguments_pp[1];
    buildMode = BUILD_PRODUCT;
    sprintf(cachePath, ".%cbuild%c.cBuildCache", PATH_SEPERATOR, PATH_SEPERATOR);
  }

  gettimeofday(&end, 0);
  double timeTaken = timeval_diff(&end, &start);
  printf("[%lf s] (%d) PARSE INPUT ARGUMENTS: build %s code, number compiler arguments = %d \n",
      timeTaken, stepCounter, get_build_mode_string(buildMode), compilerArgumentCount);
  stepCounter++;
}