#ifndef __serialize_h____
#define __serialize_h____

#include <stddef.h>
#include <stdint.h>

struct serialize_stream {
	size_t offset;
	size_t size;
	uint8_t *data;
};

enum {
	SERIALIZE_STREAM_SEEK_BEGIN,
	SERIALIZE_STREAM_SEEK_END,
	SERIALIZE_STREAM_SEEK_CURRENT
};

void serialize_stream_init(struct serialize_stream *stream, void *ptr, size_t size);
void serialize_stream_reset(struct serialize_stream *stream);

void serialize_stream_seek(struct serialize_stream *stream, size_t offset, int whence);

void serialize_stream_read_u8(struct serialize_stream *stream, uint8_t *p);
void serialize_stream_read_u32(struct serialize_stream *stream, uint32_t *p);
void serialize_stream_read_u64(struct serialize_stream *stream, uint64_t *p);
void serialize_stream_read_data(struct serialize_stream *stream, void *data, size_t size);

void serialize_stream_write_u8(struct serialize_stream *stream, const uint8_t *p);
void serialize_stream_write_u32(struct serialize_stream *stream, const uint32_t *p);
void serialize_stream_write_u64(struct serialize_stream *stream, const uint64_t *p);
void serialize_stream_write_data(struct serialize_stream *stream, const void *data, size_t size);

#endif
