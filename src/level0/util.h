#ifndef LEVEL0_UTIL_H
#define LEVEL0_UTIL_H

static constexpr uint64_t UTIL_FNV1A64_VALUE = 0xCBF29CE484222325;
static constexpr uint64_t UTIL_FNV1A64_PRIME = 0x100000001B3;

static inline constexpr size_t UtilConstStrlen(const char *str) noexcept
{
	size_t n = 0;
	while (*str++) n++;
	return n;
}

static inline constexpr uint64_t UtilFnv1a64(const char *str, size_t len, const uint64_t value = UTIL_FNV1A64_VALUE) noexcept
{
	return len == 0 ? value : UtilFnv1a64(str + 1, len - 1, (value ^ uint64_t(str[0]))*UTIL_FNV1A64_PRIME);
}

struct UtilStringHash {
	uint64_t hash;

	// Always compile time
	inline constexpr UtilStringHash(uint64_t _hash) noexcept : hash(_hash) {}
	inline constexpr UtilStringHash(const char *str) noexcept : hash(0) { this->hash = UtilFnv1a64(str, UtilConstStrlen(str)); }
	inline constexpr UtilStringHash(const char *str, size_t len) noexcept : hash(0) { this->hash = UtilFnv1a64(str, len); }

	template< size_t N >
	inline constexpr UtilStringHash(char (&str)[N]) noexcept : hash(0) { this->hash = UtilFnv1a64(str, N - 1); }

	template< size_t N >
	inline constexpr UtilStringHash(const char (&str)[N]) noexcept : hash(0) { this->hash = UtilFnv1a64(str, N - 1); }

	// Non compile time
	inline UtilStringHash(const std::string_view &str) noexcept : hash(0) { this->hash = UtilFnv1a64(str.data(), str.size()); }
	inline UtilStringHash(const std::string &str) noexcept : hash(0) { this->hash = UtilFnv1a64(str.data(), str.size()); }

	UtilStringHash(const UtilStringHash &other) = default;

	inline constexpr operator auto() noexcept { return this->hash; }
};

#endif
