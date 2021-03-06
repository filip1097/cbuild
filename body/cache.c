/*> Description ******************************************************************************************************/ 
/** 
* @brief Defines functions to deal with cache.
* @file cache.c
*/

/*> Includes *********************************************************************************************************/
#include <globals.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>

#include "char_util.h"
#include "int_util.h"
#include "json_util.h"
#include "path_util.h"
#include "time_util.h"

/*> Defines **********************************************************************************************************/
#define CHECKSUM_STRING_LENGTH 20

/*> Type Declarations ************************************************************************************************/

/*> Global Constant Definitions **************************************************************************************/

/*> Global Variable Definitions **************************************************************************************/

/*> Local Constant Definitions ***************************************************************************************/

/*> Local Variable Definitions ***************************************************************************************/

/*> Local Function Declarations **************************************************************************************/
static JSON_Struct* create_file_json_object(File_List_Node_Struct* fileNode_p);

static void convert_checksum_to_string(uint64_t checksum, char checksumString[CHECKSUM_STRING_LENGTH]);

/*> Local Function Definitions ***************************************************************************************/
static JSON_Struct* create_file_json_object(File_List_Node_Struct* fileNode_p)
{
  JSON_Struct* fileObject_p = new_json_struct(JSON_TYPE_OBJECT, "");

  JSON_Struct* fileAttribute_p = new_json_struct(JSON_TYPE_STRING, "file");
  add_string_value_to_json(fileAttribute_p, fileNode_p->path);
  add_child_to_json(fileObject_p, fileAttribute_p);

  JSON_Struct* checksumAttribute_p = new_json_struct(JSON_TYPE_STRING, "checksum");
  char checksumString[CHECKSUM_STRING_LENGTH] = {0};
  uint64_t checksumToCache = 0;
  if (fileNode_p->failedCompilation)
  {
    File_List_Node_Struct* cachedFile = 
        find_file_node(&cachedFiles, fileNode_p->fileName_p, strlen(fileNode_p->fileName_p));
    if (cachedFile != NULL) checksumToCache = cachedFile->checksum;
  }
  else
  {
    checksumToCache = fileNode_p->checksum;
  }
  convert_checksum_to_string(checksumToCache, checksumString);
  add_string_value_to_json(checksumAttribute_p, checksumString);
  add_child_to_json(fileObject_p, checksumAttribute_p);

  JSON_Struct* extraArgumentList_p = new_json_struct(JSON_TYPE_ARRAY, "extraArgs");
  for (int i = 0; i < compilerArgumentCount; i++)
  {
    JSON_Struct* extraArg_p = new_json_struct(JSON_TYPE_STRING, "");
    add_string_value_to_json(extraArg_p, compilerArguments_pp[i]);
    add_child_to_json(extraArgumentList_p, extraArg_p);
  }
  add_child_to_json(fileObject_p, extraArgumentList_p);

  return fileObject_p;
}

static void convert_checksum_to_string(uint64_t checksum, char checksumString[CHECKSUM_STRING_LENGTH])
{
  checksumString[19] = '\0';

  for (int i = 18; i >= 0; i--)
  {
    checksumString[i] = char_parse_digit(checksum % 10);
    checksum = checksum / 10;
  }
}

/*> Global Function Definitions **************************************************************************************/
bool load_stored_cache(char* pathToCache_p)
{
  struct timeval start, end; 
  gettimeofday(&start, 0);

  bool foundCache = false;
  if (entry_exists(pathToCache_p))
  {
    JSON_Struct* jsonCache_p = parse_json(pathToCache_p);

    for (int i = 0; i < jsonCache_p->numChildren; i++)
    {
      JSON_Struct* fileObject_p = jsonCache_p->children[i];
      uint64_t checksum;
      char* name_p;
      JSON_Struct* jsonExtraArgList_p;

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
        else if (strcmp(attribute_p->name, "extraArgs") == 0)
        {
          jsonExtraArgList_p = attribute_p;
        }
      }

      add_to_file_list(&cachedFiles, name_p);
      cachedFiles.last->checksum = checksum;
      for (int j = 0; j < jsonExtraArgList_p->numChildren; j++)
      {
        add_extra_arg_to_file_node(cachedFiles.last, jsonExtraArgList_p->children[j]->stringValue);
      }
    }
    
    free_json(jsonCache_p);
    foundCache = true;
  }

  gettimeofday(&end, 0);
  double timeTaken = timeval_diff(&end, &start);
  printf("[%lf s] (%d) FIND AND LOAD STORED CACHE: ", timeTaken, stepCounter);
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

void write_cache(char* path_p)
{
  struct timeval start, end; 
  gettimeofday(&start, 0);

  JSON_Struct* jsonList_p = new_json_struct(JSON_TYPE_ARRAY, "");

  for (File_List_Node_Struct* node_p = cFiles.first; node_p != NULL; node_p = node_p->next) {
    JSON_Struct* fileObject_p = create_file_json_object(node_p);
    add_child_to_json(jsonList_p, fileObject_p);
  }

  for (File_List_Node_Struct* node_p = hFiles.first; node_p != NULL; node_p = node_p->next) {
    JSON_Struct* fileObject_p = create_file_json_object(node_p);
    add_child_to_json(jsonList_p, fileObject_p);
  }

  pretty_print_json_to_file(jsonList_p, path_p);

  free_json(jsonList_p);

  gettimeofday(&end, 0);
  double timeTaken = timeval_diff(&end, &start);
  printf("[%lf s] (%d) WRITE TO CACHE\n", timeTaken, stepCounter);
  stepCounter++;
}
