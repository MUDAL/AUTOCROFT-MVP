#include <stdint.h>
#include <string.h>
#include "conversions.h"

/*Miniature lookup table for accessing 
a few powers of ten for string to integer
conversion.
*/
const uint32_t powerOfTen[5] = {1, 10, 100, 1000, 10000};

/**
@brief Converts integer to string.  
@param integer: Integer to be converted to a string.  
@param pBuffer: String equivalent of the integer. i.e.  
after the function call, the result of the conversion is  
stored here.
@return None
*/
void Conv_IntegerToString(uint32_t integer, char* pBuffer)
{
	if (integer == 0)
	{//Edge case  
		pBuffer[0] = '0';
		return;
	}
	uint32_t copyOfInt = integer;
	uint8_t noOfDigits = 0;

	while(copyOfInt > 0)
	{
		copyOfInt /= 10;
		noOfDigits++;
	}
	while (integer > 0)
	{
		pBuffer[noOfDigits - 1] = '0' + (integer % 10);
		integer /= 10;
		noOfDigits--;
	}
}

/**
@brief Converts string to integer.
@param pBuffer: Buffer/string to be converted to an integer.  
@return Integer equivalent of the string.  
@details Each character of the string is converted to  
an integer and multiplied by the appropriate power of 10.   
Example:  
'932':
'9' - '0' = 9
'3' - '0' = 3
'2' - '0' = 2

9 x 10^2 + 3 x 10^1 + 2 x 10^0 = 932.
*/
uint32_t Conv_StringToInteger(char* pBuffer)
{
  uint32_t integer = 0;
  uint8_t bufferLength = strlen(pBuffer);
	
  for (uint8_t i = 0; i < bufferLength; i++)
  {
    integer += ( (pBuffer[i] - '0') * powerOfTen[bufferLength - i - 1] );
  }
  return integer;
}
