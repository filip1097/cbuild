/*> Description ******************************************************************************************************/
/**
* @brief Defines functions dealing with File_List_Struct.
* @file file_list.c
*/

/*> Includes *********************************************************************************************************/
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
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
static void print_file_list_node(File_List_Node_Struct* node_p);

/*> Local Function Definitions ***************************************************************************************/
static void print_file_list_node(File_List_Node_Struct* node_p)
{
  printf("Path: %s\n", node_p->path);
  printf("--compile: %d\n", node_p->toBeCompiled);
}

/*> Global Function Definitions **************************************************************************************/

void add_to_file_list(File_List_Struct* list_p, char* path_p)
{
  File_List_Node_Struct* newNode_p = malloc(sizeof(*newNode_p));
  newNode_p->numDependencies = 0;
  newNode_p->numExtraArgs = 0;
  newNode_p->next = NULL;
  newNode_p->toBeCompiled = true;
  newNode_p->failedCompilation = false;
  
  assert(strlen(path_p) < MAX_PATH_LENGTH);
  strcpy(newNode_p->path, path_p);
  char* lastPathSeperator_p = strrchr(newNode_p->path, PATH_SEPERATOR);
  if (lastPathSeperator_p == NULL)
  {
    newNode_p->fileName_p = newNode_p->path;
  }
  else{
    newNode_p->fileName_p = (lastPathSeperator_p + 1);
  }

  if (list_p->first == NULL) 
  {
    list_p->first = newNode_p;
    list_p->last = newNode_p;
  }
  else
  {
    list_p->last->next = newNode_p;
    list_p->last = newNode_p;
  }

  list_p->length++;
}

File_List_Node_Struct* find_file_node(File_List_Struct* list_p, char* const fileName_p, int fileNameLength)
{
  for (File_List_Node_Struct* node_p = list_p->first; node_p != NULL; node_p = node_p->next)
  {
    if (strncmp(node_p->fileName_p, fileName_p, fileNameLength) == 0)
    {
      return node_p;
    }
  }
  return NULL;
}

void add_dependencies_to_file_node(File_List_Node_Struct* dest_node, File_List_Node_Struct* src_node)
{
  for (int i = 0; i < src_node->numDependencies; i++)
  {
    assert(dest_node != src_node->dependencies[i]);

    bool already_added = false;
    for (int j = 0; j < dest_node->numDependencies; j++)
    {
      if (dest_node->dependencies[j] == src_node->dependencies[i])
      {
        already_added = true;
        break;
      }
    }
    if (!already_added)
    {
      add_dependency_to_file_node(dest_node, src_node->dependencies[i]);
    }
  }
}

void add_extra_arg_to_file_node(File_List_Node_Struct* destNode_p, char* extraArgString_p)
{
  assert(strlen(extraArgString_p) < MAX_ARG_LENGTH);
  strcpy(destNode_p->extraArgs[destNode_p->numExtraArgs], extraArgString_p);
  (destNode_p->numExtraArgs)++;
  assert(destNode_p->numExtraArgs <= MAX_NUM_ARGS);
}

void print_file_list(File_List_Struct* fileList_p)
{
  for (File_List_Node_Struct* node_p = fileList_p->first; node_p != NULL; node_p = node_p->next)
  {
    print_file_list_node(node_p);
  }
}
