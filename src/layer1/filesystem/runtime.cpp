#include "runtime.h"

namespace Filesystem {

	Runtime::Runtime()
		: Service("Filesystem")
		, m_chunk_paks{}
		, m_entries{}
		, m_chunk_begin_pos{}
	{
	}

	void Runtime::Release()
	{
		m_chunk_paks.clear();
		m_entries.clear();
	}

	bool Runtime::Setup()
	{
		std::ifstream file;
		PakarIndexHeader header;
		PakarChunkHeader chunk_header;

		char stream_buf[512];
		MeasureStream measure;
		ReadStream stream;

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
		}

		return true;
	}

}
