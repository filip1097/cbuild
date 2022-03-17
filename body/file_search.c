/*> Description ***********************************************************************************/
/**
* @brief Defines functions for searching files.
* @file file_search.c
*/

/*> Includes **************************************************************************************/
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

#include "file_list.h"
#include "file_search.h"
#include "globals.h"
#include "path_util.h"
#include "time_util.h"

/*> Defines ***************************************************************************************/

/*> Type Declarations *****************************************************************************/

/*> Global Constant Definitions *******************************************************************/

/*> Global Variable Definitions *******************************************************************/

/*> Local Constant Definitions ********************************************************************/

/*> Local Variable Definitions ********************************************************************/

/*> Local Function Declarations *******************************************************************/
static void search_for_files(char* currentPath_p);
static bool should_add_c_file(struct dirent* dirEntry_p);
static bool should_search_for_files_in_directory(struct dirent* dirEntry_p);
static void get_name_of_cwd(char* dest_p, int bufferSize);
static void get_path_of_executable(char* dest_p, int bufferSize);

/*> Local Function Definitions ********************************************************************/
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

      if (S_ISDIR(dirEntryStatus.st_mode))
      {
        if (should_search_for_files_in_directory(dirEntry_p))
        {
          search_for_files(dirEntryPath);
        }
      }
      else if (S_ISREG(dirEntryStatus.st_mode))
      {
        char* firstPeriod_p = strchr(dirEntry_p->d_name, '.');
        if (firstPeriod_p != NULL)
        {
          if (strcmp(&firstPeriod_p[1], "c") == 0)
          {
            if (should_add_c_file(dirEntry_p))
            {
              add_to_file_list(&cFiles, dirEntryPath);
            }
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

static bool should_add_c_file(struct dirent* dirEntry_p)
{
  char* name_p = dirEntry_p->d_name;

  bool should_add = true;
  if (buildMode == BUILD_TEST)
  {
    should_add = strcmp(name_p, "main.c") != 0;
  }

  return should_add;
}

static bool should_search_for_files_in_directory(struct dirent* dirEntry_p)
{
  char* name_p = dirEntry_p->d_name;

  bool should_search = name_p[0] != '.';
  switch (buildMode)
  {
  case BUILD_PRODUCT:
    should_search = should_search &&
                    strcmp(name_p, "test") != 0 &&
                    strcmp(name_p, "test_build") != 0;
    break;
  case BUILD_TEST:
    should_search = should_search && strcmp(name_p, "build") != 0;
    break;
  }

  return should_search;
}

static void get_name_of_cwd(char* dest_p, int bufferSize)
{
  char* absolutePathNameOfCwd_p = getcwd(dest_p, bufferSize);
  if (absolutePathNameOfCwd_p == NULL)
  {
    perror("getcwd returned NULL: "); 
    exit(1);
  }
  char* lastPathSeperator_p = strrchr(dest_p, PATH_SEPERATOR);
  strcpy(dest_p, lastPathSeperator_p + 1);
}

static void get_path_of_executable(char* dest_p, int bufferSize)
{
  char cwdName[bufferSize];
  switch (buildMode)
  {
  case BUILD_PRODUCT:
    get_name_of_cwd(cwdName, bufferSize);
    // TODO: check no buffer overflow
    sprintf(dest_p, "%s%s", buildDirPath, cwdName);
    break;
  case BUILD_TEST:
    // TODO: check no buffer overflow
    sprintf(dest_p, "%stest", buildDirPath);
    break;
  }
#if defined _WIN32
  strcat(dest_p, ".exe");
#endif
}

/*> Global Function Definitions *******************************************************************/
void find_files(char* projectPath_p)
{
  struct timeval start, end; 
  gettimeofday(&start, 0);

  search_for_files(projectPath_p);
  get_path_of_executable(executablePath, MAX_PATH_LENGTH);
  foundExecutable = entry_exists(executablePath);

  gettimeofday(&end, 0);
  double timeTaken = timeval_diff(&end, &start);
  printf("[%lf s] (%d) FIND FILES: C-files = %d, H-files = %d, O-files = %d, Executable = %d\n", 
      timeTaken, stepCounter, cFiles.length, hFiles.length, oFiles.length, foundExecutable);
  stepCounter++;
}

