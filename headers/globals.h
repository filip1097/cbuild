/*> Description ******************************************************************************************************/
/**
 * @brief Header files to deal with the global variables.
 * @file globals.h
 */

/*> Multiple Inclusion Protection ************************************************************************************/
#ifndef GLOBALS_H
#define GLOBALS_H

/*> Includes *********************************************************************************************************/
#include "file_list.h"

/*> Defines **********************************************************************************************************/

/*> Type Declarations ************************************************************************************************/

/*> Constant Declarations ********************************************************************************************/

/*> Variable Declarations ********************************************************************************************/
extern File_List_Struct cFiles;
extern File_List_Struct hFiles;
extern File_List_Struct oFiles;
extern File_List_Struct cachedFiles;
extern int stepCounter;

/*> Function Declarations ********************************************************************************************/

/*> End of Multiple Inclusion Protection *****************************************************************************/
#endif 
