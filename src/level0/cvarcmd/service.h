#ifndef LEVEL0_CVARCMD_SERVICE_H
#define LEVEL0_CVARCMD_SERVICE_H

#include "level0/game/game_service.h"

#define CVAR_STORAGE_MAX_VALUES 512

class CVarCmdService;

enum class CVarType : uint32_t {
	Float = 0,
	Int,
	String,
	MaxTypes
};

enum class CVarFlags : uint32_t {
	None = 0,
	NoEdit = 1 << 1,
	EditReadOnly = 1 << 2,
	Advanced = 1 << 3,

	EditCheckBox = 1 << 8,
	EditFloatDrag = 1 << 9,
};

template< typename T >
struct CVarTypeStruct {
};

template<>
struct CVarTypeStruct< double > {
	static constexpr uint32_t Value = static_cast< uint32_t >(CVarType::Float);
};

template<>
struct CVarTypeStruct< int > {
	static constexpr uint32_t Value = static_cast< uint32_t >(CVarType::Int);
};

template<>
struct CVarTypeStruct< std::string > {
	static constexpr uint32_t Value = static_cast< uint32_t >(CVarType::String);
};

struct CVarParameter {
	std::shared_mutex mutex;
	std::string name;
	std::string description;
	uint64_t hash;
	uint32_t type;
	size_t arrayIndex;
	CVarCmdService *service;
};

template< typename T >
struct CVarStorage {
	T init, max, min, value;
	CVarParameter *parameter;
};

template<>
struct CVarStorage< std::string > {
	std::string init, value;
	CVarParameter *parameter;
};

template< typename T >
struct CVarCallbackType {
	using Value = std::function< int(CVarCmdService *, CVarStorage< T > *cvar) >;
};

template< typename T >
struct CVarArray {
	std::array< CVarStorage< T >, CVAR_STORAGE_MAX_VALUES > cvars {};
	std::array< typename CVarCallbackType< T >::Value, CVAR_STORAGE_MAX_VALUES > callbacks {};
	std::array< bool, CVAR_STORAGE_MAX_VALUES > callbacksEnabled {};
	std::unordered_map< uint64_t, size_t > hashToIndexMap {};
	size_t last = 0;

	T GetValue(size_t index);
	void SetValue(size_t index, const T &value);
	void ResetValue(size_t index);
	size_t Add(CVarParameter *parameter, const T &init, const T &min, const T &max, const T &value);
	void SetCallback(uint64_t index, typename CVarCallbackType< T >::Value callback);
};

template<>
struct CVarArray< std::string > {
	std::array< CVarStorage< std::string >, CVAR_STORAGE_MAX_VALUES > cvars {};
	std::array< typename CVarCallbackType< std::string >::Value, CVAR_STORAGE_MAX_VALUES > callbacks {};
	std::array< bool, CVAR_STORAGE_MAX_VALUES > callbacksEnabled {};
	std::unordered_map< uint64_t, size_t > hashToIndexMap {};
	size_t last = 0;

	std::string GetValue(size_t index);
	void SetValue(size_t index, std::string_view value);
	void ResetValue(size_t index);
	size_t Add(CVarParameter *parameter, std::string_view init, std::string_view value);
	void SetCallback(size_t index, typename CVarCallbackType< std::string >::Value callback);
};

using CmdFunction = std::function< int(CVarCmdService *, std::string_view args, const std::vector< std::string > &argv) >;

struct CVarCmd {
	std::string name;
	std::string description;
	CmdFunction func;
	uint64_t hash;
};

class CVarCmdService : public GameService {
public:
	UTIL_DEFINE_SERVICE_HASH(CVarCmdService);

	CVarCmdService();
	~CVarCmdService();

	inline void Update() {}

	template< typename T >
	CVarArray< T > *GetCVarArray();

	void AddAlias(std::string_view name, std::string_view aliasTo);
	void AddCmd(std::string_view name, std::string_view description, CmdFunction func);

	template< typename T >
	void CreateCVar(std::string_view name, std::string_view description, const T &initValue, const T &minValue, const T &maxValue, const T &value);

	template< typename T >
	void CreateCVar(std::string_view name, std::string_view description, const T &initValue, const T &minValue, const T &maxValue, const T &value, typename CVarCallbackType< T >::Value callback);

	void CreateCVar(std::string_view name, std::string_view description, std::string_view initValue, std::string_view value);
	void CreateCVar(std::string_view name, std::string_view description, std::string_view initValue, std::string_view value, typename CVarCallbackType< std::string >::Value callback);

	template< typename T >
	void SetCVarCallback(uint64_t hash, typename CVarCallbackType< T >::Value callback);

	template< typename T >
	T GetCVarValue(uint64_t hash);

	void Exec(std::string_view cmd);
private:
	CVarParameter *CreateCVar(std::string_view name, std::string_view description);
	bool ParseStr(std::string_view str);
	bool ExecCVarCmd();

	template< typename T >
	bool ExecCVarCmdType(CVarParameter *parameter);
private:
	std::shared_mutex m_mutex;
	std::string m_args;
	uint64_t m_argsHash;
	std::vector< std::string > m_argv;
	std::vector< uint64_t > m_argvHash;
	std::vector< std::string > m_aliases;
	std::unordered_map< uint64_t, std::string > m_aliasMap;
	std::unordered_map< uint64_t, CVarCmd > m_cmds;
	std::unordered_map< uint64_t, std::unique_ptr< CVarParameter > > m_cvarMap;
	CVarArray< double > m_cvarFloats;
	CVarArray< int > m_cvarInts;
	CVarArray< std::string > m_cvarStrings;
};

#endif
