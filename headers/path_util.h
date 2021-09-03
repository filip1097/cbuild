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
#include <string.h>
#include <sys/stat.h>

/*> Defines **********************************************************************************************************/
#if defined _WIN32
  #define PATH_SEPERATOR '\\'
#elif defined __linux__
  #define PATH_SEPERATOR '/'
#endif

/*> Type Declarations ************************************************************************************************/
typedef struct Path_Part_Struct
{
  char* path_p;
  int length;
} Path_Part_Struct;

/*> Constant Declarations ********************************************************************************************/

/*> Variable Declarations ********************************************************************************************/

/*> Function Declarations ********************************************************************************************/

static inline bool entry_exists(char* path_p)
{
  struct stat status;
  return stat(path_p, &status) == 0;
}

Path_Part_Struct get_dir_part(char* path_p);

void join_paths(char* dest, char* left_path, char* right_path);

static inline bool path_part_equal(Path_Part_Struct* pathPart1_p, Path_Part_Struct* pathPart2_p)
{
  return (pathPart1_p->length == pathPart2_p->length) && 
         (strncmp(pathPart1_p->path_p, pathPart2_p->path_p, pathPart1_p->length) == 0);
}

/*> End of Multiple Inclusion Protection *****************************************************************************/
#endif 
