/*> Description ******************************************************************************************************/
/**
 * @brief Header files to deal with the global variables.
 * @file globals.h
 */

/*> Multiple Inclusion Protection ************************************************************************************/
#ifndef GLOBALS_H
#define GLOBALS_H

/*> Includes *********************************************************************************************************/
#include <stdbool.h>

#include "file_list.h"

/*> Defines **********************************************************************************************************/
#define MAX_EXE_PATH_LENGTH 200

/*> Type Declarations ************************************************************************************************/

/*> Constant Declarations ********************************************************************************************/

/*> Variable Declarations ********************************************************************************************/
extern File_List_Struct cFiles;
extern File_List_Struct hFiles;
extern File_List_Struct oFiles;
extern File_List_Struct cachedFiles;
extern char executablePath[MAX_EXE_PATH_LENGTH];
extern bool foundExecutable;
extern int stepCounter;
extern int argumentCount;
extern char** arguments_pp;

/*> Function Declarations ********************************************************************************************/

/*> End of Multiple Inclusion Protection *****************************************************************************/
#endif 
