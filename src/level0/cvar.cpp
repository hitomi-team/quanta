#include "level0/pch.h"
#include "cmd.h"
#include "log.h"

static std::vector< CVar * > CVarList;
static std::mutex CVarList_mtx;

CVar::CVar(const std::string &name, const std::string &default_value, const std::string &value)
{
	m_name = name;
	m_default_value = default_value;
	SetValue(value);
}

void CVar::Cleanup()
{
	std::lock_guard< std::mutex > lock(m_mtx);

	m_valuef = 0.0f;
	m_valueb = false;
}

void CVar::Reset()
{
	Cleanup();
	SetValue(m_default_value);
}

void CVar::SetValue(const std::string &value)
{
	std::lock_guard< std::mutex > lock(m_mtx);

	m_value = value;
	m_valuef = (float)atof(value.c_str());
	m_valueb = m_valuef != 0.0f;
}

std::string CVar::GetValue()
{
	std::lock_guard< std::mutex > lock(m_mtx);
	return m_value;
}

float CVar::GetFloat()
{
	std::lock_guard< std::mutex > lock(m_mtx);
	return m_valuef;
}

bool CVar::GetBool()
{
	std::lock_guard< std::mutex > lock(m_mtx);
	return m_valueb;
}

void CVar_Add(CVar *var)
{
	std::lock_guard< std::mutex > lock(CVarList_mtx);

	for (auto &ivar : CVarList) {
		if (var->GetName() == ivar->GetName())
			return;
	}

	CVarList.push_back(var);
	g_Log.Debug(FMT_COMPILE("Add CVar \"{}\""), var->GetName());
}

CVar *CVar_Find(const std::string &name)
{
	std::lock_guard< std::mutex > lock(CVarList_mtx);

	for (CVar *var : CVarList) { 
		if (var->GetName() == name)
			return var;
	}

	return nullptr;
}

void CVar_SaveToFile()
{
	std::lock_guard< std::mutex > lock(CVarList_mtx);
	throw std::runtime_error("CVar_SaveToFile unimplemented");
}

void CVar_CleanAll()
{
	std::lock_guard< std::mutex > lock(CVarList_mtx);
	for (CVar *var : CVarList)
		var->Cleanup();
}

bool CVar_Cmd()
{
	CVar *cvar = CVar_Find(Cmd_Argv(0));

	if (cvar == nullptr)
		return false;

	if (Cmd_Argc() == 1) {
		g_Log.Info(FMT_COMPILE("cvar \"{}\" is \"{}\""), cvar->GetName(), cvar->GetValue());
		return true;
	}

	cvar->SetValue(Cmd_Argv(1));
	return true;
}
