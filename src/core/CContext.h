#pragma once

class CContext{


	vector<CScriptVariable *> m_function; // 0 is the main scope or main function.
public:

	CContext();

	bool eval(const string & s);
	bool executeFunction(string & function, CVariable * argv[]);

	~CContext();

};
