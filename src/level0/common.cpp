#include "common.h"
#include "log.h"

// TODO: cleanup
const char *Com_ParseStr(char *token, size_t tokensize, const char *data)
{
	int len = 0, e, c_len;
	uint32_t c;
	const char *data_end = data + strlen(data) + 1;
	char *token_end = token + tokensize;

	token[0] = 0;

	if (data == nullptr || token == nullptr)
		return nullptr;

skip_char:
	for (;;) {
		c_len = utf8_decode_v2(data, data_end, &c, &e);
		if (e != 0)
			return nullptr;
		if (c > ' ')
			break;
		if (c == '\0')
			return nullptr;
		data += c_len;
	}

	// comments
	if (c == '/') {
		c_len = utf8_decode_v2(data + c_len, data_end, &c, &e);
		if (c == '/') {
			for (;;) {
				c_len = utf8_decode_v2(data, data_end, &c, &e);
				data += c_len;
				if (e != 0)
					return nullptr;
				if (c == '\0' || c == '\n')
					goto skip_char;
			}
		} else if (c == '*') {
			for (;;) {
				c_len = utf8_decode_v2(data, data_end, &c, &e);
				data += c_len;
				if (e != 0)
					return nullptr;
				if (c == '\0')
					goto skip_char;
				if (c == '*') {
					c_len = utf8_decode_v2(data, data_end, &c, &e);
					data += c_len;
					if (e != 0)
						return nullptr;
					if (c == '/')
						goto skip_char;
				}
			}
		}
	}

	// quotes
	if (c == '\"') {
		data += c_len;
		for (;;) {
			c_len = utf8_decode_v2(data, data_end, &c, &e);
			data += c_len;
			if (e != 0)
				return nullptr;
			if (c == '\0' || c == '\"') {
				utf8_encode(token + len, token_end, '\0');
				return data;
			}
			if (utf8_encode(token + len, token_end, c) == nullptr)
				goto insert_null_and_exit;
			len += c_len;
		}
	}

	if (c == '{' || c == '}' || c == '(' || c == ')' || c == '\'' || c == ':') {
		if (utf8_encode(token + len, token_end, c) == nullptr)
			goto insert_null_and_exit;
		len += c_len;
		if (utf8_encode(token + len, token_end, '\0') == nullptr)
			goto insert_null_and_exit;
		return data + 1;
	}

	for (;;) {
		if (utf8_encode(token + len, token_end, c) == nullptr)
			goto insert_null_and_exit;
		len += c_len;
		data += c_len;
		c_len = utf8_decode_v2(data, data_end, &c, &e);
		if (utf8_encode(token + len, token_end, c) == nullptr)
			goto insert_null_and_exit;
		if (c == '{' || c == '}' || c == '(' || c == ')' || c == '\'' || c == '\n')
			break;
		if (c <= 32)
			break;
	}

	utf8_encode(token + len, token_end, '\0');
	return data;
// cleanup output
insert_null_and_exit:
	size_t offset = (size_t)(token_end - (token + len));
	memset((token + len) + offset, 0, offset);
	if (token + len == token_end)
		token[len - 1] = 0;
	return data;
}
