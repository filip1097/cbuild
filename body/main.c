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

    #include <inttypes.h>
    #include "path_util.h"

/*> Defines **********************************************************************************************************/
#define CURRENT_DIR_PATH "."
#define CACHE_PATH ".cBuildCache"

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

  bool found_stored_cache = load_stored_cache(CACHE_PATH);
  if (found_stored_cache)
  {
    determine_files_to_be_recompiled();
  }

  do_gcc_calls();

  for (File_List_Node_Struct* node = c_files.first; node != NULL; node = node->next)
  {
    printf("C node: name=%s, cache=%" PRIu64 ", needsCompilation=%d\n", 
              node->path, node->checksum, node->needsRecompilation);
  }

  for (File_List_Node_Struct* node = h_files.first; node != NULL; node = node->next)
  {
    printf("H node: name=%s, cache=%" PRIu64 ", needsCompilation=%d\n", 
              node->path, node->checksum, node->needsRecompilation);
  }

  for (File_List_Node_Struct* node = cached_files.first; node != NULL; node = node->next)
  {
    printf("Cached node: name=%s, cache=%" PRIu64 ", needsCompilation=%d\n", 
              node->path, node->checksum, node->needsRecompilation);
  }

  // compile files and link

  // write cache

  return 0;
}

/*> Global Function Definitions **************************************************************************************/
