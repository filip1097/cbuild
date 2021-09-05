/*> Description ******************************************************************************************************/ 
/** 
* @brief Defines functions to find and load stored cache.
* @file checksum_files.c
*/

/*> Includes *********************************************************************************************************/
#include <globals.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "json_util.h"
#include "int_util.h"
#include "path_util.h"

/*> Defines **********************************************************************************************************/

/*> Type Declarations ************************************************************************************************/

/*> Global Constant Definitions **************************************************************************************/

/*> Global Variable Definitions **************************************************************************************/

/*> Local Constant Definitions ***************************************************************************************/

/*> Local Variable Definitions ***************************************************************************************/

/*> Local Function Declarations **************************************************************************************/

/*> Local Function Definitions ***************************************************************************************/

/*> Global Function Definitions **************************************************************************************/
bool load_stored_cache(char* pathToCache_p)
{
  bool foundCache = false;
  if (entry_exists(pathToCache_p))
  {
    JSON_Struct* jsonCache_p = parse_json(pathToCache_p);

    for (int i = 0; i < jsonCache_p->numChildren; i++)
    {
      JSON_Struct* fileObject_p = jsonCache_p->children[i];
      uint64_t checksum;
      char* name_p;

      for (int j = 0; j < fileObject_p->numChildren; j++)
      {
        JSON_Struct* attribute_p = fileObject_p->children[j];
        
        if (strcmp(attribute_p->name, "file") == 0)
        {
          name_p = attribute_p->stringValue;
        }
        else if (strcmp(attribute_p->name, "checksum") == 0)
        {
          checksum = parseUint64_t(attribute_p->stringValue);
        }
      }

      add_to_file_list(&cachedFiles, name_p);
      cachedFiles.last->checksum = checksum;
    }
    
    free_json(jsonCache_p);
    foundCache = true;
  }
  printf("(%d) STEP FIND AND LOAD STORED CACHE: ", stepCounter);
  if (foundCache)
  {
    printf("true\n");
  }
  else
  {
    printf("false\n");
  }
  stepCounter++;
  return foundCache;
}


