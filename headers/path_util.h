/*> Description ******************************************************************************************************/
/**
 * @brief Header file for helper functions on paths.
 * @file path_util.h
 */

/*> Multiple Inclusion Protection ************************************************************************************/
#ifndef PATH_UTIL_H
#define PATH_UTIL_H

/*> Includes *********************************************************************************************************/

#include <stdbool.h>
#include <sys/stat.h>

/*> Defines **********************************************************************************************************/

/*> Type Declarations ************************************************************************************************/
typedef struct Path_Part_Struct
{
  char* path_p;
  int length;
} Path_Path_Struct;

/*> Constant Declarations ********************************************************************************************/

/*> Variable Declarations ********************************************************************************************/

/*> Function Declarations ********************************************************************************************/

void join_paths(char* dest, char* left_path, char* right_path);

Path_Part_Struct get_dir_part(char* path_p);

static inline bool file_exists(char* path_p)
{
  struct stat status;
  return stat(path_p, &status) == 0;
}

/*> End of Multiple Inclusion Protection *****************************************************************************/
#endif 
