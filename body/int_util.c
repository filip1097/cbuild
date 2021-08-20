/*> Description ******************************************************************************************************/
/**
* @brief Defines helper functions for dealing with ints.
* @file int_util.c
*/

/*> Includes *********************************************************************************************************/
#include <stdint.h>
#include <string.h>

#include "int_util.h"

/*> Defines **********************************************************************************************************/
#define ZERO_IN_ASCII '0'

/*> Type Declarations ************************************************************************************************/

/*> Global Constant Definitions **************************************************************************************/

/*> Global Variable Definitions **************************************************************************************/

/*> Local Constant Definitions ***************************************************************************************/

/*> Local Variable Definitions ***************************************************************************************/

/*> Local Function Declarations **************************************************************************************/
static int digitAsInt(char ch);

/*> Local Function Definitions ***************************************************************************************/
static int digitAsInt(char ch)
{
  return ch - ZERO_IN_ASCII;
}

/*> Global Function Definitions **************************************************************************************/
uint64_t parseUint64_t(char* string_p)
{
  int strLength = strlen(string_p);
  uint64_t newUint64_t = 0;

  for (int i = 0; i < strLength; i++)
  {
    newUint64_t *= 10;
    newUint64_t += digitAsInt(string_p[i]);
  }

  return newUint64_t;
}
