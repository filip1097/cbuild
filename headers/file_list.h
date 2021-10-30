/*> Description ******************************************************************************************************/
/**
 * @brief Header file for the struct file_list.
 * @file file_list.h
 */

/*> Multiple Inclusion Protection ************************************************************************************/
#ifndef FILE_LIST_H
#define FILE_LIST_H

/*> Includes *********************************************************************************************************/
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/*> Defines **********************************************************************************************************/
#define MAX_PATH_LENGTH 250
#define MAX_DEPENDENCIES 100
#define MAX_NUM_ARGS 20
#define MAX_ARG_LENGTH 50

/*> Type Declarations ************************************************************************************************/

typedef struct File_List_Node_Struct
{
  char path[MAX_PATH_LENGTH];
  char* fileName_p;
  uint64_t checksum;
  struct File_List_Node_Struct* dependencies[MAX_DEPENDENCIES];
  int numDependencies;

  int numExtraArgs;
  char extraArgs[MAX_NUM_ARGS][MAX_ARG_LENGTH];

  union {
    bool toBeCompiled;
    bool hasChanged;
  };
  bool failedCompilation;

  struct File_List_Node_Struct* next;
} File_List_Node_Struct;

typedef struct File_List_Struct 
{
  File_List_Node_Struct* first;
  File_List_Node_Struct* last;
  int length;
} File_List_Struct;

/*> Constant Declarations ********************************************************************************************/

/*> Variable Declarations ********************************************************************************************/

/*> Function Declarations ********************************************************************************************/

void add_to_file_list(File_List_Struct* list, char* path_p);

File_List_Node_Struct* find_file_node(File_List_Struct* list_p, char* const fileNameStart_p, int fileNameLength); 

void static inline add_dependency_to_file_node(File_List_Node_Struct* node, File_List_Node_Struct* dependency)
{
  assert(node->numDependencies < MAX_DEPENDENCIES);
  node->dependencies[node->numDependencies] = dependency;
  node->numDependencies++;
}

void add_dependencies_to_file_node(File_List_Node_Struct* dest_node, File_List_Node_Struct* src_node);

void add_extra_arg_to_file_node(File_List_Node_Struct* destNode_p, char* extraArgString_p);

void print_file_list(File_List_Struct* fileList_p);

/*> End of Multiple Inclusion Protection *****************************************************************************/
#endif 
