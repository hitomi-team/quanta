#ifndef UTF8_H
#define UTF8_H

#ifdef __cplusplus
extern "C" {
#endif

int utf8_decode_v2(const char *in_text, const char *in_text_end, uint32_t *out_char, int *e);
char *utf8_encode(char *buf, char *buf_end, uint32_t c);

#ifdef __cplusplus
}
#endif

#endif
