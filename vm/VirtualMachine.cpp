/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

#define CALL_GC SHARED_LIST_DESTROY(zero_shares[idx_current_call])

namespace zetscript{


	void VirtualMachine::doStackDump(){
		// derefer all variables in all scopes (except main )...
		while(vm_scope<(vm_current_scope)){
			popVmScope(false);
		}

		removeEmptySharedPointers(IDX_CALL_STACK_MAIN);
		idx_current_call=0;
	}

	VirtualMachine::VirtualMachine(ZetScript *_zs){
		//-----------------------------------------------------------
		// set memory manager

		memset(&zero_shares,0,sizeof(zero_shares));
		memset(&shared_vars,0,sizeof(shared_vars));
		StackElement *aux=vm_stack;

		for(int i=0; i < VM_STACK_LOCAL_VAR_MAX;i++){
			*aux++={0,NULL,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED};
		}

		idx_current_call=0;
		vm_stk_current=NULL;
		vm_current_scope = vm_scope;

		f_aux_value1=0;
		f_aux_value2=0;

		idx_last_statment=0;

		vm_scope_max = &vm_scope[VM_SCOPE_MAX-1];

		main_function_object = NULL;
		main_class_object=NULL;

		current_call_c_function = NULL;
		zs=_zs;

		script_function_factory=NULL;
		script_class_factory=NULL;
		scope_factory=NULL;
		error=false;
		error_str="";
	}

	#ifdef  __ZETSCRIPT_VERBOSE_MESSAGE__

	#define print_vm_cr ZS_PRINT_INFO
	#else
	#define print_vm_cr(s,...)
	#endif


	void VirtualMachine::init(){
		script_function_factory=this->zs->getScriptFunctionFactory();
		script_class_factory=this->zs->getScriptClassFactory();
		scope_factory = this->zs->getScopeFactory();
		main_function_object = MAIN_FUNCTION(this);
		main_class_object = SCRIPT_CLASS_MAIN(this);
	}


	//============================================================================================================================================
	// POINTER MANANAGER

	InfoSharedPointerNode * VirtualMachine::newSharedPointer(ScriptObject *_var_ptr){
		//int index = VirtualMachine::getFreeCell();
		InfoSharedPointerNode *_node = (InfoSharedPointerNode *)malloc(sizeof(InfoSharedPointerNode));
		// init
		_node->previous=NULL;
		_node->next=NULL;
		_node->data.n_shares=0;
		_node->data.shared_ptr=_var_ptr;
		_node->data.zero_shares=&zero_shares[idx_current_call];

		// insert node into shared nodes ...
		insertShareNode(_node->data.zero_shares,_node);
		return _node;
	}

	void VirtualMachine::sharePointer(InfoSharedPointerNode *_node){

		unsigned char *n_shares = &_node->data.n_shares;

		bool move_to_shared_list=*n_shares==0;

		if(*n_shares >= MAX_SHARES_VARIABLE){
			THROW_RUNTIME_ERROR("MAX SHARED VARIABLES (Max. %i)",MAX_SHARES_VARIABLE);
		}

		(*n_shares)++;

		if(move_to_shared_list){

			// Mov to shared pointer...
			deattachShareNode(_node->data.zero_shares,_node);
			// update current stack due different levels from functions!
			//_node->currentStack=idx_current_call;
			insertShareNode(&shared_vars,_node);

			// node is not in list of zero refs anymore...
			_node->data.zero_shares=NULL;
		}
	}

	void VirtualMachine::unrefSharedScriptObject(InfoSharedPointerNode *_node, bool remove_if_0){

		unsigned char *n_shares = &_node->data.n_shares;
		if(*n_shares > 0){ // already zero
			if(--(*n_shares)==0){ // mov back to 0s shares (candidate to be deleted on GC check)

				deattachShareNode(&shared_vars,_node);

				if(remove_if_0){ // force remove node and data ...
					delete _node->data.shared_ptr;
					free(_node);
				}
				else{ // insert into zero array.. if not referenced anymore will be removed by REMOVE_0_SHARED
					// update current stack due different levels from functions!
					_node->data.zero_shares=&zero_shares[idx_current_call];
					insertShareNode(_node->data.zero_shares,_node);

				}
			}
		}
	}

	const ScriptFunction * VirtualMachine::getCurrent_C_FunctionCall(){
		return current_call_c_function;
	}

	StackElement * VirtualMachine::getStackElement(unsigned int idx_glb_element){

		if(idx_glb_element < main_function_object->registered_symbols->count){
			return &vm_stack[idx_glb_element];
		}
		return NULL;
	}

	void VirtualMachine::setError(const char *str){
		error = true;
		error_str=str;
	}

	StackElement  VirtualMachine::execute(
			 ScriptFunction 	*	calling_function
			 ,ScriptObject 		*	this_object
			 ,StackElement 		*  	stk_params
			 ,unsigned	char  		n_stk_params
	){

		StackElement stk_result={0,0,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED};
		StackElement info={0,0,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED};

		if(idx_current_call==0){ // set stack and Init vars for first call...

			error=false;
			error_str="";

			vm_stk_current=vm_stack;

			// calls script function from C : preserve stack space for global vars
			if(calling_function->idx_script_function != IDX_SCRIPT_FUNCTION_MAIN){
				vm_stk_current=&vm_stack[main_function_object->registered_symbols->count];
			}
			//vm_foreach_current=&vm_foreach[0];
		}else{ // Not main function -> allow params for other functions
			// push param stack elements...
			for(unsigned i = 0; i < n_stk_params; i++){
				*vm_stk_current++=stk_params[i];
			}
		}

		// byte code executing starts here. Later script function can call c++ function, but once in c++ function is not possible by now call script function again.

		info=callFunctionScript(
			this_object,
			calling_function,
			vm_stk_current,
			n_stk_params);

		if(error){
			// it was error so reset stack and stop execution ? ...
			doStackDump();
			throw std::runtime_error(error_str);
		}

		return info;
	}

	void VirtualMachine::setStackElement(unsigned int idx, StackElement stk){
		if(idx >= VM_STACK_LOCAL_VAR_MAX){
			throw "setStackElement: out of bounds";
		}

		vm_stack[idx]=stk;
	}

	StackElement  * VirtualMachine::getLastStackValue(){
		return (vm_stk_current-1);
	}


	VirtualMachine::~VirtualMachine(){
		// destroy c variable scripts
		ScriptFunction * main_function = this->script_function_factory->getScriptFunction(IDX_SCRIPT_FUNCTION_MAIN);

		// clear all symbols except c variables/functions ...
		for (int v = 0;
			 v<main_function->registered_symbols->count;
			 v++) {
			Symbol *symbol=(Symbol *)main_function->registered_symbols->items[v];
			if (((symbol->properties & SYMBOL_PROPERTY_C_OBJECT_REF) == SYMBOL_PROPERTY_C_OBJECT_REF)
			) {
				// if is variable we should delete
				//main_function->registered_symbols->pop_back();
				StackElement *stk=&vm_stack[v];
				switch(GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(stk->properties)){
				default:
					break;
				case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_ZS_INT:
				case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOL:
				case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT:
					break;
				case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING:
				case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPT_OBJECT:
					if(stk->var_ref != NULL){
						delete (ScriptObject *)stk->var_ref;
					}
					break;

				}
			}
		}
	}
}


#include "VirtualMachine_call_function_native.cpp"
#include "VirtualMachine_call_function_script.cpp"
