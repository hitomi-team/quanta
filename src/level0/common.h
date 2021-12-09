#ifndef LEVEL0_COMMON_H
#define LEVEL0_COMMON_H

template< typename T, typename U = T >
static constexpr T IO_AlignUp(T what, U to)
{
	return (what + to - 1) & ~(to - 1);
}

template< typename T, typename U = T >
static constexpr T IO_AlignDown(T what, U to)
{
	return (what / to) * to;
}

template< typename T >
static constexpr void IO_EraseCPPVector(T &vec)
{
	T().swap(vec);
}

const char *Com_ParseStr(char *token, size_t tokensize, const char *data);

#endif
