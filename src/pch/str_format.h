#ifndef PCH_STR_FORMAT_H
#define PCH_STR_FORMAT_H

inline void _StringFormat(std::stringstream &s)
{
	(void)s;
}

template< typename _Type, typename... _Args >
inline void _StringFormat(std::stringstream &s, const _Type &arg, const _Args &... args)
{
	s << arg;
	_StringFormat(s, args...);
}

template< typename... _Args >
inline std::string StringFormat(const _Args &... args)
{
	std::stringstream stream;
	_StringFormat(stream, args...);
	return stream.str();
}

// example: StringFormat("/sounds/", somestr, ".wav")

#endif
