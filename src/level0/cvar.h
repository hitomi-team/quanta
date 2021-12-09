#ifndef LEVEL0_CVAR_H
#define LEVEL0_CVAR_H

class CVar {
public:

	CVar() = delete;
	CVar(const std::string &name, const std::string &default_value, const std::string &value);

	void Cleanup();
	void Reset();

	void SetValue(const std::string &value);

	inline std::string GetName() { return m_name; }
	inline std::string GetDefaultValue() { return m_default_value; }

	std::string GetValue();
	float GetFloat();
	bool GetBool();

private:

	std::string m_name, m_default_value, m_value;
	float m_valuef = 0.f;
	bool m_valueb = false;
	std::mutex m_mtx;

};

void CVar_Add(CVar *var);
CVar *CVar_Find(const std::string &name);

void CVar_SaveToFile();
void CVar_CleanAll();
bool CVar_Cmd();

#define CVAR_VAR(name) _cvar_##name
#define CVAR_INIT(name, default_value, value) CVar CVAR_VAR(name)( #name, default_value, value )
#define CVAR_DECLARE(name) extern CVar CVAR_VAR(name)

#endif
