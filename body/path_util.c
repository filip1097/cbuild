/*> Description ******************************************************************************************************/
/**
* @brief Defines helper functions for dealing with paths.
* @file path_util.c
*/

/*> Includes *********************************************************************************************************/
#include <assert.h>
#include <string.h>

#include "file_list.h"
#include "path_util.h"

/*> Defines **********************************************************************************************************/

/*> Type Declarations ************************************************************************************************/

/*> Global Constant Definitions **************************************************************************************/

/*> Global Variable Definitions **************************************************************************************/

/*> Local Constant Definitions ***************************************************************************************/

/*> Local Variable Definitions ***************************************************************************************/

/*> Local Function Declarations **************************************************************************************/

/*> Local Function Definitions ***************************************************************************************/

/*> Global Function Definitions **************************************************************************************/

void join_paths(char* dest, char* left_path, char* right_path)
{
  int left_length = strlen(left_path);
  int right_length = strlen(right_path);

  assert(MAX_PATH_LENGTH > left_length + right_length + 1);

  dest[0] = '\0';
  strcat(dest, left_path);
  strcat(dest, "\\");
  strcat(dest, right_path);
}

Path_Part_Struct get_dir_part(char* path_p)
{
  char* lastBackSlash_p = strrchr(path_p, '\\');
  Path_Part_Struct dirPath = {.path_p = path_p, .length = lastBackSlash_p - path_p + 1};
  return dirPath; 
}
