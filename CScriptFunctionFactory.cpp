#include "CZetScript.h"

namespace zetscript{

	CScriptFunctionFactory * CScriptFunctionFactory::script_function_factory_singleton=NULL;



	CScriptFunctionFactory * CScriptFunctionFactory::getInstance(){
		if(script_function_factory_singleton ==NULL){
			script_function_factory_singleton = new CScriptFunctionFactory();
		}

		return script_function_factory_singleton;
	}

	void CScriptFunctionFactory::destroySingleton(){

		if(script_function_factory_singleton !=NULL){
			delete script_function_factory_singleton;
			script_function_factory_singleton = NULL;
		}
	}

	//----

	CScriptFunctionFactory::CScriptFunctionFactory(){

	}

	vector<CScriptFunction *> 	*CScriptFunctionFactory::getVectorScriptFunctionNode(){
		return &vec_script_function_node;
	}

	CScriptFunction *		 CScriptFunctionFactory::newScriptFunction(unsigned char idxClass, short idxScope, const string & function_ref, vector<tArgumentInfo> args, int idx_return_type,intptr_t ref_ptr, unsigned short properties){

		if((properties & PROPERTY_C_OBJECT_REF) == PROPERTY_C_OBJECT_REF){
			if(vec_script_function_node.size() > 1){ // if greather than 1 check if node consecutive...
				if(!(vec_script_function_node.at(vec_script_function_node.size()-1)->symbol_info.properties & PROPERTY_C_OBJECT_REF) == PROPERTY_C_OBJECT_REF){ // non consecutive c node..
						THROW_RUNTIME_ERROR("C Functions should be added after global scope and consecutuve C scope node.");
						return NULL;
				}
			}
		}

		CScriptFunction *irs = new CScriptFunction(idxScope,idxClass);

		irs->m_arg = args;
		irs->idx_return_type = idx_return_type;
		irs->symbol_info.ref_ptr = ref_ptr;


		irs->symbol_info.symbol_ref = function_ref; // <-- defined as global
		irs->symbol_info.properties = properties;

		irs->symbol_info.idxSymbol = (short)(irs->m_function.size());

		vec_script_function_node.push_back(irs);
		irs->idxScriptFunction = vec_script_function_node.size()-1;
		return irs;
	}

	CScriptFunction 	* CScriptFunctionFactory::getScriptFunction(int idx){
		if(idx < 0 || (unsigned)idx >= vec_script_function_node.size()){
			THROW_RUNTIME_ERROR("CScriptFunction node out of bound");
			return NULL;
		}

		return vec_script_function_node.at(idx);
	}

	bool CScriptFunctionFactory::checkCanRegister_C_Function(const char *f){

		int size = vec_script_function_node.size();

		if(size>=3){ //0 is main function (reserved). We check when >= 3 to check previous one (i.e from 1)
			if((

				((vec_script_function_node)[size-1]->symbol_info.properties&PROPERTY_C_OBJECT_REF)!=PROPERTY_C_OBJECT_REF)
			){
				THROW_RUNTIME_ERROR("function \"%s\" should register after C functions. Register after script functions is not allowed",f);
				return false;
			}

		}

		return true;
	}

	void CScriptFunctionFactory::clear(){
		bool end=false;
		do{
			CScriptFunction * info_function = vec_script_function_node.at(vec_script_function_node.size()-1);
			end=(info_function->symbol_info.properties & PROPERTY_C_OBJECT_REF) == PROPERTY_C_OBJECT_REF || vec_script_function_node.size()==1;

			if(!end){

				if (info_function->instruction != NULL) {
					//for (PtrInstruction stat = info_function->object_info.instruction; *stat != NULL; stat++) {

						//free(*stat);
					//}

					free(info_function->instruction);
					info_function->instruction=NULL;
				}

				// unloading scope ...
				if (info_function->lut_scope_symbol != NULL) {
					for (unsigned j = 0; j < info_function->n_lut_scope_symbols; j++) {
						free(info_function->lut_scope_symbol[j].var_index);
					}

					free(info_function->lut_scope_symbol);
					info_function->lut_scope_symbol=NULL;
				}

				vec_script_function_node.pop_back();
				delete info_function;

			}

		}while(!end);

	}


	CScriptFunctionFactory::~CScriptFunctionFactory(){
		for(unsigned i = 0;i < vec_script_function_node.size();i++){
				//zs_print_debug_cr("* Erasing function %s...", vec_script_function_node.at(i)->object_info.symbol_info.symbol_ref.c_str());
				CScriptFunction * info_function = vec_script_function_node.at(i);

				if (info_function->instruction != NULL) {

					free(info_function->instruction);
					info_function->instruction=NULL;
				}

				// unloading scope ...
				if (info_function->lut_scope_symbol != NULL) {
					for (unsigned j = 0; j < info_function->n_lut_scope_symbols; j++) {
						free(info_function->lut_scope_symbol[j].var_index);
					}

					free(info_function->lut_scope_symbol);
					info_function->lut_scope_symbol=NULL;
				}

				delete info_function;
			}

			vec_script_function_node.clear();
	}

};
