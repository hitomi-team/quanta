#ifndef FILESYSTEM_PAKAR_STRUCT_H
#define FILESYSTEM_PAKAR_STRUCT_H

#include "stream.h"

// These must match
#define PAKAR_VERSION 0x948BE910

// ?!**
#define PAKAR_CHUNK_HEADER_BEGIN 0x3F212A2A

// ?!*|
#define PAKAR_INDEX_HEADER_BEGIN 0x3F212A7C

namespace Filesystem {

	struct PakarChunkHeader {
		std::uint32_t begin;
		std::uint32_t version;

		inline void Deserialize(Stream &stream)
		{
			stream.U32(this->begin);
			stream.U32(this->version);
		}
	};

	struct PakarIndexHeader {
		std::uint32_t begin;
		std::uint32_t version;
		std::uint32_t num_files;
		std::uint32_t num_chunks;
		std::uint64_t size;
		std::uint8_t sha256[32];

		inline void Deserialize(Stream &stream)
		{
			stream.U32(this->begin);
			stream.U32(this->version);
			stream.U32(this->num_files);
			stream.U32(this->num_chunks);
			stream.U64(this->size);
			stream.Data(this->sha256, sizeof(this->sha256));
		}
	};

	struct PakarIndexEntry {
		std::uint32_t crc32;
		std::uint64_t mtime;
		std::uint64_t size;
		std::uint64_t offset;
		std::uint8_t type;
		char path[260];

		inline void Deserialize(Stream &stream)
		{
			stream.U32(this->crc32);
			stream.U64(this->mtime);
			stream.U64(this->size);
			stream.U64(this->offset);
			stream.U8(this->type);
			stream.Data(reinterpret_cast< std::uint8_t * >(this->path), sizeof(this->path));
		}
	};

}

#endif
