#include "runtime.h"

// std::memcpy
#include <cstring>

namespace Filesystem {

	Runtime::Runtime()
		: Service("FilesystemService")
		, m_chunk_paks{}
		, m_entries{}
		, m_offsets{}
		, m_chunk_begin_pos{}
		, m_mtx{}
		, m_seekmtx{}
	{
	}

	void Runtime::Release()
	{
		std::unique_lock< std::mutex > lock(m_mtx), seek_lock(m_seekmtx);

		m_chunk_paks.clear();
		m_entries.clear();
		m_offsets.clear();
	}

	bool Runtime::Setup()
	{
		std::ifstream file;
		PakarIndexHeader header;
		PakarChunkHeader chunk_header;

		char stream_buf[512];
		MeasureStream measure;
		ReadStream stream;

		if (this->Initialized)
			return true;

		stream.data = reinterpret_cast< std::uint8_t * >(stream_buf);
		file.open("data_index.pak", std::ios::binary);
		if (!file.is_open()) {
			global_log.Error("Failed to open pak file:" + std::string("data_index.pak"));
			return false;
		}

		measure.ResetPosition();
		header.Deserialize(measure);

		file.read(stream_buf, measure.pos);
		if (static_cast< std::size_t >(file.gcount()) != measure.pos) {
			global_log.Error("Failed to read pak file: " + std::string("data_index.pak"));
			return false;
		}

		stream.ResetPosition();
		header.Deserialize(stream);

		if (header.begin != PAKAR_INDEX_HEADER_BEGIN || header.version != PAKAR_VERSION) {
			global_log.Error("Invalid pak file: " + std::string("data_index.pak"));
			return false;
		}

		measure.ResetPosition();
		chunk_header.Deserialize(measure);
		m_chunk_begin_pos = measure.pos;

		// FIXME: implement multi-chunk
		m_chunk_paks.resize(1);

		for (auto &chunk_pak : m_chunk_paks) {
			chunk_pak.open("data_000.pak", std::ios::binary);
			if (!chunk_pak.is_open()) {
				global_log.Error("Failed to open pak file: " + std::string("data_000.pak"));
				m_chunk_paks.clear();
				return false;
			}

			chunk_pak.read(stream_buf, measure.pos);

			stream.ResetPosition();
			chunk_header.Deserialize(stream);
			if (chunk_header.begin != PAKAR_CHUNK_HEADER_BEGIN) {
				global_log.Error("Invalid pak file: " + std::string("data_000.pak"));
				m_chunk_paks.clear();
				return false;
			}
		}

		m_entries.resize(header.num_files);

		for (auto &entry : m_entries) {
			measure.ResetPosition();
			entry.Deserialize(measure);
			file.read(stream_buf, measure.pos);

			stream.ResetPosition();
			entry.Deserialize(stream);

			m_offsets.push_back(0);
		}

		this->Initialized = true;

		return true;
	}

	int Runtime::FindFile(const std::string &path)
	{
		int i = 0;

		for (auto &entry : m_entries) {
			std::string path_in_index = entry.path;
			if (path == path_in_index) {
				if (entry.type == 0)
					continue;

				return i;
			}

			i++;
		}

		return -1;
	}

	std::size_t Runtime::ReadFile(int handle, char *block, std::size_t n)
	{
		char buf[4096];
		std::size_t n_read, to_read, total_read = 0;
		std::uint64_t actual_size;

		if (handle == -1)
			return 0;

		if (m_entries[handle].type == 0)
			return 0;

		std::unique_lock< std::mutex > lock(m_mtx), seek_lock(m_seekmtx);

		// TODO: implement multi-chunk
		actual_size = m_entries[handle].size;

		if (m_offsets[handle] >= actual_size)
			return 0;

		while (n != 0 && actual_size != 0) {
			if (sizeof(buf) > actual_size || sizeof(buf) < n)
				to_read = static_cast< size_t >(actual_size);
			else if (sizeof(buf) > n)
				to_read = n;
			else
				to_read = sizeof(buf);

			m_chunk_paks[0].seekg(m_entries[handle].offset + m_offsets[handle], std::ios_base::beg);
			m_chunk_paks[0].read(buf, to_read);

			n_read = m_chunk_paks[0].gcount();

			std::memcpy(block + total_read, buf, n_read);

			n -= n_read;
			actual_size -= n_read;
			total_read += n_read;

			m_offsets[handle] += n_read;
		}

		return total_read;
	}

	bool Runtime::SeekFile(int handle, uint64_t offset, int seek_dir)
	{
		if (handle == -1)
			return false;

		if (m_entries[handle].type == 0)
			return false;

		std::unique_lock< std::mutex > seek_lock(m_seekmtx);

		switch (seek_dir) {
		case SEEKDIR_BEG:
			m_offsets[handle] = std::max(offset, m_entries[handle].size);
			break;
		case SEEKDIR_END:
			m_offsets[handle] = std::max(m_entries[handle].size + offset, m_entries[handle].size);
			break;
		case SEEKDIR_CUR:
			m_offsets[handle] = std::max(m_offsets[handle] + offset, m_entries[handle].size);
			break;
		}

		return true;
	}

	std::uint64_t Runtime::TellFile(int handle)
	{
		if (handle == -1)
			return UINT64_MAX;

		if (m_entries[handle].type == 0)
			return UINT64_MAX;

		std::unique_lock< std::mutex > seek_lock(m_seekmtx);

		return m_offsets[handle];
	}

	std::uint64_t Runtime::GetFileSize(int handle)
	{
		if (handle == -1)
			return UINT64_MAX;

		if (m_entries[handle].type == 0)
			return UINT64_MAX;

		return m_entries[handle].size;
	}

}
