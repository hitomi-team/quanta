#ifndef FILESYSTEM_STREAM_H
#define FILESYSTEM_STREAM_H

// Stream class for only Filesystem.
// The stream class is intended for use to validate and grab info from PAK's.
// Reading blobs are handled somewhere else

#define READ_STREAM(type, data, n) (static_cast< type >(data[n]) << (8*n))

#if defined(__hppa__) || \
	defined(__m68k__) || defined(mc68000) || defined(_M_M68K) || \
	(defined(__MIPS__) && defined(__MIPSEB__)) || \
	defined(__ppc__) || defined(__POWERPC__) || defined(_M_PPC) || \
	defined(__sparc__)

#define __fs_swap_be_u16(x) (x)
#define __fs_swap_be_u32(x) (x)
#define __fs_swap_be_u64(x) (x)

#else

static inline std::uint16_t __fs_swap_be_u16(std::uint16_t val)
{
	return (val << 8) | (val >> 8);
}

static inline std::uint32_t __fs_swap_be_u32(std::uint32_t val)
{
	val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
	return (val << 16) | (val >> 16);
}

static inline std::uint64_t __fs_swap_be_u64(std::uint64_t val)
{
	val = ((val << 8) & 0xFF00FF00FF00FF00ULL) | ((val >> 8) & 0x00FF00FF00FF00FFULL);
	val = ((val << 16) & 0xFFFF0000FFFF0000ULL) | ((val >> 16) & 0x0000FFFF0000FFFFULL);
	return (val << 32) | (val >> 32);
}

#endif

namespace Filesystem {

	struct Stream {
		std::size_t pos;

		inline virtual ~Stream() {}
		inline virtual void ResetPosition() { this->pos = 0; }

		virtual void U8(std::uint8_t &val) = 0;
		virtual void U16(std::uint16_t &val) = 0;
		virtual void U32(std::uint32_t &val) = 0;
		virtual void U64(std::uint64_t &val) = 0;
		virtual void Data(std::uint8_t *p, std::size_t size) = 0;
	};

	struct MeasureStream : public Stream {
		inline MeasureStream()
		{
			this->pos = 0;
		}

		inline void U8(uint8_t &val)
		{
			(void)val;
			this->pos++;
		}

		inline void U16(std::uint16_t &val)
		{
			(void)val;
			this->pos += 2;
		}

		inline void U32(std::uint32_t &val)
		{
			(void)val;
			this->pos += 4;
		}

		inline void U64(std::uint64_t &val)
		{
			(void)val;
			this->pos += 8;
		}

		inline void Data(std::uint8_t *p, std::size_t size)
		{
			(void)p;
			this->pos += size;
		}
	};

	// Read the file info into data, then deserialize.
	struct ReadStream : public Stream {
		std::size_t size;
		std::uint8_t *data;

		inline ReadStream()
			: size{ 0 }
			, data{ 0 }
		{
			this->pos = 0;
		}

		inline void ResetPosition()
		{
			this->pos = 0;
		}

		inline void U8(uint8_t &val)
		{
			std::uint8_t *p = this->data + this->pos++;
			val = p[0];
		}

		inline void U16(std::uint16_t &val)
		{
			std::uint8_t *p = this->data + this->pos;

			val  = READ_STREAM(std::uint16_t, p, 0);
			val |= READ_STREAM(std::uint16_t, p, 1);
			val = __fs_swap_be_u16(val);

			this->pos += 2;
		}

		inline void U32(std::uint32_t &val)
		{
			std::uint8_t *p = this->data + this->pos;

			val  = READ_STREAM(std::uint32_t, p, 0);
			val |= READ_STREAM(std::uint32_t, p, 1);
			val |= READ_STREAM(std::uint32_t, p, 2);
			val |= READ_STREAM(std::uint32_t, p, 3);
			val = __fs_swap_be_u32(val);

			this->pos += 4;
		}

		inline void U64(std::uint64_t &val)
		{
			std::uint8_t *p = this->data + this->pos;

			val  = READ_STREAM(std::uint64_t, p, 0);
			val |= READ_STREAM(std::uint64_t, p, 1);
			val |= READ_STREAM(std::uint64_t, p, 2);
			val |= READ_STREAM(std::uint64_t, p, 3);
			val |= READ_STREAM(std::uint64_t, p, 4);
			val |= READ_STREAM(std::uint64_t, p, 5);
			val |= READ_STREAM(std::uint64_t, p, 6);
			val |= READ_STREAM(std::uint64_t, p, 7);
			val = __fs_swap_be_u64(val);

			this->pos += 8;
		}

		inline void Data(std::uint8_t *p, std::size_t size)
		{
			while (size--)
				(*p++) = this->data[this->pos++];
		}
	};

}

#endif
