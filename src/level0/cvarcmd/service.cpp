#include "level0/pch.h"
#include "level0/log.h"

#include "level0/game/game.h"
#include "service.h"

// CVar utils
template< typename T >
T CVarArray< T >::GetValue(size_t index)
{
	auto &cvar = this->cvars[index];

	std::shared_lock lock(cvar.parameter->mutex);
	return cvar.value;
}

template< typename T >
void CVarArray< T >::SetValue(size_t index, const T &value)
{
	auto &cvar = this->cvars[index];

	std::unique_lock lock(cvar.parameter->mutex);
	cvar.value = std::max(cvar.min, std::min(value, cvar.max));

	if (this->callbacksEnabled[index])
		this->callbacks[index](cvar.parameter->service, &cvar);
}

template< typename T >
void CVarArray< T >::ResetValue(size_t index)
{
	auto &cvar = this->cvars[index];

	std::unique_lock lock(cvar.parameter->mutex);
	cvar.value = cvar.init;

	if (this->callbacksEnabled[index])
		this->callbacks[index](cvar.parameter->service, &cvar);
}

template< typename T >
size_t CVarArray< T >::Add(CVarParameter *parameter, const T &init, const T &min, const T &max, const T &value)
{
	size_t index = this->last++;
	auto &cvar = this->cvars[index];
	cvar.init = init;
	cvar.min = min;
	cvar.max = max;
	cvar.value = value;
	cvar.parameter = parameter;
	parameter->arrayIndex = index;

	this->hashToIndexMap[parameter->hash] = index;
	return index;
}

template< typename T >
void CVarArray< T >::SetCallback(size_t index, typename CVarCallbackType< T >::Value callback)
{
	this->callbacks[index] = callback;
	this->callbacksEnabled[index] = true;
}

template struct CVarArray< double >;
template struct CVarArray< int >;

std::string CVarArray< std::string >::GetValue(size_t index)
{
	auto &cvar = this->cvars[index];

	std::shared_lock lock(cvar.parameter->mutex);
	return cvar.value;
}

void CVarArray< std::string >::SetValue(size_t index, std::string_view value)
{
	auto &cvar = this->cvars[index];

	std::unique_lock lock(cvar.parameter->mutex);
	cvar.value = value;

	if (this->callbacksEnabled[index])
		this->callbacks[index](cvar.parameter->service, &cvar);
}

void CVarArray< std::string >::ResetValue(size_t index)
{
	auto &cvar = this->cvars[index];

	std::unique_lock lock(cvar.parameter->mutex);
	cvar.value = cvar.init;

	if (this->callbacksEnabled[index])
		this->callbacks[index](cvar.parameter->service, &cvar);
}

size_t CVarArray< std::string >::Add(CVarParameter *parameter, std::string_view init, std::string_view value)
{
	size_t index = this->last++;
	auto &cvar = this->cvars[index];

	cvar.init = init;
	cvar.value = value;
	cvar.parameter = parameter;
	parameter->arrayIndex = index;

	this->hashToIndexMap[parameter->hash] = index;
	return index;
}

void CVarArray< std::string >::SetCallback(size_t index, typename CVarCallbackType< std::string >::Value callback)
{
	this->callbacks[index] = callback;
	this->callbacksEnabled[index] = true;
}

// CVarCmdService begin
template<>
CVarArray< double > *CVarCmdService::GetCVarArray()
{
	return &m_cvarFloats;
}

template<>
CVarArray< int > *CVarCmdService::GetCVarArray()
{
	return &m_cvarInts;
}

template<>
CVarArray< std::string > *CVarCmdService::GetCVarArray()
{
	return &m_cvarStrings;
}

CVarCmdService::CVarCmdService() : GameService("CVarCmdService")
{
	this->AddCmd("find", "usage: find [part of cvar/cmd] - Find console variables or commands", [](CVarCmdService *cvarCmdService, std::string_view, const std::vector< std::string > &argv) -> int {
		if (argv.size() < 2) {
			g_Log.Error("find usage: find [part of cvar/cmd]");
			return 1;
		}

		auto printCVarHelp = [](auto array, auto parameter) {
			g_Log.Info(FMT_COMPILE("(cvar) \"{}\" = \"{}\""), parameter->name, array->GetValue(parameter->arrayIndex));
		};

		auto floatArray = cvarCmdService->GetCVarArray< double >();
		auto intArray = cvarCmdService->GetCVarArray< int >();
		auto stringArray = cvarCmdService->GetCVarArray< std::string >();

		for (const auto &i : cvarCmdService->m_cmds) {
			if (i.second.name.find(argv[1]) == 0)
				g_Log.Info(FMT_COMPILE("(cmd) \"{}\""), i.second.name);
		}

		for (const auto &i : cvarCmdService->m_cvarMap) {
			if (i.second->name.find(argv[1]) == 0) {
				switch (i.second->type) {
				case static_cast< uint32_t >(CVarType::Float):
					printCVarHelp(floatArray, i.second.get());
					break;
				case static_cast< uint32_t >(CVarType::Int):
					printCVarHelp(intArray, i.second.get());
					break;
				case static_cast< uint32_t >(CVarType::String):
					printCVarHelp(stringArray, i.second.get());
					break;
				}

			}
		}

		for (const auto &i : cvarCmdService->m_aliases) {
			if (i.find(argv[1]) == 0)
				g_Log.Info(FMT_COMPILE("(alias) \"{}\" -> \"{}\""), i, cvarCmdService->m_aliasMap[UtilStringHash(i)]);
		}

		return 0;
	});

	this->AddCmd("help", "usage: help [cvar/cmd] - Display help for a console variable or command", [](CVarCmdService *cvarCmdService, std::string_view, const std::vector< std::string > &argv) -> int {
		if (argv.size() < 2) {
			g_Log.Error("help usage: help [cvar/cmd]");
			return 1;
		}

		auto printCVarHelp = [](auto array, uint64_t hash) {
			auto &storage = array->cvars[array->hashToIndexMap[hash]];
			g_Log.Info(FMT_COMPILE("\"{}\" - {}"), storage.parameter->name, storage.parameter->description);
		};

		auto floatArray = cvarCmdService->GetCVarArray< double >();
		auto intArray = cvarCmdService->GetCVarArray< int >();
		auto stringArray = cvarCmdService->GetCVarArray< std::string >();

		uint64_t hash = UtilStringHash(argv[1]);

		if (cvarCmdService->m_cmds.count(hash) != 0) {
			g_Log.Info(FMT_COMPILE("\"{}\" - {}"), argv[1], cvarCmdService->m_cmds[hash].description);
			return 0;
		} else if (floatArray->hashToIndexMap.count(hash) != 0) {
			printCVarHelp(floatArray, hash);
			return 0;
		} else if (intArray->hashToIndexMap.count(hash) != 0) {
			printCVarHelp(intArray, hash);
			return 0;
		} else if (stringArray->hashToIndexMap.count(hash) != 0) {
			printCVarHelp(stringArray, hash);
			return 0;
		}

		g_Log.Error(FMT_COMPILE("No console variable or command found: \"{}\""), argv[1]);
		return 1;
	});

	this->AddCmd("alias", "usage: alias [aliasName] [aliasTo] - Add alias", [](CVarCmdService *cvarCmdService, std::string_view, const std::vector< std::string > &argv) -> int {
		if (argv.size() < 3) {
			g_Log.Error("alias usage: alias [aliasName] [aliasTo]");
			return 1;
		}

		cvarCmdService->AddAlias(argv[1], argv[2]);
		return 0;
	});

	this->AddCmd("quit", "Quit the game", [](CVarCmdService *, std::string_view, const std::vector< std::string > &) -> int {
		g_Game->RequestClose();
		return 0;
	});

	this->AddCmd("clear", "Clear console scrollback buffer", [](CVarCmdService *, std::string_view, const std::vector< std::string > &) -> int {
		g_Log.ClearBuffer();
		return 0;
	});

	this->AddAlias("exit", "quit");
}

CVarCmdService::~CVarCmdService()
{
}

void CVarCmdService::AddAlias(std::string_view name, std::string_view aliasTo)
{
	m_aliases.push_back(std::string(name));
	m_aliasMap[UtilStringHash(name)] = aliasTo;
}

void CVarCmdService::AddCmd(std::string_view name, std::string_view description, CmdFunction func)
{
	CVarCmd cmd = { name.data(), description.data(), func, UtilStringHash(name) };
	m_cmds[UtilStringHash(name)] = std::move(cmd);
}

template< typename T >
void CVarCmdService::CreateCVar(std::string_view name, std::string_view description, const T &initValue, const T &minValue, const T &maxValue, const T &value)
{
	auto param = this->CreateCVar(name, description);
	if (param == nullptr)
		g_Game->Abort("CVarCmdService: Failed to create CVar!");

	this->GetCVarArray< T >()->Add(param, initValue, minValue, maxValue, value);
}

template< typename T >
void CVarCmdService::CreateCVar(std::string_view name, std::string_view description, const T &initValue, const T &minValue, const T &maxValue, const T &value, typename CVarCallbackType< T >::Value callback)
{
	auto param = this->CreateCVar(name, description);
	if (param == nullptr)
		g_Game->Abort("CVarCmdService: Failed to create CVar!");

	auto array = this->GetCVarArray< T >();
	array->Add(param, initValue, minValue, maxValue, value);
	array->SetCallback(param->arrayIndex, callback);
	param->type = CVarTypeStruct< T >::Value;
}

template void CVarCmdService::CreateCVar< double >(std::string_view, std::string_view, const double &, const double &, const double &, const double &);
template void CVarCmdService::CreateCVar< double >(std::string_view, std::string_view, const double &, const double &, const double &, const double &, typename CVarCallbackType< double >::Value);
template void CVarCmdService::CreateCVar< int >(std::string_view, std::string_view, const int &, const int &, const int &, const int &);
template void CVarCmdService::CreateCVar< int >(std::string_view, std::string_view, const int &, const int &, const int &, const int &, typename CVarCallbackType< int >::Value);

void CVarCmdService::CreateCVar(std::string_view name, std::string_view description, std::string_view initValue, std::string_view value)
{
	auto param = this->CreateCVar(name, description);
	if (param == nullptr)
		g_Game->Abort("CVarCmdService: Failed to create CVar!");

	this->GetCVarArray< std::string >()->Add(param, initValue, value);
	param->type = CVarTypeStruct< std::string >::Value;
}

void CVarCmdService::CreateCVar(std::string_view name, std::string_view description, std::string_view initValue, std::string_view value, typename CVarCallbackType< std::string >::Value callback)
{
	auto param = this->CreateCVar(name, description);
	if (param == nullptr)
		g_Game->Abort("CVarCmdService: Failed to create CVar!");

	auto array = this->GetCVarArray< std::string >();
	array->Add(param, initValue, value);
	array->SetCallback(param->arrayIndex, callback);
}

template< typename T >
void CVarCmdService::SetCVarCallback(uint64_t hash, typename CVarCallbackType< T >::Value callback)
{
	auto array = this->GetCVarArray< T >();
	array->SetCallback(array->hashToIndexMap[hash], callback);
}

template void CVarCmdService::SetCVarCallback< double >(uint64_t, typename CVarCallbackType< double >::Value);
template void CVarCmdService::SetCVarCallback< int >(uint64_t, typename CVarCallbackType< int >::Value);
template void CVarCmdService::SetCVarCallback< std::string >(uint64_t, typename CVarCallbackType< std::string >::Value);

template< typename T >
T CVarCmdService::GetCVarValue(uint64_t hash)
{
	auto array = GetCVarArray< T >();
	return array->GetValue(array->hashToIndexMap[hash]);
}

template double CVarCmdService::GetCVarValue< double >(uint64_t);
template int CVarCmdService::GetCVarValue< int >(uint64_t);
template std::string CVarCmdService::GetCVarValue< std::string >(uint64_t);

void CVarCmdService::Exec(std::string_view cmd)
{
	m_args = cmd.data();
	m_argsHash = UtilStringHash(m_args);

	if (!this->ParseStr(cmd))
		return;

	if (m_argv.size() == 0)
		return;

	if (m_cmds.count(m_argvHash[0]) != 0) {
		if (m_cmds[m_argvHash[0]].func(this, m_args, m_argv) != 0)
			g_Log.Error(FMT_COMPILE("failed to execute command \"{}\""), m_argv[0]);
		return;
	}

	if (m_aliasMap.count(m_argvHash[0]) != 0) {
		this->Exec(m_aliasMap[m_argvHash[0]]);
		return;
	}

	if (!this->ExecCVarCmd()) {
		g_Log.Error(FMT_COMPILE("command does not exist: \"{}\""), m_argv[0]);
		return;
	}
}

CVarParameter *CVarCmdService::CreateCVar(std::string_view name, std::string_view description)
{
	uint64_t hash = UtilStringHash(name);

	m_cvarMap[hash] = std::make_unique< CVarParameter >();

	auto &param = m_cvarMap[hash];
	param->name = name;
	param->hash = hash;
	param->description = description;
	param->service = this;

	return param.get();
}

bool CVarCmdService::ParseStr(std::string_view str)
{
	IO_EraseCPPVector(m_args);
	IO_EraseCPPVector(m_argv);
	IO_EraseCPPVector(m_argvHash);

	std::array< char, 1024 > tokenBuffer;

	int c_len, e;
	uint32_t c;

	while (true) {
		while (true) {
			c_len = utf8_decode_v2(str.data(), str.data() + str.size(), &c, &e);
			if (e != 0)
				return false;
			if (c == '\0' || c > ' ' || c == '\n')
				break;
			str.remove_prefix(c_len);
		}

		if (c == '\n') {
			str.remove_prefix(c_len);
			break;
		}

		if (c == '\0')
			return true;

		const char *ptr = Com_ParseStr(tokenBuffer.data(), tokenBuffer.size(), str.data());
		if (ptr == nullptr)
			return true;

		auto offset = static_cast< size_t >(ptr - str.data());
		str.remove_prefix(offset);

		m_argv.push_back(tokenBuffer.data());
		m_argvHash.push_back(UtilStringHash(tokenBuffer.data()));
	}

	return true;
}

bool CVarCmdService::ExecCVarCmd()
{
	if (m_cvarMap.count(m_argvHash[0]) == 0)
		return false;

	auto &cvar = m_cvarMap[m_argvHash[0]];

	switch (cvar->type) {
	case static_cast< uint32_t >(CVarType::Float):
		return this->ExecCVarCmdType< double >(cvar.get());
	case static_cast< uint32_t >(CVarType::Int):
		return this->ExecCVarCmdType< int >(cvar.get());
	case static_cast< uint32_t >(CVarType::String):
		return this->ExecCVarCmdType< std::string >(cvar.get());
	default:
		g_Game->Abort("CVarCmdService: What is this type?!");
		return false;
	}
}

template< typename T >
bool CVarCmdService::ExecCVarCmdType(CVarParameter *parameter)
{
	auto array = this->GetCVarArray< T >();
	auto &storage = array->cvars[parameter->arrayIndex];

	if (m_argv.size() == 1) {
		g_Log.Info(FMT_COMPILE("(cvar) \"{}\" = \"{}\""), parameter->name, storage.value);
		return true;
	}

	T val;
	std::stringstream stream;
	stream << m_argv[1];
	stream >> val;

	array->SetValue(parameter->arrayIndex, val);
	return true;
}

template bool CVarCmdService::ExecCVarCmdType< double >(CVarParameter *);
template bool CVarCmdService::ExecCVarCmdType< int >(CVarParameter *);
template bool CVarCmdService::ExecCVarCmdType< std::string >(CVarParameter *);
