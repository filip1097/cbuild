/*> Description ******************************************************************************************************/
/**
* @brief Contains the main method of the program.
* @file main.c
*/

/*> Includes *********************************************************************************************************/
#include <stdbool.h>
#include <stdio.h>
#include <sys/time.h>

#include "cache.h"
#include "calc_compile.h"
#include "calc_dependencies.h"
#include "checksum_files.h"
#include "cli_parser.h"
#include "file_list.h"
#include "file_search.h"
#include "gcc_calls.h"
#include "globals.h"
#include "time_util.h"

/*> Defines **********************************************************************************************************/
#define CURRENT_DIR_PATH "."

/*> Type Declarations ************************************************************************************************/

/*> Global Constant Definitions **************************************************************************************/

/*> Global Variable Definitions **************************************************************************************/

/*> Local Constant Definitions ***************************************************************************************/

/*> Local Variable Definitions ***************************************************************************************/

/*> Local Function Declarations **************************************************************************************/

/*> Local Function Definitions ***************************************************************************************/
int main(int argc, char* argv[])
{
  struct timeval start, end;
  gettimeofday(&start, 0);

  parse_arguments(argc, argv);

  find_files(CURRENT_DIR_PATH); 
  checksum_and_find_includes();
  calc_dependencies();

  bool foundStoredCache = load_stored_cache(cachePath);
  int numFilesToCompile = determine_files_to_compile(foundStoredCache);
  bool successfulCompilation = true;

  if (numFilesToCompile > 0)
  {
    successfulCompilation = compile_object_files();
  }
  if ((numFilesToCompile > 0 && successfulCompilation) || !foundExecutable)
  {
    call_linker();
  }
  if (numFilesToCompile > 0)
  {
    write_cache(cachePath);
  }

  gettimeofday(&end, 0);
  double totalTime = timeval_diff(&end, &start);
  printf("Total time = %lf s\n", totalTime);

  return 0;
}

/*> Global Function Definitions **************************************************************************************/
