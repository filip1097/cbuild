/*> Description ******************************************************************************************************/
/**
* @brief Defines functions dealing with File_List_Struct.
* @file file_list.c
*/

/*> Includes *********************************************************************************************************/
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "file_list.h"

/*> Defines **********************************************************************************************************/

/*> Type Declarations ************************************************************************************************/

/*> Global Constant Definitions **************************************************************************************/

/*> Global Variable Definitions **************************************************************************************/

/*> Local Constant Definitions ***************************************************************************************/

/*> Local Variable Definitions ***************************************************************************************/

/*> Local Function Declarations **************************************************************************************/

/*> Local Function Definitions ***************************************************************************************/

/*> Global Function Definitions **************************************************************************************/

void add_to_file_list(File_List_Struct* list, char* path_p)
{
  File_List_Node_Struct* new_node = malloc(sizeof(*new_node));
  new_node->n_dependencies = 0;
  new_node->next = NULL;
  new_node->needsRecompilation = true;
  
  /* TODO: ensure buffer is not overflowed */
  strcpy(new_node->path, path_p);
  char* lastBackSlash = strrchr(path_p, '\\');
  new_node->fileName_p = (lastBackSlash + 1);

  if (list->first == NULL) 
  {
    list->first = new_node;
    list->last = new_node;
  }
  else
  {
    list->last->next = new_node;
    list->last = new_node;
  }
}

File_List_Node_Struct* find_file_node(File_List_Struct* list_p, char* fileName_p, int fileNameLength)
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
  for (int i = 0; i < src_node->n_dependencies; i++)
  {
    assert(dest_node != src_node->dependencies[i]);

    bool already_added = false;
    for (int j = 0; j < dest_node->n_dependencies; j++)
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
