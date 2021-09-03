/*> Description ******************************************************************************************************/ 
/** 
* @brief Defines functions to cache files.
* @file cache_files.c
*/

/*> Includes *********************************************************************************************************/
#include <stdbool.h>
#include <string.h>

#include "char_util.h"
#include "file_list.h"
#include "globals.h"
#include "int_util.h"
#include "json_util.h"
#include "path_util.h"

/*> Defines **********************************************************************************************************/
#define CHAR_BIT_LENGTH 8

#define CHECKSUM_STRING_LENGTH 20

#define LINE_BUFFER_LENGTH 200

#define ESCAPE_CHAR '\\'

#define PLACEHOLDER_CHAR ' '

/*> Type Declarations ************************************************************************************************/
typedef struct Checksum_Struct
{
  uint64_t checksum;
  uint64_t char_buffer;
  int buffer_index;
} Checksum_Struct;

typedef struct Line_Buffer_Struct
{
  char buffer[LINE_BUFFER_LENGTH];
  int num_chars;
} Line_Buffer_Struct;

typedef struct Scanning_Status_Struct
{
  bool scanning_multi_line_comment;
  bool scanning_single_line_comment;
  bool scanning_string_literal;
  bool scanning_char_literal;
  bool scanning_directive;
  bool first_non_ws_on_line;
  char prev_char;
} Scanning_Status_Struct;

typedef struct Include_Line_Parser_Struct
{
  bool parsing_comment;
  bool parsing_filename;
  bool finished_parsing;
  int first_filename_char_index;
  int last_filename_char_index;
  char prev_char;
} Include_Line_Parser_Struct;

/*> Global Constant Definitions **************************************************************************************/

/*> Global Variable Definitions **************************************************************************************/

/*> Local Constant Definitions ***************************************************************************************/

/*> Local Variable Definitions ***************************************************************************************/

/*> Local Function Declarations **************************************************************************************/

static void checksum_file(File_List_Node_Struct* file);

static inline bool scanning_comment_or_literal(Scanning_Status_Struct* scan_status);

static void process_scanned_char(File_List_Node_Struct* file_node,
                                 Scanning_Status_Struct* scan_status, 
                                 Line_Buffer_Struct* scanned_line,
                                 char current_char);
static inline void process_scanned_single_qoute(Scanning_Status_Struct* scan_status);
static inline void process_scanned_double_qoute(Scanning_Status_Struct* scan_status);
static inline void process_scanned_star(Scanning_Status_Struct* scan_status);
static inline void process_scanned_slash(Scanning_Status_Struct* scan_status);
static inline void process_scanned_hash_sign(Scanning_Status_Struct* scan_status);
static inline void process_scanned_linefeed(File_List_Node_Struct* file_node,
                                            Scanning_Status_Struct* scan_status, 
                                            Line_Buffer_Struct* scanned_line);
static inline void update_ws_status(Scanning_Status_Struct* scan_status, char current_char);

static inline void update_checksum(Checksum_Struct* cache, char current_char);
static inline void finalize_checksum(Checksum_Struct* cache);

static inline void add_to_line_buffer(Line_Buffer_Struct* scanned_line, char ch);

static void parse_and_add_include(File_List_Node_Struct* file_node, Line_Buffer_Struct* scanned_line);

static JSON_Struct* create_file_json_object(File_List_Node_Struct* fileNode_p);

static void convert_checksum_to_string(uint64_t checksum, char checksumString[CHECKSUM_STRING_LENGTH]);

/*> Local Function Definitions ***************************************************************************************/

static void checksum_file(File_List_Node_Struct* fileNode_p)
{
  FILE* file = fopen(fileNode_p->path, "r");

  Checksum_Struct cache = { .checksum = 0UL, .char_buffer = 0UL, .buffer_index = 0 };
  Line_Buffer_Struct scanned_line = { .num_chars = 0 };
  Scanning_Status_Struct scan_status = { 
    .first_non_ws_on_line = true, .scanning_char_literal = false, .scanning_directive = false, 
    .scanning_multi_line_comment = false, .scanning_single_line_comment = false, .scanning_string_literal = false,
    .prev_char = PLACEHOLDER_CHAR};

  char current_char = fgetc(file);
  while (current_char != EOF) {
    /* escape chars */
    current_char = scan_status.prev_char == ESCAPE_CHAR ? PLACEHOLDER_CHAR : current_char;
    
    process_scanned_char(fileNode_p, &scan_status, &scanned_line, current_char);
    update_checksum(&cache, current_char);

    scan_status.prev_char = current_char;
    current_char = fgetc(file);
  }

  finalize_checksum(&cache);
  fileNode_p->checksum = cache.checksum;

  fclose(file);
}

static inline bool scanning_comment_or_literal(Scanning_Status_Struct* scan_status)
{
  return scan_status->scanning_char_literal || scan_status->scanning_string_literal || 
         scan_status->scanning_single_line_comment || scan_status->scanning_multi_line_comment;
}

static void process_scanned_char(File_List_Node_Struct* file_node,
                                 Scanning_Status_Struct* scan_status, 
                                 Line_Buffer_Struct* scanned_line,
                                 char current_char)
{
  add_to_line_buffer(scanned_line, current_char);
  switch (current_char)
  {
  case '/':
    process_scanned_slash(scan_status);
    break;
  case '"':
    process_scanned_double_qoute(scan_status);
    break;
  case '*':
    process_scanned_star(scan_status);
    break;
  case '\'':
    process_scanned_single_qoute(scan_status);
    break;
  case '\n':
    process_scanned_linefeed(file_node, scan_status, scanned_line);
    break;
  case '#':
    process_scanned_hash_sign(scan_status);
    break;
  }
  update_ws_status(scan_status, current_char);
}

static inline void process_scanned_single_qoute(Scanning_Status_Struct* scan_status) 
{
  if (!scanning_comment_or_literal(scan_status))
  {
    scan_status->scanning_char_literal = true;
  }
  else if (scan_status->scanning_char_literal)
  {
    scan_status->scanning_char_literal = false;
  }
}

static inline void process_scanned_double_qoute(Scanning_Status_Struct* scan_status) 
{
  if (!scanning_comment_or_literal(scan_status))
  {
    scan_status->scanning_string_literal = true;
  }
  else if (scan_status->scanning_string_literal)
  {
    scan_status->scanning_string_literal = false;
  }
}

static inline void process_scanned_star(Scanning_Status_Struct* scan_status) 
{
  if (!scanning_comment_or_literal(scan_status) && scan_status->prev_char == '/')
  {
    scan_status->scanning_multi_line_comment = true;
  }
}

static inline void process_scanned_slash(Scanning_Status_Struct* scan_status) 
{
  if (scan_status->scanning_multi_line_comment && scan_status->prev_char == '*')
  {
    scan_status->scanning_multi_line_comment = false;
  }
  else if (!scanning_comment_or_literal(scan_status) && scan_status->prev_char == '/')
  {
    scan_status->scanning_single_line_comment = true;
  }
}

static inline void process_scanned_hash_sign(Scanning_Status_Struct* scan_status) 
{
  if (scan_status->first_non_ws_on_line) 
  {
    scan_status->scanning_directive = true;
  }
}

static inline void process_scanned_linefeed(File_List_Node_Struct* file_node,
                                            Scanning_Status_Struct* scan_status, 
                                            Line_Buffer_Struct* scanned_line) 
{
  if (scan_status->scanning_single_line_comment)
  {
    scan_status->scanning_single_line_comment = false;
  }
  scan_status->first_non_ws_on_line = true;

  if (scan_status->scanning_directive)
  {
    scan_status->scanning_directive = false;
    add_to_line_buffer(scanned_line, '\0');
    parse_and_add_include(file_node, scanned_line);
  }
  scanned_line->num_chars = 0;
}

static inline void update_ws_status(Scanning_Status_Struct* scan_status, char current_char)
{
  if (current_char != ' ' && current_char != '\t' && current_char != '\r' && current_char != '\n')
  {
    scan_status->first_non_ws_on_line = false;
  } 
}

static inline void update_checksum(Checksum_Struct* cache, char current_char)
{
  cache->char_buffer = (cache->char_buffer << CHAR_BIT_LENGTH) + current_char; 
  cache->buffer_index++;
  if (cache->buffer_index == 8)
  {
    cache->checksum = cache->checksum ^ cache->char_buffer;
    cache->buffer_index = 0;
  }
}

static inline void finalize_checksum(Checksum_Struct* cache)
{
  if (cache->buffer_index != 0)
  {
    cache->checksum = cache->checksum ^ cache->char_buffer;
  }
}

static inline void add_to_line_buffer(Line_Buffer_Struct* line, char ch)
{
  line->buffer[line->num_chars] = ch;
  line->num_chars++;
}

static void parse_and_add_include(File_List_Node_Struct* file_node, Line_Buffer_Struct* scanned_line) 
{
  char* start_of_include = strchr(scanned_line->buffer, '#');
  if (strncmp(start_of_include + 1, "include", 7) == 0) 
  {
    Include_Line_Parser_Struct line_parser = { 
      .finished_parsing = false, .parsing_comment = false, .parsing_filename = false, .prev_char = PLACEHOLDER_CHAR };

    for (int i = 8; !line_parser.finished_parsing; i++)
    {
      switch (start_of_include[i])
      {
      case '/':
        if (line_parser.parsing_comment && line_parser.prev_char == '*') 
        {
          line_parser.parsing_comment = false;
        }
        break;
      case '*':
        if (!line_parser.parsing_comment && line_parser.prev_char == '/') 
        {
          line_parser.parsing_comment = true;
        }
        break;
      case '<':
        if (!line_parser.parsing_comment)
        {
          line_parser.parsing_filename = true;
          line_parser.first_filename_char_index = i + 1;
        }
        break;
      case '>':
        if (line_parser.parsing_filename)
        {
          line_parser.finished_parsing = true;
          line_parser.last_filename_char_index = i - 1;
        }
        break;
      case '"':
        if (!line_parser.parsing_comment)
        {
          if (!line_parser.parsing_filename)
          {
            line_parser.parsing_filename = true;
            line_parser.first_filename_char_index = i + 1;
          }
          else 
          {
            line_parser.finished_parsing = true;
            line_parser.last_filename_char_index = i - 1;
          }
        }
        break;
      }      
    }

    int file_name_length = line_parser.last_filename_char_index - line_parser.first_filename_char_index + 1;
    File_List_Node_Struct* include_file_node = 
        find_file_node(&h_files,
                      (start_of_include + line_parser.first_filename_char_index),
                      file_name_length);
    if (include_file_node != NULL)
    {
      add_dependency_to_file_node(file_node, include_file_node);
    }

  }
}

static JSON_Struct* create_file_json_object(File_List_Node_Struct* fileNode_p)
{
  JSON_Struct* fileObject_p = new_json_struct(JSON_TYPE_OBJECT, "");

  JSON_Struct* fileAttribute_p = new_json_struct(JSON_TYPE_STRING, "file");
  add_string_value_to_json(fileAttribute_p, fileNode_p->path);
  add_child_to_json(fileObject_p, fileAttribute_p);

  JSON_Struct* checksumAttribute_p = new_json_struct(JSON_TYPE_STRING, "checksum");
  char checksumString[CHECKSUM_STRING_LENGTH] = {0};
  convert_checksum_to_string(fileNode_p->checksum, checksumString);
  add_string_value_to_json(checksumAttribute_p, checksumString);
  add_child_to_json(fileObject_p, checksumAttribute_p);

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

void checksum_files()
{
  for (File_List_Node_Struct* fileNode_p = c_files.first; fileNode_p != NULL; fileNode_p = fileNode_p->next)
  {
    checksum_file(fileNode_p);
  }

  for (File_List_Node_Struct* fileNode_p = h_files.first; fileNode_p != NULL; fileNode_p = fileNode_p->next)
  {
    checksum_file(fileNode_p);
  }
}

bool load_stored_cache(char* pathToCache_p)
{
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

      add_to_file_list(&cached_files, name_p);
      cached_files.last->checksum = checksum;
    }
    
    free_json(jsonCache_p);
    
    return true;
  }
  return false;
}

void write_cache(char* path_p)
{
  JSON_Struct* jsonList_p = new_json_struct(JSON_TYPE_ARRAY, "");

  for (File_List_Node_Struct* node_p = c_files.first; node_p != NULL; node_p = node_p->next) {
    JSON_Struct* fileObject_p = create_file_json_object(node_p);
    add_child_to_json(jsonList_p, fileObject_p);
  }

  for (File_List_Node_Struct* node_p = h_files.first; node_p != NULL; node_p = node_p->next) {
    JSON_Struct* fileObject_p = create_file_json_object(node_p);
    add_child_to_json(jsonList_p, fileObject_p);
  }

  pretty_print_json_to_file(jsonList_p, path_p);

  free_json(jsonList_p);
}

void determine_files_to_be_recompiled()
{
  for (File_List_Node_Struct* node_p = c_files.first; node_p != NULL; node_p = node_p->next)
  {
    char* nodeName_p = node_p->fileName_p;
    int nameLength = strlen(nodeName_p);

    File_List_Node_Struct* cachedNode_p = find_file_node(&cached_files, nodeName_p, nameLength);
    if (cachedNode_p != NULL)
    {
      node_p->needsRecompilation = !(node_p->checksum == cachedNode_p->checksum);
    }
  }
}
