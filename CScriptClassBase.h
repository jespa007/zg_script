#pragma once

namespace zetscript{

	class CScriptFunctionFactory;
	class CZetScript;
	class CScriptClassBase { // script function is shared by class and function ...



	public:

		CZetScript *zs;
		CScopeFactory *scope_factory;
		CScriptFunctionFactory *script_function_factory;

		bool searchVarFunctionSymbol( std::string symbol_to_find,OpCodeInstruction *iao, int current_function, bool & symbol_not_found, unsigned int param_scope_type, int n_args_to_find=-1);


		// local symbols for class or function...
		std::vector<VariableSymbolInfo> m_variable; // member variables to be copied in every new instance
		std::vector<CScriptFunction *>   m_function; // idx member functions (from main std::vector collection)

		VariableSymbolInfo symbol_info; // class/function symbol info ...

		unsigned char idx_class; // wich idx_class class itself or function belongs to...

		CScriptClassBase(unsigned char _idxClass, CZetScript *_zs);

		/* Registers local variable
		 * Desc: Inserts variable at scope some block scope or by scope info itself.
		 */
		VariableSymbolInfo *  registerVariable(const std::string & file, short line,  short idxBlockScope,const std::string & variable, const std::string & c_type="", intptr_t ref_ptr=0, unsigned short properties=0);
		VariableSymbolInfo *  registerVariable(const std::string & file, short line, const std::string & variable, const std::string & c_type="", intptr_t ref_ptr=0, unsigned short properties=0);
		VariableSymbolInfo *  getVariable(const std::string & symbol_ref, short idxScope);


		/* Registers a function.
		 * Desc: Inserts function at custom scope. It returns the idx std::vector element on symbol_info.scope_info.[vRegisteredFunction/vRegisteredVariables]
		 */
		CScriptFunction * registerFunction(const std::string & file, short line,  short idxBlockScope, const std::string & function_name, std::vector<ParamArgInfo> args={}, int idx_return_type=ZS_UNDEFINED_IDX,intptr_t ref_ptr=0, unsigned short properties=0);
		CScriptFunction * registerFunction(const std::string & file, short line,  const std::string & function_name, std::vector<ParamArgInfo> args={}, int idx_return_type=ZS_UNDEFINED_IDX,intptr_t ref_ptr=0, unsigned short properties=0);
		CScriptFunction * getFunction(const std::string & symbol, short idxScope,char n_args=0);



	};
}
