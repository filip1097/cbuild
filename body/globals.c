/*> Description ******************************************************************************************************/
/**
* @brief Defines the global variables used by program.
* @file globals.c
*/

/*> Includes *********************************************************************************************************/
#include "file_list.h"
#include "globals.h"

/*> Defines **********************************************************************************************************/

/*> Type Declarations ************************************************************************************************/

/*> Global Constant Definitions **************************************************************************************/

/*> Global Variable Definitions **************************************************************************************/
File_List_Struct cFiles = { .first = NULL, .last = NULL, .length = 0 };
File_List_Struct hFiles = { .first = NULL, .last = NULL, .length = 0 };
File_List_Struct oFiles = { .first = NULL, .last = NULL, .length = 0 };
File_List_Struct cachedFiles = { .first = NULL, .last = NULL, .length = 0 };
char executablePath[MAX_EXE_PATH_LENGTH];
bool foundExecutable = false;
int stepCounter = 1;
int argumentCount = 0;
char** arguments_pp = NULL;

/*> Local Constant Definitions ***************************************************************************************/

/*> Local Variable Definitions ***************************************************************************************/

/*> Local Function Declarations **************************************************************************************/

/*> Local Function Definitions ***************************************************************************************/

/*> Global Function Definitions **************************************************************************************/
