#ifndef __pakar_h____
#define __pakar_h____

#include <stdint.h>

/* this stuff is always stored in big endian */

struct pakar_chunk_header {
	/* file header: ?!** */
	uint32_t begin;

	/* version */
	uint32_t version;
};

struct pakar_index_header {
	/* file header: ?!*| */
	uint32_t begin;

	/* version */
	uint32_t version;

	/* number of files in every chunk */
	uint32_t num_files;

	/* number of chunks: data_0.pak data_1.pak ... etc */
	uint32_t num_chunks;

	/* total size of the pak(s) */
	uint64_t size;

	/* sha256 checksum of the index */
	uint8_t sha256[32];
};

struct pakar_index_entry {
	/* CRC-32 of the file */
	uint32_t crc32;

	/* modified time in milliseconds in UNIX time */
	uint64_t modified_time;

	/* file size */
	uint64_t size;

	/* offset to a chunk */
	uint64_t offset;

	/* file type */
	uint8_t type;

	/* path */
	char path[260];
};

enum {
	PAKAR_INDEX_ENTRY_TYPE_DIR,
	PAKAR_INDEX_ENTRY_TYPE_REG
};

#define PAKAR_VERSION 0x948BE910

#ifdef __serialize_h____

static inline void serialize_stream_read_pakar_index_header(struct serialize_stream *stream, struct pakar_index_header *header)
{
	serialize_stream_read_u32(stream, &header->begin);
	serialize_stream_read_u32(stream, &header->version);
	serialize_stream_read_u32(stream, &header->num_files);
	serialize_stream_read_u32(stream, &header->num_chunks);
	serialize_stream_read_u64(stream, &header->size);
	serialize_stream_read_data(stream, header->sha256, sizeof(header->sha256));
}

static inline void serialize_stream_write_pakar_index_header(struct serialize_stream *stream, struct pakar_index_header *header)
{
	serialize_stream_write_u32(stream, &header->begin);
	serialize_stream_write_u32(stream, &header->version);
	serialize_stream_write_u32(stream, &header->num_files);
	serialize_stream_write_u32(stream, &header->num_chunks);
	serialize_stream_write_u64(stream, &header->size);
	serialize_stream_write_data(stream, header->sha256, sizeof(header->sha256));
}

static inline void serialize_stream_read_pakar_index_entry(struct serialize_stream *stream, struct pakar_index_entry *entry)
{
	serialize_stream_read_u32(stream, &entry->crc32);
	serialize_stream_read_u64(stream, &entry->modified_time);
	serialize_stream_read_u64(stream, &entry->size);
	serialize_stream_read_u64(stream, &entry->offset);
	serialize_stream_read_u8(stream, &entry->type);
	serialize_stream_read_data(stream, entry->path, sizeof(entry->path));
}

static inline void serialize_stream_write_pakar_index_entry(struct serialize_stream *stream, struct pakar_index_entry *entry)
{
	serialize_stream_write_u32(stream, &entry->crc32);
	serialize_stream_write_u64(stream, &entry->modified_time);
	serialize_stream_write_u64(stream, &entry->size);
	serialize_stream_write_u64(stream, &entry->offset);
	serialize_stream_write_u8(stream, &entry->type);
	serialize_stream_write_data(stream, entry->path, sizeof(entry->path));
}

#endif

#endif
