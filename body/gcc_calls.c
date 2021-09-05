/*> Description *********************************************************************************************/
/**
* @brief Defines functions to call gcc.
* @file gcc_calls.c
*/

/*> Includes ************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file_list.h"
#include "globals.h"
#include "path_util.h"

/*> Defines *************************************************************************************************/

#define MAX_INCLUDE_PATHS 100
#define MAX_COMMAND_LENGTH 1000
#define MAX_PATH_LENGTH 250

/*> Type Declarations ***************************************************************************************/

/*> Global Constant Definitions *****************************************************************************/

/*> Global Variable Definitions *****************************************************************************/

/*> Local Constant Definitions ******************************************************************************/

/*> Local Variable Definitions ******************************************************************************/

/*> Local Function Declarations *****************************************************************************/
static void mkdir_if_not_exists(char* dirPath_p);
static void strcat_object_file_path(char* dest_p, char* buildFolderPath_p, char* fileName_p);

/*> Local Function Definitions ******************************************************************************/
static void mkdir_if_not_exists(char* dirPath_p)
{
  if (!entry_exists(dirPath_p))
  {
    mkdir(dirPath_p, 0777);
  }

}

static void strcat_object_file_path(char* dest_p, char* buildFolderPath_p, char* fileName_p)
{
  char* lastPeriodInFileName_p = strrchr(fileName_p, '.');
  int lengthOfFileNameNotIncludingExtension = lastPeriodInFileName_p - fileName_p;
  strcat(dest_p, buildFolderPath_p);
  strcat(dest_p, "/");
  strncat(dest_p, fileName_p, lengthOfFileNameNotIncludingExtension);
  strcat(dest_p, ".o ");
}

/*> Global Function Definitions *****************************************************************************/

void do_gcc_calls()
{
  // TODO: ensure no buffer overflow
  char buildFolderPath[MAX_PATH_LENGTH];
  sprintf(buildFolderPath, ".%cbuild", PATH_SEPERATOR);

  mkdir_if_not_exists(buildFolderPath);

  for (File_List_Node_Struct* node_p = cFiles.first; node_p != NULL; node_p = node_p->next)
  {
    if (!(node_p->toBeCompiled))
    {
      continue;
    }

    int numDirsToBeIncluded = 0;
    Path_Part_Struct dirPathsToBeIncluded[MAX_INCLUDE_PATHS] = {0}; 

    for (int i = 0; i < node_p->numDependencies; i++)
    {
      File_List_Node_Struct* dependency_p = node_p->dependencies[i];
      Path_Part_Struct currentPathPart = get_dir_part(dependency_p->path);
      bool foundDirPath = false;

      for (int i = 0; i < numDirsToBeIncluded; i++)
      {
        if (path_part_equal(&currentPathPart, &(dirPathsToBeIncluded[i])))
        {
          foundDirPath = true;
          break;
        }
      }
      
      if (!foundDirPath)
      {
        assert(numDirsToBeIncluded < MAX_INCLUDE_PATHS);
        dirPathsToBeIncluded[numDirsToBeIncluded] = currentPathPart;
        numDirsToBeIncluded++;
      }
    }

    // TODO: ensure no buffer overflow
    char command[MAX_COMMAND_LENGTH] = "gcc -c ";
    if (numDirsToBeIncluded == 0)
    {
      strcat(command, node_p->path);
      strcat(command, " -o ");
      strcat_object_file_path(command, buildFolderPath, node_p->fileName_p);
      printf("DO COMMAND: %s\n", command);
      system(command);
    } 
    else 
    {
      strcat(command, node_p->path);
      strcat(command, " -o ");
      strcat_object_file_path(command, buildFolderPath, node_p->fileName_p);
      strcat(command, " -I ");
      for (int i = 0; i < numDirsToBeIncluded; i++)
      {
        strncat(command, dirPathsToBeIncluded[i].path_p, dirPathsToBeIncluded[i].length);
        strcat(command, " ");
      }
      printf("DO COMMAND: %s\n", command);
      system(command);
    }

  }


  char linkCommand[MAX_COMMAND_LENGTH] = "gcc ";
  strcat(linkCommand, "-o ./build/cbuild ");
  for (File_List_Node_Struct* node_p = cFiles.first; node_p != NULL; node_p = node_p->next) 
  {
    strcat_object_file_path(linkCommand, buildFolderPath, node_p->fileName_p);
  }
  printf("DO COMMAND: %s\n", linkCommand);
  system(linkCommand);

}

