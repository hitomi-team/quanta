#ifndef FILESYSTEM_FS_H
#define FILESYSTEM_FS_H

#include "../log.h"
#include "../graph/service.h"

#include "pakar_struct.h"

#include <cstdio>
#include <vector>
#include <string>
#include <fstream>

namespace Filesystem {

	struct FSRequest {
		int handle;
		char *block;
		std::size_t n;
	};

	class Runtime : public Graph::Service {
	public:
		Runtime();
		inline ~Runtime() {}

		bool Setup();
		void Release();

		// TODO: return a descriptor
		int FindFileInPak(const std::string &path);

		std::size_t ReadFileInPak(int handle, char *block, std::size_t n);

	private:
		std::vector< std::ifstream > m_chunk_paks;
		std::vector< PakarIndexEntry > m_entries;
		std::size_t m_chunk_begin_pos;
	};

}

#endif
