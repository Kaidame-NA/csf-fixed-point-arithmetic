#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "fixedpoint.h"

Fixedpoint fixedpoint_create(uint64_t whole)
{
    Fixedpoint fixedpoint;

    fixedpoint.whole = whole;
    fixedpoint.frac = 0;
    fixedpoint.tag = VALID_NONNEGATIVE;

    return fixedpoint;
}

Fixedpoint fixedpoint_create2(uint64_t whole, uint64_t frac)
{
    Fixedpoint fixedpoint;

    fixedpoint.whole = whole;
    fixedpoint.frac = frac;
    fixedpoint.tag = VALID_NONNEGATIVE;

    return fixedpoint;
}

Fixedpoint fixedpoint_create_from_hex(const char *hex)
{
    Fixedpoint fixedpoint;
    // Initialize whole and frac in case the function ends early due to encountering an error
    fixedpoint.whole = 0;
    fixedpoint.frac = 0;

    // Max length of a valid string is 34 characters if of the form -X.Y where X and Y are 16 characters long
    if (strlen(hex) > 34 || !is_valid_hex(hex))
    {
        fixedpoint.tag = ERROR;
        return fixedpoint;
    }

    parse_hex(hex, &fixedpoint);

    return fixedpoint;
}

int is_valid_hex(const char *hex)
{
    // Find index of decimal point to determine sizes of the whole and fractional portions
    int decimal_location = -1;
    // Marks index of the start of the whole portion, adjusted based on if there is a negative symbol
    int start = 0;
    for (size_t i = 0; i < strlen(hex); ++i)
    {
        if (hex[i] == '-' && i == 0)
        {
            // Negative symbol can only appear at beginning of string
            start = 1;
        }
        else if (hex[i] == '.' && decimal_location == -1)
        {
            // Only the first decimal point is valid
            decimal_location = i;

            // Calculate lengths of whole and fractional portions, restricting them to a 16 character maximum
            if (decimal_location - start > 16)
            {
                return 0;
            }
            if (strlen(hex) - decimal_location - 1 > 16)
            {
                return 0;
            }
        }
        else
        {
            if (!is_valid_char(hex[i]))
            {
                return 0;
            }
        }
    }

    // If the loop ends without the decimal location being set, the length of the whole part is the entire string length
    if (decimal_location == -1)
    {
        if (strlen(hex) - start > 16)
        {
            return 0;
        }
    }

    // All checks have passed
    return 1;
}

int is_valid_char(char c)
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

void parse_hex(const char *hex, Fixedpoint *ptr_fixedpoint)
{
    // Create two C strings to hold each part of the hex
    char whole_string[17];
    char frac_string[17];

    // Find location of the decimal point to slice the string
    const char *decimal_location = strchr(hex, '.');

    // Temporary pointer in order to point to various parts of hex to effectively slice strings
    const char *temp;

    // Assign tag based on sign and assign temp to the beginning of the hex string when excluding the sign
    if (hex[0] == '-')
    {
        ptr_fixedpoint->tag = VALID_NEGATIVE;
        temp = hex + 1;
    }
    else
    {
        ptr_fixedpoint->tag = VALID_NONNEGATIVE;
        temp = hex;
    }

    if (decimal_location == NULL)
    {
        // Put everything into wholeString if decimal is not found
        strcpy(whole_string, temp);
        strcpy(frac_string, "0");
    }
    else
    {
        // Copy strings based on the location of the decimal
        strncpy(whole_string, temp, (decimal_location - temp));
        whole_string[decimal_location - temp] = '\0';

        strncpy(frac_string, decimal_location + 1, hex + strlen(hex) - decimal_location - 1);
        // Pad the fracString with 0s on the end so the hex string takes up all 16 bits
        for (size_t i = hex + strlen(hex) - decimal_location - 1; i < 16; ++i)
        {
            frac_string[i] = '0';
        }
        frac_string[16] = '\0';
    }

    // Convert individual hex strings to unsigned long and do assignment
    // https://stackoverflow.com/questions/10156409/convert-hex-string-char-to-int
    ptr_fixedpoint->whole = strtoul(whole_string, NULL, 16);
    ptr_fixedpoint->frac = strtoul(frac_string, NULL, 16);

    // Handle edge case of -0
    if (ptr_fixedpoint->whole == 0 && ptr_fixedpoint->frac == 0)
    {
        ptr_fixedpoint->tag = VALID_NONNEGATIVE;
    }
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
    if (left.tag != right.tag)
    {
        // If they are different signs, convert negative one to positive and perform subtraction
        if (left.tag == VALID_NONNEGATIVE)
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
        if (sum.frac < left.frac) // If overflow in fractional, we need to carry
        {
            sum.whole = 1UL + left.whole + right.whole;
        }
        else
        {
            sum.whole = left.whole + right.whole;
        }
        sum.tag = left.tag;

        // Check for overflow
        if (compare_magnitude(left, sum) == 1)
        {
            if (left.tag == VALID_NONNEGATIVE)
            {
                sum.tag = OVERFLOW_POSITIVE;
            }
            else
            {
                sum.tag = OVERFLOW_NEGATIVE;
            }
        }

        return sum;
    }
}

Fixedpoint fixedpoint_sub(Fixedpoint left, Fixedpoint right)
{
    if (left.tag != right.tag)
    {
        // If signs are different, we can calculate it using addition of two fixedpoints of the same sign
        return fixedpoint_add(left, fixedpoint_negate(right));
    }
    else
    {
        // Determine sign by comparing which one is larger and then subtract the larger magnitude from the smaller magnitude
        Fixedpoint difference;
        if (fixedpoint_compare(left, right) >= 0)
        {
            difference.tag = VALID_NONNEGATIVE;
        }
        else
        {
            difference.tag = VALID_NEGATIVE;
        }

        if (compare_magnitude(left, right) == 1)
        {
            // Perform borrow
            if (right.frac > left.frac)
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
            if (left.frac > right.frac)
            {
                difference.whole = right.whole - left.whole - 1;
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
    Fixedpoint fixedpoint;
    fixedpoint.whole = val.whole;
    fixedpoint.frac = val.frac;
    fixedpoint.tag = VALID_NONNEGATIVE; // Assign to nonnegative so it works for the case where val is 0

    if (!fixedpoint_is_zero(val))
    {
        if (val.tag == VALID_NONNEGATIVE) // Only need to change value of fixedPoint if val is nonnegative
        {
            fixedpoint.tag = VALID_NEGATIVE;
        }
    }

    return fixedpoint;
}

Fixedpoint fixedpoint_halve(Fixedpoint val)
{
    Fixedpoint quotient;
    quotient.whole = val.whole / 2;
    quotient.frac = val.frac / 2;
    // If least significant bit in front of decimal is 1, we add 0.5 to fractional part after halving
    if (val.whole & 1UL)
    {
        quotient.frac += (0x8000000000000000UL);
    }

    // If least significant bit in frac part is 1, we encounter underflow
    if (val.frac & 1UL)
    {
        if (val.tag == VALID_NONNEGATIVE)
        {
            quotient.tag = UNDERFLOW_POSITIVE;
        }
        else
        {
            quotient.tag = UNDERFLOW_NEGATIVE;
        }
    }
    else
    {
        quotient.tag = val.tag;
    }

    return quotient;
}

Fixedpoint fixedpoint_double(Fixedpoint val)
{
    return fixedpoint_add(val, val);
}

int fixedpoint_compare(Fixedpoint left, Fixedpoint right)
{
    // Compare signs
    if (left.tag == right.tag)
    {
        if (left.tag == VALID_NONNEGATIVE)
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
        if (left.tag == VALID_NONNEGATIVE)
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
    // Compare whole portionsA and then frac portions
    if (left.whole == right.whole)
    {
        if (left.frac == right.frac)
        {
            return 0;
        }
        else if (left.frac > right.frac)
        {
            return 1;
        }
        else
        {
            return -1;
        }
    }
    else if (left.whole > right.whole)
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
    if (fixedpoint_is_err(val) || val.tag == VALID_NONNEGATIVE)
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
    char *s = calloc(35, sizeof(char));
    size_t next_index = 0;
    if (fixedpoint_is_err(val))
    {
        strcpy(s, "<invalid>");
    }
    else
    {
        // Check for negative sign
        if (fixedpoint_is_neg(val))
        {
            s[next_index++] = '-';
        }

        // Convert whole part to hex string
        // https://stackoverflow.com/questions/3464194/how-can-i-convert-an-integer-to-a-hexadecimal-string-in-c
        char whole[17];
        sprintf(whole, "%lx", val.whole);
        // Use pointer arithmetic to place in the right place
        strcpy(s + next_index, whole);
        next_index += strlen(whole);

        // Check fractional part to see whether to put decimal point
        if (val.frac == 0UL)
        {
            s[next_index] = '\0';
            return s;
        }
        else
        {
            s[next_index++] = '.';
        }

        // Convert frac part to string
        char frac[17];
        // Remove trailing 0s
        int zeroes_removed = 0;
        uint64_t frac_copy = val.frac;
        while ((frac_copy & 0xFUL) == 0)
        {
            frac_copy >>= 4;
            // printf("%lu\n", fracCopy);
            zeroes_removed++;
        }
        sprintf(frac, "%lx", frac_copy);

        // Add zeroes after the decimal point if needed
        for (int i = strlen(frac) + zeroes_removed; i < 16; i++)
        {
            s[next_index++] = '0';
        }
        strcpy(s + next_index, frac);
    }
    return s;
}
