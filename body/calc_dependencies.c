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
    int old_n_dependencies;
    int start_index = 0;
    do 
    {
      old_n_dependencies = current_node->n_dependencies;
      for (int i = start_index; i < old_n_dependencies; i++)
      {
        add_dependencies_to_file_node(current_node, current_node->dependencies[i]);
      }
      start_index = old_n_dependencies;
    } while (current_node->n_dependencies > old_n_dependencies);
    current_node = current_node->next;
  }
}

/*> Global Function Definitions **************************************************************************************/

void calc_dependencies() 
{
  calc_dependencies_for_list(&c_files);
  calc_dependencies_for_list(&h_files);
}
