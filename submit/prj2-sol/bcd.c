#include "bcd.h"

#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>

Bcd
get_bcd_digit(Bcd val, int indx){
	val = val >> (indx*4);
	val = val & (Bcd)(0xf);
	return val;
}

Bcd
set_bcd_digit(Bcd bcd, int indx, Bcd nbcd){
	bcd &= ~(((Bcd)(0xf)) << (indx*4));
	bcd |= (nbcd << (indx*4));
	return bcd;
}

/** Return BCD encoding of binary (which has normal binary representation).
 *
 *  Examples: binary_to_bcd(0xc) => 0x12;
 *            binary_to_bcd(0xff) => 0x255
 *
 *  If error is not NULL, sets *error to OVERFLOW_ERR if binary is too
 *  big for the Bcd type, otherwise *error is unchanged.
 */
Bcd
binary_to_bcd(Binary value, BcdError *error)
{		
	int index = 0;
	Bcd ret = 0;

	while (value > 0)
	{
		ret = set_bcd_digit(ret, index, value % 10);
		value = value / 10;
		index++;
	}
	
	if (index > MAX_BCD_DIGITS)
	{
		if (error != NULL)
		{
		*error = OVERFLOW_ERR;
		}
	}
	return ret;
}

/** Return binary encodin

g of BCD value bcd.
 *
 *  Examples: bcd_to_binary(0x12) => 0xc;
 *            bcd_to_binary(0x255) => 0xff
 *
 *  If error is not NULL, sets *error to BAD_VALUE_ERR if bcd contains
 *  a bad BCD digit.
 *  Cannot overflow since Binary can represent larger values than Bcd
 */
Binary
bcd_to_binary(Bcd bcd, BcdError *error)
{
	Binary ret = 0;
	Bcd test = 0;
	long i = 1;
	
	while (bcd > 0)
	{
		test = get_bcd_digit(bcd, 0);
		bcd >>= 4;
		if (error != NULL)
		{
			if (test > 9)
			{
				*error = BAD_VALUE_ERR;
				break;
			}
		}
		ret += test*(i);
		i*=10;
	}
	return ret;
}

/** Return BCD encoding of decimal number corresponding to string s.
 *  Behavior undefined on overflow or if s contains a non-digit
 *  character.  Sets *p to point to first non-digit char in s.
 *  Rougly equivalent to strtol().
 *
 *  If error is not NULL, sets *error to OVERFLOW_ERR if binary is too
 *  big for the Bcd type, otherwise *error is unchanged.
 */
Bcd
str_to_bcd(const char *s, const char **p, BcdError *error)
{
	Binary ret = 0;
	*p = s;
	int carryOut = 0;

	for(int count = 0; s[count] != '\0' ; count++)
	{		
		if(s[count] >= '0' && s[count] <= '9')
		{   
			ret *= 10;
			ret += s[count] - '0';
			++(*p);
			carryOut++;
		}
		else
		{
			break;
		}
	}

	if(error != NULL && carryOut > MAX_BCD_DIGITS)
	{
		*error = OVERFLOW_ERR;
	}

	return binary_to_bcd(ret, error);
}

/** Convert bcd to a NUL-terminated string in buf[] without any
 *  non-significant leading zeros.  Never write more than bufSize
 *  characters into buf.  The return value is the number of characters
 *  written (excluding the NUL character used to terminate strings).
 *
 *  If error is not NULL, sets *error to BAD_VALUE_ERR is bcd contains
 *  a BCD digit which is greater than 9, OVERFLOW_ERR if bufSize bytes
 *  is less than BCD_BUF_SIZE, otherwise *error is unchanged.
 */
int
bcd_to_str(Bcd bcd, char buf[], size_t bufSize, BcdError *error)
{
	int written = 0;
	
	if (bufSize < BCD_BUF_SIZE)
	{
		*error = OVERFLOW_ERR;
	}
	else
	{
		for (int i =0; i<bufSize;i++)
		{
			Binary digit = get_bcd_digit(bcd, i);
			if (digit < 10)
			{
				written = snprintf(buf, bufSize, "%" BCD_FORMAT_MODIFIER "x", bcd);
			}
			if (digit > 9 && error != NULL)
			{
				*error = BAD_VALUE_ERR;
			}
			if (error != NULL && bufSize < BCD_BUF_SIZE)
			{
				*error = OVERFLOW_ERR;
			}
		}
	}
  return written;
}

/** Return the BCD representation of the sum of BCD int's x and y.
 *
 *  If error is not NULL, sets *error to to BAD_VALUE_ERR is x or y
 *  contains a BCD digit which is greater than 9, OVERFLOW_ERR on
 *  overflow, otherwise *error is unchanged.
 */
Bcd
bcd_add(Bcd x, Bcd y, BcdError *error)
{	
	Binary bin_x = bcd_to_binary(x, error);
    Binary bin_y = bcd_to_binary(y, error);
	
    return binary_to_bcd(bin_x + bin_y, error);
}

/** Return the BCD representation of the product of BCD int's x and y.
 *
 * If error is not NULL, sets *error to to BAD_VALUE_ERR is x or y
 * contains a BCD digit which is greater than 9, OVERFLOW_ERR on
 * overflow, otherwise *error is unchanged.
 */
Bcd
bcd_multiply(Bcd x, Bcd y, BcdError *error)
{
	Binary bin_x = bcd_to_binary (x, error);
	Binary bin_y = bcd_to_binary (y, error); 
	
	return binary_to_bcd (bin_x * bin_y, error);
}
