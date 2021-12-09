#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "utf8.h"

/* UTF-8 Decoder based on the ImGui work by ocornut (https://github.com/ocornut/imgui) */

inline static int utf8_min(int x, int y)
{
	return (y < x) ? y : x;
}

// Convert UTF-8 to 32-bit character, process single character input.
// A nearly-branchless UTF-8 decoder, based on work of Christopher Wellons (https://github.com/skeeto/branchless-utf8).
// We handle UTF-8 decoding error by skipping forward.
int utf8_decode_v2(const char* in_text, const char* in_text_end, uint32_t *out_char, int *e)
{
    static const char lengths[32] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 4, 0 };
    static const int masks[]  = { 0x00, 0x7f, 0x1f, 0x0f, 0x07 };
    static const uint32_t mins[] = { 0x400000, 0, 0x80, 0x800, 0x10000 };
    static const int shiftc[] = { 0, 18, 12, 6, 0 };
    static const int shifte[] = { 0, 6, 4, 2, 0 };
    int len = lengths[*(const unsigned char*)in_text >> 3];
    int wanted = len + !len;

    if (in_text_end == NULL)
        in_text_end = in_text + wanted; // Max length, nulls will be taken into account.

    // Copy at most 'len' bytes, stop copying at 0 or past in_text_end. Branch predictor does a good job here,
    // so it is fast even with excessive branching.
    unsigned char s[4];
    s[0] = in_text + 0 < in_text_end ? in_text[0] : 0;
    s[1] = in_text + 1 < in_text_end ? in_text[1] : 0;
    s[2] = in_text + 2 < in_text_end ? in_text[2] : 0;
    s[3] = in_text + 3 < in_text_end ? in_text[3] : 0;

    // Assume a four-byte character and load four bytes. Unused bits are shifted out.
    *out_char  = (uint32_t)(s[0] & masks[len]) << 18;
    *out_char |= (uint32_t)(s[1] & 0x3f) << 12;
    *out_char |= (uint32_t)(s[2] & 0x3f) <<  6;
    *out_char |= (uint32_t)(s[3] & 0x3f) <<  0;
    *out_char >>= shiftc[len];

    // Accumulate the various error conditions.
    *e  = (*out_char < mins[len]) << 6; // non-canonical encoding
    *e |= ((*out_char >> 11) == 0x1b) << 7;  // surrogate half?
    *e |= (*out_char > 0x10FFFF) << 8;  // out of range?
    *e |= (s[1] & 0xc0) >> 2;
    *e |= (s[2] & 0xc0) >> 4;
    *e |= (s[3]       ) >> 6;
    *e ^= 0x2a; // top two bits of each tail byte correct?
    *e >>= shifte[len];

    if (*e)
    {
        // No bytes are consumed when *in_text == 0 || in_text == in_text_end.
        // One byte is consumed in case of invalid first byte of in_text.
        // All available bytes (at most `len` bytes) are consumed on incomplete/invalid second to last bytes.
        // Invalid or incomplete input may consume less bytes than wanted, therefore every byte has to be inspected in s.
        wanted = utf8_min(wanted, !!s[0] + !!s[1] + !!s[2] + !!s[3]);
        *out_char = 0xFFFD;
    }

    return wanted;
}

char *utf8_encode(char *buf, char *buf_end, uint32_t c)
{
	if (c < 0x80) {
		if (buf + 1 > buf_end)
			return NULL;

		buf[0] = (char)(c);
		return buf + 1;
	} else if (c < 0x800) {
		if (buf + 2 > buf_end)
			return NULL;

		buf[0] = (char)(0xC0 + (c >> 6));
		buf[1] = (char)(0x80 + (c & 0x3F));
		return buf + 2;
	} else if (c < 0x10000) {
		if (buf + 3 > buf_end)
			return NULL;

		buf[0] = (char)(0xE0 + (c >> 12));
		buf[1] = (char)(0x80 + ((c >> 6) & 0x3F));
		buf[2] = (char)(0x80 + (c & 0x3F));
		return buf + 3;
	} else if (c < 0x110000) {
		if (buf + 4 > buf_end)
			return NULL;

		buf[0] = (char)(0xF0 + (c >> 18));
		buf[1] = (char)(0x80 + ((c >> 12) & 0x3F));
		buf[2] = (char)(0x80 + ((c >> 6) & 0x3F));
		buf[3] = (char)(0x80 + (c & 0x3F));
		return buf + 4;
	}

	if (buf + 2 < buf_end) {
		buf[0] = (char)(0xC0 + (0xFFFD >> 6));
		buf[1] = (char)(0x80 + (0xFFFD & 0x3F));
	}

	return NULL;
}

size_t utf8_sizeofstr(const char *s)
{
	size_t len = strlen(s);
	return len + 4;
}
