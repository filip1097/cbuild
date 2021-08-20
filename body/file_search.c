/*> Description ******************************************************************************************************/
/**
* @brief Defines functions for searching files.
* @file file_search.c
*/

/*> Includes *********************************************************************************************************/
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "file_list.h"
#include "globals.h"
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

void find_files(char* currentPath_p)
{
  DIR* dir_stream = opendir(currentPath_p);
  if (dir_stream != NULL)
  {
    struct dirent* dir_entry = readdir(dir_stream);
    while (dir_entry != NULL)
    {
      char dirEntryPath[MAX_PATH_LENGTH];
      join_paths(dirEntryPath, currentPath_p, dir_entry->d_name);
      struct stat dirEntryStatus;
      stat(dirEntryPath, &dirEntryStatus);

      if (S_ISDIR(dirEntryStatus.st_mode) && dir_entry->d_name[0] != '.')
      {
        find_files(dirEntryPath);
      }
      else if (S_ISREG(dirEntryStatus.st_mode))
      {
        char* first_period = strchr(dir_entry->d_name, '.');
        if (strcmp(&first_period[1], "c") == 0)
        {
          add_to_file_list(&c_files, dirEntryPath);
        }
        else if (strcmp(&first_period[1], "h") == 0)
        {
          add_to_file_list(&h_files, dirEntryPath);
        }
      }
      dir_entry = readdir(dir_stream);
    }
    closedir(dir_stream);
  }

}
