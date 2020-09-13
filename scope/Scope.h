/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{
	class ScopeFactory;
	class ZetScript;
	class  Scope{
	public:

		bool is_c_node;
		ScriptClass *script_class;
		int tmp_idx_instruction_push_scope;
		// This flag is used to remove usign eraseUnusedScopes...
		bool unusued;
		//void		*eval_scope_tmp; // aux variable used on evaluation
		Scope 		*scope_parent, *scope_base;

		zs_vector				*registered_scopes;  // local scopes from starting block { }
		zs_vector				*registered_symbols; // local symbols from starting block { }
		//std::vector<Scope *> local_scopes;
		//std::vector<Symbol *> symbols; // vars registered from base.

		//--------------------------------------------------------------------
		// Functions

		Scope(ZetScript * _zs, Scope * _scope_parent=NULL,bool is_c_node=false);

		//--------------------------------------------------------------------
		// Register functions

		/**
		 * register/search symbol info
		 * @n_params:
		 */
		Symbol * registerNativeFunctionSymbol(const std::string & file, short line,const std::string & symbol_name, char n_params);
		Symbol * registerSymbol(const std::string & file, short line,const std::string & symbol_name, char n_params=NO_PARAMS_SYMBOL_ONLY);
		Symbol * getSymbol(const std::string & var_name, char n_params=NO_PARAMS_SYMBOL_ONLY);

		void						    markAsUnusued();


		void setScriptClass(ScriptClass *sc);
		ScriptClass * getScriptClass();

		~Scope();

	private:

		ZetScript *zs;
		ScopeFactory *scope_factory;

		Symbol * getSymbolRecursive(const std::string & var_name, char n_params=NO_PARAMS_SYMBOL_ONLY);
		Symbol * getSymbolRecursiveDownScope(const std::string & ref_symbol, char n_params=NO_PARAMS_SYMBOL_ONLY);
		Symbol * getSymbolRecursiveUpScope(const std::string & ref_symbol, char n_params=NO_PARAMS_SYMBOL_ONLY);


	};
}
