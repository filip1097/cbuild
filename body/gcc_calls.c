/*> Description ***********************************************************************************/
/**
* @brief Defines functions to call gcc.
* @file gcc_calls.c
*/

/*> Includes **************************************************************************************/
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "file_list.h"
#include "globals.h"
#include "path_util.h"
#include "time_util.h"

/*> Defines ***************************************************************************************/

#define MAX_INCLUDE_PATHS 100
#define MAX_COMMAND_LENGTH 1000
#define MAX_PATH_LENGTH 250

/*> Type Declarations *****************************************************************************/

/*> Global Constant Definitions *******************************************************************/

/*> Global Variable Definitions *******************************************************************/

/*> Local Constant Definitions ********************************************************************/

/*> Local Variable Definitions ********************************************************************/

/*> Local Function Declarations *******************************************************************/
static void determine_dir_paths_to_include(File_List_Node_Struct* fileNode_p,
                                           int* numDirsToInclude_p,
                                           Path_Part_Struct* dirPathsToInclude_p);
static void mkdir_if_not_exists(char* dirPath_p);
static void strcat_object_file_path(char* dest_p, char* buildFolderPath_p, char* fileName_p);
static void strcat_arguments(char* dest_p);

/*> Local Function Definitions ********************************************************************/
static void determine_dir_paths_to_include(File_List_Node_Struct* fileNode_p,
                                           int* numDirsToInclude_p, 
                                           Path_Part_Struct* dirPathsToInclude_p)
{
  for (int i = 0; i < fileNode_p->numDependencies; i++)
  {
    File_List_Node_Struct* dependency_p = fileNode_p->dependencies[i];
    Path_Part_Struct currentPathPart = get_dir_part(dependency_p->path);
    bool foundDirPath = false;

    for (int i = 0; i < *numDirsToInclude_p; i++)
    {
      if (path_part_equal(&currentPathPart, &(dirPathsToInclude_p[i])))
      {
        foundDirPath = true;
        break;
      }
    }

    if (!foundDirPath)
    {
      assert(*numDirsToInclude_p < MAX_INCLUDE_PATHS);
      dirPathsToInclude_p[*numDirsToInclude_p] = currentPathPart;
      (*numDirsToInclude_p)++;
    }
  }
}


static void mkdir_if_not_exists(char* dirPath_p)
{
  if (!entry_exists(dirPath_p))
  {
    make_directory(dirPath_p);
  }
}

static void strcat_object_file_path(char* dest_p, char* buildFolderPath_p, char* fileName_p)
{
  char* lastPeriodInFileName_p = strrchr(fileName_p, '.');
  int lengthOfFileNameNotIncludingExtension = lastPeriodInFileName_p - fileName_p;
  sprintf(dest_p, "%s%s%c", dest_p, buildFolderPath_p, PATH_SEPERATOR);
  strncat(dest_p, fileName_p, lengthOfFileNameNotIncludingExtension);
  strcat(dest_p, ".o ");
}

static void strcat_arguments(char* dest_p)
{
  for (int i = 0; i < compilerArgumentCount; i++)
  {
    int snprintfStatus =
        snprintf(dest_p, MAX_COMMAND_LENGTH, "%s%s ", dest_p, compilerArguments_pp[i]);
    assert(snprintfStatus >= 0);
  }
}

/*> Global Function Definitions *******************************************************************/
bool compile_object_files()
{
  struct timeval start, end; 
  gettimeofday(&start, 0);

  // TODO: ensure no buffer overflow
  char buildFolderPath[MAX_PATH_LENGTH];
  switch (buildMode)
  {
  case BUILD_PRODUCT:
    sprintf(buildFolderPath, ".%cbuild", PATH_SEPERATOR);
    break;
  case BUILD_TEST:
    sprintf(buildFolderPath, ".%ctest_build", PATH_SEPERATOR);
    break;
  }
  mkdir_if_not_exists(buildFolderPath);

  int numFilesFailingCompilation = 0;

  for (File_List_Node_Struct* node_p = cFiles.first; node_p != NULL; node_p = node_p->next)
  {
    if (!(node_p->toBeCompiled))
    {
      continue;
    }

    int numDirsToBeIncluded = 0;
    Path_Part_Struct dirPathsToBeIncluded[MAX_INCLUDE_PATHS] = {0};
    determine_dir_paths_to_include(node_p, &numDirsToBeIncluded, dirPathsToBeIncluded);

    // TODO: ensure no buffer overflow
    char command[MAX_COMMAND_LENGTH] = "gcc -c -Werror ";
    strcat_arguments(command);
    strcat(command, node_p->path);
    strcat(command, " -o ");
    strcat_object_file_path(command, buildFolderPath, node_p->fileName_p);
    if (numDirsToBeIncluded > 0)
    {
      strcat(command, " -I ");
      for (int i = 0; i < numDirsToBeIncluded; i++)
      {
        // remove corner case in windows: '.\' cannot be included, but '.' can.
#if defined _WIN32
        if (strncmp(dirPathsToBeIncluded[i].path_p, ".\\", dirPathsToBeIncluded[i].length) == 0)
        {
          dirPathsToBeIncluded[i].length = 1;
        }
#endif

        strncat(command, dirPathsToBeIncluded[i].path_p, dirPathsToBeIncluded[i].length);
        strcat(command, " ");
      }
    }

    printf("DO COMMAND: %s\n", command);
    int responseCode = system(command);
    node_p->failedCompilation = (responseCode != 0);
    if (node_p->failedCompilation)
    {
      numFilesFailingCompilation++; 
    }
  }

  gettimeofday(&end, 0);
  double timeTaken = timeval_diff(&end, &start);
  printf("[%lf s] (%d) COMPILE OBJECT FILES: %d files failed compilation\n", 
      timeTaken, stepCounter, numFilesFailingCompilation);
  stepCounter++;

  return (numFilesFailingCompilation == 0);
}

void call_linker()
{
  struct timeval start, end; 
  gettimeofday(&start, 0);

  // TODO: ensure no buffer overflow
  char buildFolderPath[MAX_PATH_LENGTH];
  switch (buildMode)
  {
  case BUILD_PRODUCT:
    sprintf(buildFolderPath, ".%cbuild", PATH_SEPERATOR);
    break;
  case BUILD_TEST:
    sprintf(buildFolderPath, ".%ctest_build", PATH_SEPERATOR);
    break;
  }

  char linkCommand[MAX_COMMAND_LENGTH]; 
  sprintf(linkCommand, "gcc -o %s ", executablePath);
  for (File_List_Node_Struct* node_p = cFiles.first; node_p != NULL; node_p = node_p->next) 
  {
    strcat_object_file_path(linkCommand, buildFolderPath, node_p->fileName_p);
  }
  printf("DO COMMAND: %s\n", linkCommand);
  int systemResponseCode = system(linkCommand);
  bool successfulLinking = systemResponseCode == 0;

  char* linkResultString = successfulLinking ? "successful" : "failed";

  gettimeofday(&end, 0);
  double timeTaken = timeval_diff(&end, &start);
  printf("[%lf s] (%d) CALL LINKER: %s\n", timeTaken, stepCounter, linkResultString);
  stepCounter++;
}

