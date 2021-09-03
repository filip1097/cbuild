/*> Description ******************************************************************************************************/
/**
* @brief Contains the main method of the program.
* @file main.c
*/

/*> Includes *********************************************************************************************************/
#include <stdbool.h>
#include <stdio.h>

#include "cache_files.h"
#include "calc_dependencies.h"
#include "file_list.h"
#include "file_search.h"
#include "gcc_calls.h"
#include "globals.h"

/*> Defines **********************************************************************************************************/
#define CURRENT_DIR_PATH "."
#define CACHE_PATH "build/.cBuildCache"

/*> Type Declarations ************************************************************************************************/

/*> Global Constant Definitions **************************************************************************************/

/*> Global Variable Definitions **************************************************************************************/

/*> Local Constant Definitions ***************************************************************************************/

/*> Local Variable Definitions ***************************************************************************************/

/*> Local Function Declarations **************************************************************************************/

/*> Local Function Definitions ***************************************************************************************/
int main()
{
  find_files(CURRENT_DIR_PATH); 
  checksum_files();
  calc_dependencies();

  bool foundStoredCache = load_stored_cache(CACHE_PATH);
  if (foundStoredCache)
  {
    determine_files_to_be_recompiled();
  }

  do_gcc_calls();
  write_cache(CACHE_PATH);

  return 0;
}

/*> Global Function Definitions **************************************************************************************/
