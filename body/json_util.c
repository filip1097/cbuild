/*> Description ******************************************************************************************************/
/**
* @brief Defines functions deal with json files.
* @file json_util.c
*/

/*> Includes *********************************************************************************************************/
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "json_util.h"

/*> Defines **********************************************************************************************************/
#define PARSING_ERROR(...) {\
  printf("JSON parsing error:\n");\
  printf(__VA_ARGS__);\
  printf("\n");\
  exit(1);\
}
#define EMPTY_STRING "\0"

/*> Type Declarations ************************************************************************************************/
typedef enum JSON_Parsing_Stage_Enum
{
  PARSING_STAGE_EXPECTING_NEW_ELEMENT_IN_OBJECT,
  PARSING_STAGE_EXPECTING_NEW_ELEMENT_IN_ARRAY,
  PARSING_STAGE_EXPECTING_COLON,
  PARSING_STAGE_EXPECTING_COMMA_OR_OBJECT_CLOSE,
  PARSING_STAGE_EXPECTING_COMMA_OR_ARRAY_CLOSE,
  PARSING_STAGE_EXPECTING_VALUE,
  PARSING_STAGE_START,
} JSON_Parsing_Stage_Enum;

typedef struct JSON_Parser_Struct {
  bool finishedParsing;
  JSON_Parsing_Stage_Enum parsingStage;
  FILE* file_p;
  JSON_Struct* currentJSONStruct_p;
  char tempName[JSON_MAX_STRING_LENGTH];
} JSON_Parser_Struct;

/*> Global Constant Definitions **************************************************************************************/

/*> Global Variable Definitions **************************************************************************************/

/*> Local Constant Definitions ***************************************************************************************/

/*> Local Variable Definitions ***************************************************************************************/

/*> Local Function Declarations **************************************************************************************/
static JSON_Struct* parse_json_element(JSON_Parser_Struct* parser_p);
static void handle_parsing_stage_start(JSON_Parser_Struct* parser_p, char nextChar);
static void handle_parsing_stage_expecting_new_element_in_array(JSON_Parser_Struct* parser_p, char nextChar);
static void handle_parsing_stage_expecting_new_element_in_object(JSON_Parser_Struct* parser_p, char nextChar);
static void handle_parsing_stage_expecting_colon(JSON_Parser_Struct* parser_p, char nextChar);
static void handle_parsing_stage_expecting_value(JSON_Parser_Struct* parser_p, char nextChar);
static void handle_parsing_stage_expecting_comma_or_object_close(JSON_Parser_Struct* parser_p, char nextChar);
static void handle_parsing_stage_expecting_comma_or_array_close(JSON_Parser_Struct* parser_p, char nextChar);
static char get_next_char_not_ws(JSON_Parser_Struct* parser_p);
static void fill_string(JSON_Parser_Struct* parser_p, char* string_p);
static void create_json_struct(JSON_Parser_Struct* parser_p, JSON_Type_Enum type, char* name_p);
static void close_json_object_or_array(JSON_Parser_Struct* parser_p);
static void add_string_value(JSON_Parser_Struct* parser_p, char* string_p);

static void print_json(JSON_Struct* json_p, char* indent_p);
static void print_json_to_file(FILE* file_p, JSON_Struct* json_p, char* indent_p);

/*> Local Function Definitions ***************************************************************************************/
static JSON_Struct* parse_json_element(JSON_Parser_Struct* parser_p)
{
  JSON_Parsing_Stage_Enum firstStage = parser_p->parsingStage;
  
  while(!parser_p->finishedParsing)
  {
    char nextChar = get_next_char_not_ws(parser_p);
    switch (parser_p->parsingStage)
    {
    case PARSING_STAGE_START:
      handle_parsing_stage_start(parser_p, nextChar);
      break;
    case PARSING_STAGE_EXPECTING_NEW_ELEMENT_IN_ARRAY:
      handle_parsing_stage_expecting_new_element_in_array(parser_p, nextChar);
      break;
    case PARSING_STAGE_EXPECTING_NEW_ELEMENT_IN_OBJECT:
      handle_parsing_stage_expecting_new_element_in_object(parser_p, nextChar);
      break;
    case PARSING_STAGE_EXPECTING_COLON:
      handle_parsing_stage_expecting_colon(parser_p, nextChar);
      break;
    case PARSING_STAGE_EXPECTING_COMMA_OR_OBJECT_CLOSE:
      handle_parsing_stage_expecting_comma_or_object_close(parser_p, nextChar);
      break;
    case PARSING_STAGE_EXPECTING_COMMA_OR_ARRAY_CLOSE:
      handle_parsing_stage_expecting_comma_or_array_close(parser_p, nextChar);
      break;
    case PARSING_STAGE_EXPECTING_VALUE:
      handle_parsing_stage_expecting_value(parser_p, nextChar);
      break;
    }
  }

  return parser_p->currentJSONStruct_p;
}

static void handle_parsing_stage_start(JSON_Parser_Struct* parser_p, char nextChar)
{
  if (nextChar == '{')
  {
    parser_p->parsingStage = PARSING_STAGE_EXPECTING_NEW_ELEMENT_IN_OBJECT;
    create_json_struct(parser_p, JSON_TYPE_OBJECT, EMPTY_STRING);
  }
  else if (nextChar == '[')
  {
    parser_p->parsingStage = PARSING_STAGE_EXPECTING_NEW_ELEMENT_IN_ARRAY;
    create_json_struct(parser_p, JSON_TYPE_ARRAY, EMPTY_STRING);
  }
  else
  {
    PARSING_ERROR("expected '{' or '[' got '%c'", nextChar);
  }
}

static void handle_parsing_stage_expecting_new_element_in_array(JSON_Parser_Struct* parser_p, char nextChar)
{
  char string[JSON_MAX_STRING_LENGTH] = {0};

  if (nextChar == '"')
  {
    parser_p->parsingStage = PARSING_STAGE_EXPECTING_COMMA_OR_ARRAY_CLOSE;
    create_json_struct(parser_p, JSON_TYPE_STRING, "");
    fill_string(parser_p, string);
    add_string_value(parser_p, string);
  }
  else if (nextChar == '{')
  {
    parser_p->parsingStage = PARSING_STAGE_EXPECTING_NEW_ELEMENT_IN_OBJECT;
    create_json_struct(parser_p, JSON_TYPE_OBJECT, EMPTY_STRING);
  }
  else if (nextChar == '[')
  {
    parser_p->parsingStage = PARSING_STAGE_EXPECTING_NEW_ELEMENT_IN_ARRAY;
    create_json_struct(parser_p, JSON_TYPE_ARRAY, EMPTY_STRING);
  }
  else if (nextChar == ']')
  {
    close_json_object_or_array(parser_p);
  }
  else
  {
    PARSING_ERROR("expected '\"', '{', '[' or ']' got '%c'", nextChar);
  }
}

static void handle_parsing_stage_expecting_new_element_in_object(JSON_Parser_Struct* parser_p, char nextChar)
{
  if (nextChar == '"')
  {
    parser_p->parsingStage = PARSING_STAGE_EXPECTING_COLON;
    fill_string(parser_p, parser_p->tempName);
  }
  else if (nextChar == '}')
  {
    close_json_object_or_array(parser_p);
  }
  else
  {
    PARSING_ERROR("expected '\"' or '}' got '%c'", nextChar);
  }
}

static void handle_parsing_stage_expecting_colon(JSON_Parser_Struct* parser_p, char nextChar)
{
  if (nextChar == ':')
  {
    parser_p->parsingStage = PARSING_STAGE_EXPECTING_VALUE;
  }
  else
  {
    PARSING_ERROR("expected ':' got '%c'", nextChar);
  }
}

static void handle_parsing_stage_expecting_value(JSON_Parser_Struct* parser_p, char nextChar)
{
  char string[JSON_MAX_STRING_LENGTH] = {0};

  if (nextChar == '"')
  {
    parser_p->parsingStage = PARSING_STAGE_EXPECTING_COMMA_OR_OBJECT_CLOSE;
    fill_string(parser_p, string);
    create_json_struct(parser_p, JSON_TYPE_STRING, parser_p->tempName);
    add_string_value(parser_p, string);
  }
  else if (nextChar == '{')
  {
    parser_p->parsingStage = PARSING_STAGE_EXPECTING_NEW_ELEMENT_IN_OBJECT;
    create_json_struct(parser_p, JSON_TYPE_OBJECT, parser_p->tempName);
  }
  else if (nextChar == '[')
  {
    parser_p->parsingStage = PARSING_STAGE_EXPECTING_NEW_ELEMENT_IN_ARRAY;
    create_json_struct(parser_p, JSON_TYPE_ARRAY, parser_p->tempName);
  }
  else
  {
    PARSING_ERROR("expected '\"', '{' or '[' got '%c'", nextChar);
  }
}

static void handle_parsing_stage_expecting_comma_or_object_close(JSON_Parser_Struct* parser_p, char nextChar)
{
  if (nextChar == ',')
  {
    parser_p->parsingStage = PARSING_STAGE_EXPECTING_NEW_ELEMENT_IN_OBJECT;
  }
  else if (nextChar == '}')
  {
    close_json_object_or_array(parser_p);
  } 
  else
  {
    PARSING_ERROR("expected ',', or '}' got '%c'", nextChar);
  }
}

static void handle_parsing_stage_expecting_comma_or_array_close(JSON_Parser_Struct* parser_p, char nextChar)
{
  switch (nextChar)
  {
  case ',':
    parser_p->parsingStage = PARSING_STAGE_EXPECTING_NEW_ELEMENT_IN_ARRAY;
    break;
  case ']':
    close_json_object_or_array(parser_p);
    break;
  default:
    PARSING_ERROR("expected ',', or ']' got '%c'", nextChar);
  }
}

static void create_json_struct(JSON_Parser_Struct* parser_p, JSON_Type_Enum type, char* name_p)
{
  JSON_Struct* oldStruct_p = parser_p->currentJSONStruct_p;
  JSON_Struct* newStruct_p = new_json_struct(type, name_p);
  newStruct_p->parent_p = oldStruct_p;

  if (type == JSON_TYPE_OBJECT || type == JSON_TYPE_ARRAY)
  {
    parser_p->currentJSONStruct_p = newStruct_p;
  }

  if (oldStruct_p != NULL)
  {
    oldStruct_p->children[oldStruct_p->numChildren] = newStruct_p;
    oldStruct_p->numChildren++;
  }
}

static char get_next_char_not_ws(JSON_Parser_Struct* parser_p)
{
  char nextChar = fgetc(parser_p->file_p);
  while (nextChar == ' ' || nextChar == '\t' || nextChar == '\n')
  {
    nextChar = fgetc(parser_p->file_p);
  }
  
  assert(nextChar != EOF);

  return nextChar;
}

static void fill_string(JSON_Parser_Struct* parser_p, char* string_p)
{
  int charIndex = 0;
  char nextChar = fgetc(parser_p->file_p);
  while (nextChar != '"')
  {
    assert(nextChar != EOF);
    assert(charIndex < JSON_MAX_STRING_LENGTH);
    string_p[charIndex] = nextChar;
    nextChar = fgetc(parser_p->file_p);
    charIndex++;
  }

  assert(charIndex < JSON_MAX_STRING_LENGTH);
  string_p[charIndex] = '\0';
  
}

static void close_json_object_or_array(JSON_Parser_Struct* parser_p)
{
  JSON_Struct* currentJSON_p = parser_p->currentJSONStruct_p;
  
  if (currentJSON_p->parent_p == NULL)
  {
    parser_p->finishedParsing = true;
  }
  else
  {
    parser_p->currentJSONStruct_p = currentJSON_p->parent_p;

    JSON_Type_Enum parentsType = currentJSON_p->parent_p->type;
    switch (parentsType)
    {
    case JSON_TYPE_OBJECT:
      parser_p->parsingStage = PARSING_STAGE_EXPECTING_COMMA_OR_OBJECT_CLOSE;
      break;
    case JSON_TYPE_ARRAY:
      parser_p->parsingStage = PARSING_STAGE_EXPECTING_COMMA_OR_ARRAY_CLOSE;
      break;
    default:
      PARSING_ERROR("Failed JSON Type");
    }
  }
}

static void add_string_value(JSON_Parser_Struct* parser_p, char* string_p)
{
  int latestChildIndex = parser_p->currentJSONStruct_p->numChildren - 1;
  JSON_Struct* latestChild = parser_p->currentJSONStruct_p->children[latestChildIndex];
  strcpy(latestChild->stringValue, string_p);
}

static void print_json(JSON_Struct* json_p, char* indent_p)
{
  // print indent_p
  printf("%s", indent_p);

  if (json_p->hasName)
  {
    printf("\"%s\" : ", json_p->name);
  }

  switch(json_p->type)
  {
  case JSON_TYPE_OBJECT:
    printf("{\n");
    break;
  case JSON_TYPE_ARRAY:
    printf("[\n");
    break;
  case JSON_TYPE_STRING:
    printf("\"%s\"", json_p->stringValue);
    break;
  }

  char newIndent[JSON_MAX_STRING_LENGTH] = {0};
  strcpy(newIndent, indent_p);
  strcat(newIndent, "  ");
  for (int i = 0; i < json_p->numChildren; i++)
  {
    print_json(json_p->children[i], newIndent);
    if (i != json_p->numChildren - 1)
    {
      printf(",\n");
    }
    else 
    {
      printf("\n");
    }
  }

  switch(json_p->type)
  {
  case JSON_TYPE_OBJECT:
    printf("%s}", indent_p);
    break;
  case JSON_TYPE_ARRAY:
    printf("%s]", indent_p);
    break;
  }
}

static void print_json_to_file(FILE* file_p, JSON_Struct* json_p, char* indent_p)
{
  // print indent_p
  fprintf(file_p, "%s", indent_p);

  if (json_p->hasName)
  {
    fprintf(file_p, "\"%s\" : ", json_p->name);
  }

  switch(json_p->type)
  {
  case JSON_TYPE_OBJECT:
    fprintf(file_p, "{\n");
    break;
  case JSON_TYPE_ARRAY:
    fprintf(file_p, "[\n");
    break;
  case JSON_TYPE_STRING:
    fprintf(file_p, "\"%s\"", json_p->stringValue);
    break;
  }

  char newIndent[JSON_MAX_STRING_LENGTH] = {0};
  strcpy(newIndent, indent_p);
  strcat(newIndent, "  ");
  for (int i = 0; i < json_p->numChildren; i++)
  {
    print_json_to_file(file_p, json_p->children[i], newIndent);
    if (i != json_p->numChildren - 1)
    {
      fprintf(file_p, ",\n");
    }
    else 
    {
      fprintf(file_p, "\n");
    }
  }

  switch(json_p->type)
  {
  case JSON_TYPE_OBJECT:
    fprintf(file_p, "%s}", indent_p);
    break;
  case JSON_TYPE_ARRAY:
    fprintf(file_p, "%s]", indent_p);
    break;
  }
}

/*> Global Function Definitions **************************************************************************************/

void add_child_to_json(JSON_Struct* parent_p, JSON_Struct* newChild_p)
{
  newChild_p->parent_p = parent_p;

  parent_p->children[parent_p->numChildren] = newChild_p;
  parent_p->numChildren++;
  assert(parent_p->numChildren < JSON_MAX_NUM_CHILDREN);
}

JSON_Struct* parse_json(char* filePath_p)
{
  FILE* file_p = fopen(filePath_p, "r");
  JSON_Parser_Struct jsonParser = 
      {.finishedParsing= false, .parsingStage = PARSING_STAGE_START, .file_p = file_p, .currentJSONStruct_p = NULL,
      .tempName = "\0"};
  
  JSON_Struct* json = parse_json_element(&jsonParser);

  fclose(file_p);

  return json;
}

void free_json(JSON_Struct* json_p)
{
  for (int i = 0; i < json_p->numChildren; i++)
  {
    free_json(json_p->children[i]);
  }
  free(json_p);
}

JSON_Struct* new_json_struct(JSON_Type_Enum type, char* name_p)
{
  JSON_Struct* newStruct_p = malloc(sizeof(*newStruct_p));
  newStruct_p->type = type;
  newStruct_p->parent_p = NULL;
  newStruct_p->numChildren = 0;
  newStruct_p->hasName = strlen(name_p) != 0;
  strcpy(newStruct_p->name, name_p);
  return newStruct_p;
}

void pretty_print_json(JSON_Struct* json_p)
{
  printf("\n");
  print_json(json_p, "");
  printf("\n");
}

void pretty_print_json_to_file(JSON_Struct* json_p, char* filePath_p)
{
  FILE* file_p = fopen(filePath_p, "w");
  print_json_to_file(file_p, json_p, "");
  fclose(file_p);
}
