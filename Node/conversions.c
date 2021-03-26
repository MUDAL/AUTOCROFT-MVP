#include <stdint.h>
#include <string.h>
#include "conversions.h"

/*miniature lookup table for accessing 
a few powers of ten for string to integer
conversion.
*/
const uint32_t powerOfTen[5] = {1, 10, 100, 1000, 10000};

void Conv_Integer_To_String(uint32_t integer, char* pBuffer)
{
	if (integer == 0)
	{  
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

uint32_t Conv_String_To_Integer(char* pBuffer)
{
	/*
  each character of the string is converted to
  an integer and multiplied by the appropriate power
  of 10. e.g.

  '932':
  '9' - '0' = 9
  '3' - '0' = 3
  '2' - '0' = 2

  9 x 10^2 + 3 x 10^1 + 2 x 10^0 = 932.
  */
  uint32_t integer = 0;
  uint8_t bufferLength = strlen(pBuffer);
  for (uint8_t i = 0; i < bufferLength; i++)
  {
    integer += ( (pBuffer[i] - '0') * powerOfTen[bufferLength - i - 1] );
  }
  return integer;
}
	

