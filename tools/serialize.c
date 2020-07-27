#include "serialize.h"
#include <stdlib.h>

#define READ_STREAM(type, data, n) ((type)(data[n]) << (8*n))
#define WRITE_STREAM(val, n)       ((val >> (8*n)) & 0xFF)

#if defined(__hppa__) || \
	defined(__m68k__) || defined(mc68000) || defined(_M_M68K) || \
	(defined(__MIPS__) && defined(__MIPSEB__)) || \
	defined(__ppc__) || defined(__POWERPC__) || defined(_M_PPC) || \
	defined(__sparc__)

#define swap_be32(x) (x)
#define swap_be64(x) (x)

#else

static inline uint32_t swap_be32(uint32_t val)
{
	val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
	return (val << 16) | (val >> 16);
}

static inline uint64_t swap_be64(uint64_t val)
{
	val = ((val << 8) & 0xFF00FF00FF00FF00ULL) | ((val >> 8) & 0x00FF00FF00FF00FFULL);
	val = ((val << 16) & 0xFFFF0000FFFF0000ULL) | ((val >> 16) & 0x0000FFFF0000FFFFULL);
	return (val << 32) | (val >> 32);
}

#endif

void serialize_stream_init(struct serialize_stream *stream, void *ptr, uint64_t size)
{
	stream->offset = 0;
	stream->size = size;
	stream->data = (uint8_t *)ptr;
}

void serialize_stream_reset(struct serialize_stream *stream)
{
	stream->offset = 0;
	stream->size = 0;
}

void serialize_stream_seek(struct serialize_stream *stream, uint64_t offset, int whence)
{
	if (whence == SERIALIZE_STREAM_SEEK_BEGIN)
		stream->offset = offset;

	if (whence == SERIALIZE_STREAM_SEEK_END)
		stream->offset = stream->size - offset;

	if (whence == SERIALIZE_STREAM_SEEK_CURRENT)
		stream->offset += offset;
}

void serialize_stream_read_u8(struct serialize_stream *stream, uint8_t *p)
{
	uint8_t *data = stream->data + stream->offset;

	(*p) = data[0];

	stream->offset += sizeof(uint8_t);
}

void serialize_stream_read_u32(struct serialize_stream *stream, uint32_t *p)
{
	uint8_t *data = stream->data + stream->offset;

	(*p)  = READ_STREAM(uint32_t, data, 0);
	(*p) |= READ_STREAM(uint32_t, data, 1);
	(*p) |= READ_STREAM(uint32_t, data, 2);
	(*p) |= READ_STREAM(uint32_t, data, 3);
	(*p) = swap_be32((*p));

	stream->offset += sizeof(uint32_t);
}

void serialize_stream_read_u64(struct serialize_stream *stream, uint64_t *p)
{
	uint8_t *data = stream->data + stream->offset;

	(*p)  = READ_STREAM(uint64_t, data, 0);
	(*p) |= READ_STREAM(uint64_t, data, 1);
	(*p) |= READ_STREAM(uint64_t, data, 2);
	(*p) |= READ_STREAM(uint64_t, data, 3);
	(*p) |= READ_STREAM(uint64_t, data, 4);
	(*p) |= READ_STREAM(uint64_t, data, 5);
	(*p) |= READ_STREAM(uint64_t, data, 6);
	(*p) |= READ_STREAM(uint64_t, data, 7);
	(*p) = swap_be64((*p));

	stream->offset += sizeof(uint64_t);
}

void serialize_stream_read_data(struct serialize_stream *stream, void *data, size_t size)
{
	uint8_t *c = data;

	while (size--)
		*c++ = stream->data[stream->offset++];
}

void serialize_stream_write_u8(struct serialize_stream *stream, const uint8_t *p)
{
	uint8_t *data = stream->data + stream->offset;
	uint8_t val = (*p);

	data[0] = val;

	stream->offset += sizeof(uint8_t);
}

void serialize_stream_write_u32(struct serialize_stream *stream, const uint32_t *p)
{
	uint8_t *data = stream->data + stream->offset;
	uint32_t val = (*p);

	val = swap_be32(val);
	data[0] = WRITE_STREAM(val, 0);
	data[1] = WRITE_STREAM(val, 1);
	data[2] = WRITE_STREAM(val, 2);
	data[3] = val >> 24;

	stream->offset += sizeof(uint32_t);
}

void serialize_stream_write_u64(struct serialize_stream *stream, const uint64_t *p)
{
	uint8_t *data = stream->data + stream->offset;
	uint64_t val = (*p);

	val = swap_be64(val);
	data[0] = WRITE_STREAM(val, 0);
	data[1] = WRITE_STREAM(val, 1);
	data[2] = WRITE_STREAM(val, 2);
	data[3] = WRITE_STREAM(val, 3);
	data[4] = WRITE_STREAM(val, 4);
	data[5] = WRITE_STREAM(val, 5);
	data[6] = WRITE_STREAM(val, 6);
	data[7] = val >> 56;

	stream->offset += sizeof(uint64_t);
}

void serialize_stream_write_data(struct serialize_stream *stream, const void *data, size_t size)
{
	const uint8_t *c = data;

	while (size--)
		stream->data[stream->offset++] = *c++;
}
