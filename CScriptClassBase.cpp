#include "zetscript.h"

namespace zetscript{

	CScriptClassBase::CScriptClassBase(CZetScript * _zs,unsigned char _idxClass) {
		idx_class=_idxClass;
		zs = _zs;
		scope_factory = zs->getScopeFactory();
		script_function_factory= zs->getScriptFunctionFactory();
		virtual_machine = zs->getVirtualMachine();
		script_class_factory=zs->getScriptClassFactory();


	}

	VariableSymbolInfo * CScriptClassBase::registerVariable(const std::string & file, short line, short idxBlockScope,const std::string & variable_name, const std::string & c_type, intptr_t ref_ptr, unsigned short properties){
		VariableSymbolInfo irs;
		//std::string symbol_ref=CEval::makeSymbolRef(variable_name,idxBlockScope);

		Symbol * symbol=NULL;

		if((symbol=GET_SCOPE(idxBlockScope)->registerSymbol(file,line,variable_name /*,var_node*/))==NULL){
				return NULL;
		}


		if(getVariable(symbol->name,symbol->idxScope) != NULL){
			THROW_RUNTIME_ERROR(string::sformat("Variable \"%s\" already exist",variable_name.c_str()));
			return NULL;
		}

		irs.ref_ptr =ref_ptr;
		irs.symbol=symbol;
		irs.c_type = c_type;
		irs.properties = properties;

		irs.idxSymbol = (short)local_variable.size();

		local_variable.push_back(irs);

		return &local_variable[local_variable.size()-1];
	}

	VariableSymbolInfo *	CScriptClassBase::registerVariable(const std::string & file, short line, const std::string & variable_name, const std::string & c_type, intptr_t ref_ptr, unsigned short properties)
	{
			return registerVariable(file,line,this->symbol_info.symbol->idxScope,  variable_name,  c_type,  ref_ptr,   properties);
	}

	VariableSymbolInfo *	 CScriptClassBase::getVariable(const std::string & var_name, short idxScope){

		if(local_variable.size()>0){

			// from lat value to first to get last override function...
			for(int i = (int)local_variable.size()-1; i >= 0 ; i--){
				if((local_variable[i].symbol->name == var_name)
				&& (idxScope ==  ZS_UNDEFINED_IDX?true:(idxScope == local_variable[i].symbol->idxScope))
				  ){
					return &local_variable[i];
				}
			}
		}

		return NULL;
	}



	CScriptFunction * CScriptClassBase::registerFunction(const std::string & file, short line, short idxScope, const std::string & function_name, std::vector<ParamArgInfo> args, int idx_return_type,intptr_t ref_ptr, unsigned short properties){

			//std::string symbol_ref = CEval::makeSymbolRef(function_name,idxScope);
			if(getFunction(function_name,(char)args.size()) != NULL){
				THROW_RUNTIME_ERROR(string::sformat("Function \"%s\" already exist",function_name.c_str()));
				return NULL;
			}

			CScriptFunction *sf =  NEW_SCRIPT_FUNCTION(file,line,idx_class,idxScope,  function_name,  args,  idx_return_type,ref_ptr, properties);
			sf->idx_class = this->idx_class;
			sf->idxLocalFunction=local_function.size();
			local_function.push_back(sf);

			return sf;
	}

	CScriptFunction * CScriptClassBase::registerFunction(const std::string & file, short line, const std::string & function_name, std::vector<ParamArgInfo> args, int idx_return_type,intptr_t ref_ptr, unsigned short properties){

		return registerFunction(file, line,this->symbol_info.symbol->idxScope, function_name,  args, idx_return_type,ref_ptr, properties);
	}

	CScriptFunction *	 CScriptClassBase::getFunction(const std::string & function_name, short idxScope, char n_args){

		if(local_function.size()>0){

			// from last value to first to get last override function...
			for(int i = (int)(local_function.size()-1); i >= 0 ; i--){
				if(
						(local_function[i]->symbol_info.symbol->name == function_name)
					 && (local_function[i]->arg_info.size() ==  n_args)
					 && (idxScope ==  ZS_UNDEFINED_IDX?true:(idxScope == local_function[i]->symbol_info.symbol->idxScope))
					 ){

					return local_function[i];
				}
			}
		}

		return NULL;
	}


}
