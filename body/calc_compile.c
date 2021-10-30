/*> Description ******************************************************************************************************/ 
/** 
* @brief Defines functions to calculate which files to compile.
* @file calc_compile.c
*/

/*> Includes *********************************************************************************************************/
#include <string.h>
#include <sys/time.h>

#include "file_list.h"
#include "globals.h"
#include "time_util.h"

/*> Defines **********************************************************************************************************/

/*> Type Declarations ************************************************************************************************/

/*> Global Constant Definitions **************************************************************************************/

/*> Global Variable Definitions **************************************************************************************/

/*> Local Constant Definitions ***************************************************************************************/

/*> Local Variable Definitions ***************************************************************************************/

/*> Local Function Declarations **************************************************************************************/
static int calc_num_files_to_compile(File_List_Struct* fileList_p);
static void determine_files_that_have_changed(File_List_Struct* fileList_p);
static void determine_files_that_used_different_args(File_List_Struct* fileList_p);
static void determine_o_files_still_exist(File_List_Struct* fileList_p);

/*> Local Function Definitions ***************************************************************************************/
static int calc_num_files_to_compile(File_List_Struct* fileList_p)
{
  int numFilesToBeCompiled = 0;
  for (File_List_Node_Struct* node_p = fileList_p->first; node_p != NULL; node_p = node_p->next)
  {
    if (node_p->toBeCompiled)
    {
      numFilesToBeCompiled++;
    }
  }
  return numFilesToBeCompiled;
}

static void determine_files_that_have_changed(File_List_Struct* fileList_p)
{
  for (File_List_Node_Struct* node_p = fileList_p->first; node_p != NULL; node_p = node_p->next)
  {
    char* nodeName_p = node_p->fileName_p;
    int nameLength = strlen(nodeName_p);

    File_List_Node_Struct* cachedNode_p = find_file_node(&cachedFiles, nodeName_p, nameLength);
    if (cachedNode_p != NULL)
    {
      node_p->hasChanged = !(node_p->checksum == cachedNode_p->checksum);
    }
  }
}

static void determine_files_that_used_different_args(File_List_Struct* fileList_p)
{
  for (File_List_Node_Struct* node_p = fileList_p->first; node_p != NULL; node_p = node_p->next)
  {
    if (!node_p->toBeCompiled)
    {
      char* nodeName_p = node_p->fileName_p;
      int nameLength = strlen(nodeName_p);

      File_List_Node_Struct* cachedNode_p = find_file_node(&cachedFiles, nodeName_p, nameLength);

      if (cachedNode_p != NULL)
      {
        printf("%s, argc=%d, numExtraArgs=%d\n", nodeName_p, argumentCount, cachedNode_p->numExtraArgs);
        if (cachedNode_p->numExtraArgs != (argumentCount - 1))
        {
          node_p->toBeCompiled = true;
        }
        else
        {
          for (int i = 1; i < argumentCount; i++)
          {
            bool hasEquivalent = false;
            for (int j = 0; j < cachedNode_p->numExtraArgs; j++)
            {
              if (strcmp(arguments_pp[i], cachedNode_p->extraArgs[j]) == 0)
              {
                hasEquivalent = true;
                break;
              }
            }
            if (!hasEquivalent)
            {
              node_p->toBeCompiled = true;
            }
          }
        }
      }
    }
  }
}

static void determine_o_files_still_exist(File_List_Struct* fileList_p)
{
  for (File_List_Node_Struct* node_p = fileList_p->first; node_p != NULL; node_p = node_p->next)
  {
    char* nodeName_p = node_p->fileName_p;
    int nameLength = strlen(nodeName_p);
    // TODO: ensure no buffer overflow
    char objectNodeName[100];
    strcpy(objectNodeName, nodeName_p);
    objectNodeName[nameLength-1] = 'o';

    File_List_Node_Struct* objectNode_p = find_file_node(&oFiles, objectNodeName, nameLength);
    if (objectNode_p == NULL)
    {
      node_p->toBeCompiled = true;
    }
  }

}

/*> Global Function Definitions **************************************************************************************/
int determine_files_to_compile(bool foundCache)
{
  struct timeval start, end; 
  gettimeofday(&start, 0);

  if (foundCache)
  {
    determine_files_that_have_changed(&cFiles);
    determine_files_that_used_different_args(&cFiles);
    determine_files_that_have_changed(&hFiles);
    determine_o_files_still_exist(&cFiles);
  
    bool foundNewFileToCompile = false;
    do 
    {
      foundNewFileToCompile = false;
      for (File_List_Node_Struct* node_p = cFiles.first; node_p != NULL; node_p = node_p->next)
      {
        if (!node_p->toBeCompiled)
        {
          for (int i = 0; i < node_p->numDependencies; i++)
          {
            if (node_p->dependencies[i]->hasChanged)
            {
              node_p->toBeCompiled = true;
              foundNewFileToCompile = true;
            }
          }
        }
      }
    } while (foundNewFileToCompile);
  }

  int numFilesToBeCompiled = calc_num_files_to_compile(&cFiles);

  gettimeofday(&end, 0);
  double timeTaken = timeval_diff(&end, &start);
  printf("[%lf s] (%d) DETERMINE FILES TO BE COMPILED: %d files to be compiled\n", 
      timeTaken, stepCounter, numFilesToBeCompiled);
  stepCounter++;

  return numFilesToBeCompiled;
}
