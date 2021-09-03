/*> Description ******************************************************************************************************/
/**
 * @brief Header file functions to cache files.
 * @file cache_files.h
 */

/*> Multiple Inclusion Protection ************************************************************************************/
#ifndef CACHE_FILES_H
#define CACHE_FILES_H

/*> Includes *********************************************************************************************************/
#include "file_list.h"

/*> Defines **********************************************************************************************************/

/*> Type Declarations ************************************************************************************************/

/*> Constant Declarations ********************************************************************************************/

/*> Variable Declarations ********************************************************************************************/

/*> Function Declarations ********************************************************************************************/

void checksum_files();

bool load_stored_cache(char* path_p);
void write_cache(char* path_p);

void determine_files_to_be_recompiled();

/*> End of Multiple Inclusion Protection *****************************************************************************/
#endif 
