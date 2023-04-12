#pragma once

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

typedef uint8_t byte;

// --------------------------------------------------------------------------------------------
// Helper for hex conversion
// --------------------------------------------------------------------------------------------

int from_hex_digit(char h)
{
    if (h >= '0' && h <= '9')
        return h - '0';
    else if (h >= 'a' && h <= 'f')
        return h - 'a' + 10;
    else if (h >= 'A' && h <= 'F')
        return h - 'A' + 10;
    else
        return -1;
}

// Convert hexadecimal string of char* to opcode of byte.
void from_hex(char *hex, byte *res)
{
    size_t length = strlen(hex);
    // Omit the optional 0x prefix.
    size_t hex_begin = (length >= 2 && hex[0] == '0' && hex[1] == 'x') ? 2 : 0;

    for (size_t i = hex_begin; i < length - 1; i += 2)
    {
        uint8_t r0 = from_hex_digit(hex[i]);
        uint8_t r1 = from_hex_digit(hex[i + 1]);
        uint8_t r = (r0 << 4) + r1;
        res[(i - hex_begin) / 2] = r;
    }
    return;
}

// Convert bytes to hexadecimal string of char*
void hex(byte* data, size_t size, char* out)
{
    out[0] = '0';
    out[1] = 'x';
    char hex_chars[16] = "0123456789abcdef";
    for (size_t i = 0; i < size; i++)
    {
        uint8_t b = data[i];
        out[i * 2 + 2] = hex_chars[b >> 4];
        out[i * 2 + 3] = hex_chars[b & 0xf];
    }
    return;
}