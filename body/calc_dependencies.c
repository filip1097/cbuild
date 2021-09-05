/*> Description ******************************************************************************************************/
/**
* @brief Contains functions to determine dependencies of files.
* @file calc_dependencies.c
*/

/*> Includes *********************************************************************************************************/
#include "file_list.h"
#include "globals.h"

/*> Defines **********************************************************************************************************/

/*> Type Declarations ************************************************************************************************/

/*> Global Constant Definitions **************************************************************************************/

/*> Global Variable Definitions **************************************************************************************/

/*> Local Constant Definitions ***************************************************************************************/

/*> Local Variable Definitions ***************************************************************************************/

/*> Local Function Declarations **************************************************************************************/
static void calc_dependencies_for_list(File_List_Struct* fileList_p);

/*> Local Function Definitions ***************************************************************************************/
static void calc_dependencies_for_list(File_List_Struct* fileList_p)
{
  File_List_Node_Struct* current_node = fileList_p->first;
  while (current_node != NULL)
  {
    int oldNumDependencies;
    int start_index = 0;
    do 
    {
      oldNumDependencies = current_node->numDependencies;
      for (int i = start_index; i < oldNumDependencies; i++)
      {
        add_dependencies_to_file_node(current_node, current_node->dependencies[i]);
      }
      start_index = oldNumDependencies;
    } while (current_node->numDependencies > oldNumDependencies);
    current_node = current_node->next;
  }
}

/*> Global Function Definitions **************************************************************************************/

void calc_dependencies() 
{
  calc_dependencies_for_list(&cFiles);
  calc_dependencies_for_list(&hFiles);
  printf("(%d) STEP CALCULATE DEPENDENCIES\n", stepCounter);
  stepCounter++;
}
