#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "fixedpoint.h"

// You can remove this once all of the functions are fully implemented
static Fixedpoint DUMMY;

Fixedpoint fixedpoint_create(uint64_t whole)
{
    Fixedpoint fixedPoint;

    fixedPoint.whole = whole;
    fixedPoint.frac = 0;
    fixedPoint.tag = VALID_NONNEGATIVE;

    return fixedPoint;
}

Fixedpoint fixedpoint_create2(uint64_t whole, uint64_t frac)
{
    Fixedpoint fixedPoint;

    fixedPoint.whole = whole;
    fixedPoint.frac = frac;
    fixedPoint.tag = VALID_NONNEGATIVE;

    return fixedPoint;
}

Fixedpoint fixedpoint_create_from_hex(const char *hex)
{
    // TODO
    Fixedpoint fixedPoint;
    // Initialize whole and frac in case the function ends early due to encountering an error
    fixedPoint.whole = 0;
    fixedPoint.frac = 0;

    // Max length of a valid string is 34 characters if of the form -X.Y where X and Y are 16 characters long
    if (strlen(hex) > 34 || !isValidHex(hex))
    {
        fixedPoint.tag = ERROR;
        return fixedPoint;
    }

    // TODO: parse valid hex
    parseHex(hex, &fixedPoint);

    return fixedPoint;
}

int isValidHex(const char *hex)
{
    // Find index of decimal point to determine sizes of the whole and fractional portions
    int decimalLocation = -1;
    // Marks index of the start of the whole portion, adjusted based on if there is a negative symbol
    int start = 0;
    for (size_t i = 0; i < strlen(hex); ++i)
    {
        if (hex[i] == '-' && i == 0)
        {
            // Negative symbol can only appear at beginning of string
            start = 1;
        }
        else if (hex[i] == '.' && decimalLocation == -1)
        {
            // Only the first decimal point is valid
            decimalLocation = i;

            // Calculate lengths of whole and fractional portions, restricting them to a 16 character maximum
            if (decimalLocation - start > 16)
            {
                return 0;
            }
            if (strlen(hex) - decimalLocation - 1 > 16)
            {
                return 0;
            }
        }
        else
        {
            if (!isValidChar(hex[i]))
            {
                return 0;
            }
        }
    }

    // If the loop ends without the decimal location being set, the length of the whole part is the entire string length
    if (decimalLocation == -1)
    {
        if (strlen(hex) - start > 16)
        {
            return 0;
        }
    }

    // All checks have passed
    return 1;
}

int isValidChar(char c)
{
    if (c >= 'a' && c <= 'f') // Lowercase
    {
        return 1;
    }
    else if (c >= 'A' && c <= 'F') // Uppercase
    {
        return 1;
    }
    else if (c >= '0' && c <= '9') // Number
    {
        return 1;
    }
    else // All other characters are invalid
    {
        return 0;
    }
}

void parseHex(const char *hex, Fixedpoint *ptrFixedPoint)
{
    // Create two C strings to hold each part of the hex
    char wholeString[17];
    char fracString[17];

    // Find location of the decimal point to slice the string
    const char *decimalLocation = strchr(hex, '.');

    // Temporary pointer in order to point to various parts of hex to effectively slice strings
    const char *temp;

    // Assign tag based on sign and assign temp to the beginning of the hex string when excluding the sign
    if (hex[0] == '-')
    {
        ptrFixedPoint->tag = VALID_NEGATIVE;
        temp = hex + 1;
    }
    else
    {
        ptrFixedPoint->tag = VALID_NONNEGATIVE;
        temp = hex;
    }

    if (decimalLocation == NULL)
    {
        // Put everything into wholeString if decimal is not found
        strcpy(wholeString, temp);
        strcpy(fracString, "0");
    }
    else
    {
        // Copy strings based on the location of the decimal
        strncpy(wholeString, temp, (decimalLocation - temp));
        wholeString[decimalLocation - temp] = '\0';

        strncpy(fracString, decimalLocation + 1, hex + strlen(hex) - decimalLocation - 1);
        // Pad the fracString with 0s on the end so the hex string takes up all 16 bits
        for (size_t i = hex + strlen(hex) - decimalLocation - 1; i < 16; ++i)
        {
            fracString[i] = '0';
        }
        fracString[16] = '\0';
    }

    // Convert individual hex strings to unsigned long and do assignment
    // https://stackoverflow.com/questions/10156409/convert-hex-string-char-to-int
    ptrFixedPoint->whole = strtoul(wholeString, NULL, 16);
    ptrFixedPoint->frac = strtoul(fracString, NULL, 16);
}

uint64_t fixedpoint_whole_part(Fixedpoint val)
{
    return val.whole;
}

uint64_t fixedpoint_frac_part(Fixedpoint val)
{
    return val.frac;
}

Fixedpoint fixedpoint_add(Fixedpoint left, Fixedpoint right)
{
    if(left.tag != right.tag)
    {
        // If they are different signs, convert negative one to positive and perform subtraction
        if(left.tag == VALID_NONNEGATIVE)
        {
            return fixedpoint_sub(left, fixedpoint_negate(right));
        }
        else
        {
            return fixedpoint_sub(right, fixedpoint_negate(left));
        }
    }
    else
    {
        // Normal addition with sign staying the same as whatever the sign of left and right are
        Fixedpoint sum;
        sum.frac = left.frac + right.frac;
        if(sum.frac < left.frac) // If overflow, we need to carry
        {
            sum.whole = 1UL + left.whole + right.whole;
        }
        else
        {
            sum.whole = left.whole + right.whole;
        }
        // Check for overflow in whole portion
        if(sum.whole < left.whole)
        {
            if(left.tag == VALID_NONNEGATIVE)
            {
                sum.tag = OVERFLOW_NEGATIVE;
            }
            else
            {
                sum.tag = OVERFLOW_POSITIVE;
            }
        }
        else
        {
            sum.tag = left.tag;
        }

        return sum;
    }
}

Fixedpoint fixedpoint_sub(Fixedpoint left, Fixedpoint right)
{
    if(left.tag != right.tag)
    {
        // If signs are different, we can calculate it using addition of two fixedpoints of the same sign
        return fixedpoint_add(left, fixedpoint_negate(right));
    }
    else
    {
        // Determine sign by comparing which one is larger and then subtract the larger magnitude from the smaller magnitude
        Fixedpoint difference;
        if(fixedpoint_compare(left, right) == 1)
        {
            difference.tag = VALID_NEGATIVE;
        }
        else
        {
            difference.tag = VALID_NONNEGATIVE;
        }

        if(compare_magnitude(left, right) == 1)
        {
            // Perform borrow
            if(right.frac > left.frac)
            {
                difference.whole = left.whole - right.whole - 1;
                difference.frac = (0xFFFFFFFFFFFFFFFFUL - right.frac) + left.frac + 1; 
            }
            else
            {
                difference.whole = left.whole - right.whole;
                difference.frac = left.frac - right.frac;
            }
        }
        else
        {
            if(left.frac > right.frac)
            {
                difference.whole = right.whole - left.whole -1;
                difference.frac = (0xFFFFFFFFFFFFFFFFUL - left.frac) + right.frac + 1;
            }
            else
            {
                difference.whole = right.whole - left.whole;
                difference.frac = right.frac - left.frac;
            }
        }
        
        return difference;
    }
}

Fixedpoint fixedpoint_negate(Fixedpoint val)
{
    Fixedpoint fixedPoint;
    fixedPoint.whole = val.whole;
    fixedPoint.frac = val.frac;
    fixedPoint.tag = VALID_NONNEGATIVE; // Assign to nonnegative so it works for the case where val is 0

    if (!fixedpoint_is_zero(val))
    {
        if(val.tag == VALID_NONNEGATIVE) // Only need to change value of fixedPoint if val is nonnegative
        {
            fixedPoint.tag = VALID_NEGATIVE;
        }
    }

    return fixedPoint;
}

Fixedpoint fixedpoint_halve(Fixedpoint val)
{
    // TODO: implement
    assert(0);
    return DUMMY;
}

Fixedpoint fixedpoint_double(Fixedpoint val)
{
    // TODO: implement
    assert(0);
    return DUMMY;
}

int fixedpoint_compare(Fixedpoint left, Fixedpoint right)
{
    // Compare signs
    if(left.tag == right.tag)
    {
        if(left.tag == VALID_NONNEGATIVE)
        {
            return compare_magnitude(left, right);
        }
        else
        {
            return -compare_magnitude(left, right);
        }
    }
    else
    {
        // Nonnegative is greater than negative
        if(left.tag == VALID_NONNEGATIVE)
        {
            return 1;
        }
        else
        {
            return -1;
        }
    }
}

int compare_magnitude(Fixedpoint left, Fixedpoint right)
{
    // Compare whole portions
    if(left.whole == right.whole)
    {
        if(left.frac == right.frac)
        {
            return 0;
        }
        else if(left.frac > right.frac)
        {
            return 1;
        }
        else
        {
            return -1;
        }
    }
    else if(left.whole > right.whole)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

int fixedpoint_is_zero(Fixedpoint val)
{
    if (!fixedpoint_is_valid(val))
    {
        return 0;
    }

    return val.whole == 0 && val.frac == 0;
}

int fixedpoint_is_err(Fixedpoint val)
{
    return !fixedpoint_is_valid(val);
}

int fixedpoint_is_neg(Fixedpoint val)
{
    if(fixedpoint_is_err(val) || val.tag == VALID_NONNEGATIVE)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

int fixedpoint_is_overflow_neg(Fixedpoint val)
{
    return val.tag == OVERFLOW_NEGATIVE;
}

int fixedpoint_is_overflow_pos(Fixedpoint val)
{
    return val.tag == OVERFLOW_POSITIVE;
}

int fixedpoint_is_underflow_neg(Fixedpoint val)
{
    return val.tag == UNDERFLOW_NEGATIVE;
}

int fixedpoint_is_underflow_pos(Fixedpoint val)
{
    return val.tag == UNDERFLOW_POSITIVE;
}

int fixedpoint_is_valid(Fixedpoint val)
{
    return val.tag == VALID_NEGATIVE || val.tag == VALID_NONNEGATIVE;
}

char *fixedpoint_format_as_hex(Fixedpoint val)
{
    // TODO: implement
    assert(0);
    char *s = malloc(20);
    strcpy(s, "<invalid>");
    return s;
}
