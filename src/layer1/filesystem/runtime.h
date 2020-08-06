#ifndef FILESYSTEM_FS_H
#define FILESYSTEM_FS_H

#include "../log.h"
#include "../graph/service.h"

#include "pakar_struct.h"

#include <cstdint>
#include <vector>
#include <string>
#include <fstream>

#include <mutex>

namespace Filesystem {

	enum {
		SEEKDIR_BEG,
		SEEKDIR_END,
		SEEKDIR_CUR
	};

	class Runtime : public Graph::Service {
	public:
		Runtime();
		inline ~Runtime() {}

		bool Setup();
		void Release();

		int FindFile(const std::string &path);

		std::size_t ReadFile(int handle, char *block, std::size_t n);

		bool SeekFile(int handle, uint64_t offset, int seek_dir);
		std::uint64_t TellFile(int handle);

		std::uint64_t GetFileSize(int handle);

	private:
		std::vector< std::ifstream > m_chunk_paks;
		std::vector< PakarIndexEntry > m_entries;
		std::vector< std::uint64_t > m_offsets;
		std::size_t m_chunk_begin_pos;

		std::mutex m_mtx;
		std::mutex m_seekmtx;

	};

}

#endif
