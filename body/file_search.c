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
#include "file_search.h"
#include "globals.h"
#include "path_util.h"

/*> Defines **********************************************************************************************************/

/*> Type Declarations ************************************************************************************************/

/*> Global Constant Definitions **************************************************************************************/

/*> Global Variable Definitions **************************************************************************************/

/*> Local Constant Definitions ***************************************************************************************/

/*> Local Variable Definitions ***************************************************************************************/

/*> Local Function Declarations **************************************************************************************/
static void search_for_files(char* currentPath_p);

/*> Local Function Definitions ***************************************************************************************/
static void search_for_files(char* currentPath_p)
{
  DIR* dir_stream = opendir(currentPath_p);
  if (dir_stream != NULL)
  {
    struct dirent* dirEntry_p = readdir(dir_stream);
    while (dirEntry_p != NULL)
    {
      char dirEntryPath[MAX_PATH_LENGTH];
      join_paths(dirEntryPath, currentPath_p, dirEntry_p->d_name);
      struct stat dirEntryStatus;
      stat(dirEntryPath, &dirEntryStatus);

      if (S_ISDIR(dirEntryStatus.st_mode) && dirEntry_p->d_name[0] != '.')
      {
        search_for_files(dirEntryPath);
      }
      else if (S_ISREG(dirEntryStatus.st_mode))
      {
        char* firstPeriod_p = strchr(dirEntry_p->d_name, '.');
        if (firstPeriod_p != NULL)
        {
          if (strcmp(&firstPeriod_p[1], "c") == 0)
          {
            add_to_file_list(&cFiles, dirEntryPath);
          }
          else if (strcmp(&firstPeriod_p[1], "h") == 0)
          {
            add_to_file_list(&hFiles, dirEntryPath);
          }
          else if (strcmp(&firstPeriod_p[1], "o") == 0)
          {
            add_to_file_list(&oFiles, dirEntryPath);
          }
        }
      }
      dirEntry_p = readdir(dir_stream);
    }
    closedir(dir_stream);
  }
}

/*> Global Function Definitions **************************************************************************************/
void find_files(char* projectPath_p)
{
  search_for_files(projectPath_p);
  printf("(%d) STEP FIND FILES: C-files = %d, H-files = %d, O-files = %d\n", stepCounter, cFiles.length, hFiles.length, oFiles.length);
  stepCounter++;
}

