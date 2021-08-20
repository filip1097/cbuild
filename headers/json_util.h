/*> Description ******************************************************************************************************/
/**
 * @brief Header files to deal with JSON data.
 * @file globals.h
 */

/*> Multiple Inclusion Protection ************************************************************************************/
#ifndef JSON_UTIL_H
#define JSON_UTIL_H

/*> Includes *********************************************************************************************************/

/*> Defines **********************************************************************************************************/
#define JSON_MAX_STRING_LENGTH 1000
#define JSON_MAX_NUM_CHILDREN 1000

/*> Type Declarations ************************************************************************************************/
typedef enum JSON_Type_Enum
{
  JSON_TYPE_STRING,
  JSON_TYPE_NUMBER,
  JSON_TYPE_BOOLEAN,
  JSON_TYPE_NULL,
  JSON_TYPE_OBJECT,
  JSON_TYPE_ARRAY
} JSON_Type_Enum;

typedef struct JSON_Struct
{
  JSON_Type_Enum type;
  struct JSON_Struct* parent_p;

  bool hasName;
  char name[JSON_MAX_STRING_LENGTH];

  char stringValue[JSON_MAX_STRING_LENGTH];
  int numChildren;
  struct JSON_Struct* children[JSON_MAX_NUM_CHILDREN];
} JSON_Struct;

/*> Constant Declarations ********************************************************************************************/

/*> Variable Declarations ********************************************************************************************/

/*> Function Declarations ********************************************************************************************/
JSON_Struct* parse_json(char* filePath_p);

void free_json(JSON_Struct* json_p);

void pretty_print_json(JSON_Struct* json_p);

/*> End of Multiple Inclusion Protection *****************************************************************************/
#endif 
