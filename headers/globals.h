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

/*> Type Declarations ************************************************************************************************/
/**
 * @brief The mode of the program - whether it will build product code or test code.
 */
typedef enum BuildModeE
{
  BUILD_PRODUCT,
  BUILD_TEST
} BuildModeE;

/*> Constant Declarations ********************************************************************************************/

/*> Variable Declarations ********************************************************************************************/
extern File_List_Struct cFiles;
extern File_List_Struct hFiles;
extern File_List_Struct oFiles;
extern File_List_Struct cachedFiles;
extern char cachePath[MAX_PATH_LENGTH];
extern char buildDirPath[MAX_PATH_LENGTH];
extern char executablePath[MAX_PATH_LENGTH];
extern bool foundExecutable;
extern BuildModeE buildMode;
extern int stepCounter;
extern int compilerArgumentCount;
extern char** compilerArguments_pp;

/*> Function Declarations ********************************************************************************************/

/*> End of Multiple Inclusion Protection *****************************************************************************/
#endif 
