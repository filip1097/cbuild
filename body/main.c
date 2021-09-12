/*> Description ******************************************************************************************************/
/**
* @brief Contains the main method of the program.
* @file main.c
*/

/*> Includes *********************************************************************************************************/
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#include "calc_compile.h"
#include "calc_dependencies.h"
#include "checksum_files.h"
#include "file_list.h"
#include "file_search.h"
#include "gcc_calls.h"
#include "globals.h"
#include "cache.h"

/*> Defines **********************************************************************************************************/
#define CURRENT_DIR_PATH "."
#if defined _WIN32
  #define CACHE_PATH "build\\.cBuildCache"
#elif defined __linux__
  #define CACHE_PATH "build/.cBuildCache"
#endif

/*> Type Declarations ************************************************************************************************/

/*> Global Constant Definitions **************************************************************************************/

/*> Global Variable Definitions **************************************************************************************/

/*> Local Constant Definitions ***************************************************************************************/

/*> Local Variable Definitions ***************************************************************************************/

/*> Local Function Declarations **************************************************************************************/

/*> Local Function Definitions ***************************************************************************************/
int main()
{
  clock_t start = clock();
  find_files(CURRENT_DIR_PATH); 
  checksum_and_find_includes();
  calc_dependencies();

  bool foundStoredCache = load_stored_cache(CACHE_PATH);
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
    write_cache(CACHE_PATH);
  }
  clock_t end = clock();
  double totalTime = ((double) (end - start)) / CLOCKS_PER_SEC;
  printf("Total time = %lf s\n", totalTime);

  return 0;
}

/*> Global Function Definitions **************************************************************************************/
