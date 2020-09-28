/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"


#define PROCESS_MOD_OPERATION \
{ \
	unsigned short properties = GET_INS_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties|stk_result_op2->properties);\
	if(properties == MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER){\
			PUSH_INTEGER(STK_VALUE_TO_INT(stk_result_op1) % STK_VALUE_TO_INT(stk_result_op2));\
	}\
	else if(properties == (MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER|MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT)){\
			if (STK_VALUE_IS_INT(stk_result_op1) && STK_VALUE_IS_FLOAT(stk_result_op2)){\
				COPY_FLOAT(&f_aux_value2,&stk_result_op2->stk_value);\
				PUSH_FLOAT(fmod(STK_VALUE_TO_INT(stk_result_op1) , f_aux_value2));\
			}else{\
				COPY_FLOAT(&f_aux_value1,&stk_result_op1->stk_value);\
				PUSH_FLOAT(fmod(f_aux_value1 , STK_VALUE_TO_INT(stk_result_op2)));\
			}\
	}\
	else if(properties == MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT){\
			COPY_FLOAT(&f_aux_value1,&stk_result_op1->stk_value);\
			COPY_FLOAT(&f_aux_value2,&stk_result_op2->stk_value);\
			PUSH_FLOAT(fmod(f_aux_value1 , f_aux_value2));\
	}\
	else{\
		if(!applyMetamethod(\
						 calling_object\
						,calling_function\
						,instruction\
						,STR(%)\
						,BYTE_CODE_METAMETHOD_MOD\
						,stk_result_op1\
						,stk_result_op2\
		)){\
			THROW_RUNTIME_ERROR("cannot perform operation %");\
		}\
	}\
}

#define PROCESS_ARITHMETIC_OPERATION(__C_OP__, __METAMETHOD__)\
{\
	unsigned short properties = GET_INS_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties|stk_result_op2->properties);\
	if(properties == MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER){\
			PUSH_INTEGER(STK_VALUE_TO_INT(stk_result_op1) __C_OP__ STK_VALUE_TO_INT(stk_result_op2));\
	}\
	else if(properties == (MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER|MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT)){\
			if (STK_VALUE_IS_INT(stk_result_op1) && STK_VALUE_IS_FLOAT(stk_result_op2)){\
				COPY_FLOAT(&f_aux_value2,&stk_result_op2->stk_value);\
				PUSH_FLOAT(STK_VALUE_TO_INT(stk_result_op1) __C_OP__ f_aux_value2);\
			}else{\
				COPY_FLOAT(&f_aux_value1,&stk_result_op1->stk_value);\
				PUSH_FLOAT(f_aux_value1 __C_OP__ STK_VALUE_TO_INT(stk_result_op2));\
			}\
	}\
	else if(properties == MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT){\
			COPY_FLOAT(&f_aux_value1,&stk_result_op1->stk_value);\
			COPY_FLOAT(&f_aux_value2,&stk_result_op2->stk_value);\
			PUSH_FLOAT(f_aux_value1 __C_OP__ f_aux_value2);\
	}\
	else{\
		if(!applyMetamethod(\
						calling_object\
						,calling_function\
						,instruction\
						,STR(__C_OP__)\
						,__METAMETHOD__\
						,stk_result_op1\
						,stk_result_op2\
		)){\
			THROW_RUNTIME_ERROR("cannot perform operation '%c'",STR(__C_OP__));\
		}\
	}\
}

#define PROCESS_COMPARE_OPERATION(__C_OP__, __METAMETHOD__)\
{\
	unsigned short properties = GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(stk_result_op1->properties|stk_result_op2->properties);\
	if(properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER){\
			PUSH_BOOLEAN(STK_VALUE_TO_INT(stk_result_op1) __C_OP__ STK_VALUE_TO_INT(stk_result_op2));\
	}\
	else if(properties == MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN){\
		PUSH_BOOLEAN(STK_VALUE_TO_BOOL(stk_result_op1) __C_OP__ STK_VALUE_TO_BOOL(stk_result_op2));\
	}\
	else if(properties == (MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER|MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT)){\
			if (STK_VALUE_IS_INT(stk_result_op1) && STK_VALUE_IS_FLOAT(stk_result_op2)){\
				COPY_FLOAT(&f_aux_value2,&stk_result_op2->stk_value);\
				PUSH_BOOLEAN(STK_VALUE_TO_INT(stk_result_op1) __C_OP__ f_aux_value2);\
			}else{\
				COPY_FLOAT(&f_aux_value1,&stk_result_op1->stk_value);\
				PUSH_BOOLEAN(f_aux_value1 __C_OP__ STK_VALUE_TO_INT(stk_result_op2));\
			}\
	}\
	else if(properties == MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT){\
			COPY_FLOAT(&f_aux_value1,&stk_result_op1->stk_value);\
			COPY_FLOAT(&f_aux_value2,&stk_result_op2->stk_value);\
			PUSH_BOOLEAN(f_aux_value1 __C_OP__ f_aux_value2);\
	}\
	else if((stk_result_op1->properties&stk_result_op2->properties) == MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING){\
		PUSH_BOOLEAN(ZS_STRCMP(STK_VALUE_TO_STRING(stk_result_op1), __C_OP__ ,STK_VALUE_TO_STRING(stk_result_op2)));\
	}else{\
		if(!applyMetamethod(\
					 calling_object\
					,calling_function\
					,instruction\
					,STR(__C_OP__)\
					, __METAMETHOD__\
					,stk_result_op1\
					,stk_result_op2\
		)){\
			THROW_RUNTIME_ERROR("cannot perform operation '%c'",STR(__C_OP__));\
		}\
	}\
}

#define PROCESS_LOGIC_OPERATION(__C_OP__)\
{\
	unsigned short properties = GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(stk_result_op1->properties|stk_result_op2->properties);\
	if(properties == MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN){\
		PUSH_BOOLEAN(STK_VALUE_TO_BOOL(stk_result_op1) __C_OP__ STK_VALUE_TO_BOOL(stk_result_op2));\
	}else{\
		PRINT_DUAL_ERROR_OP(STR(__C_OP__));\
	}\
}

#define PROCESS_BINARY_OPERATION(__C_OP__, __METAMETHOD__)\
{\
	unsigned short properties = GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(stk_result_op1->properties|stk_result_op2->properties);\
	if(properties == MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER){\
		PUSH_INTEGER(STK_VALUE_TO_INT(stk_result_op1) __C_OP__ STK_VALUE_TO_INT(stk_result_op2));\
	}else{\
		if(!applyMetamethod(\
						 calling_object\
						,calling_function\
						,instruction\
						,STR(__C_OP__)\
						, __METAMETHOD__\
						,stk_result_op1\
						,stk_result_op2\
		)){\
			THROW_RUNTIME_ERROR("cannot perform operation '%c'",STR(__C_OP__));\
		}\
	}\
}

#define PERFORM_PRE_POST_OPERATOR(stk_var, __OPERATOR__) \
{\
	void **ref=(void **)(&((stk_var)->stk_value));\
	if(stk_var->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C){\
		ref=(void **)((stk_var)->var_ref);\
	}\
	switch(GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES((stk_var)->properties)){\
	case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER:\
			(*((int *)(ref)))__OPERATOR__;\
			break;\
	case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT:\
			(*((float *)(ref)))__OPERATOR__;\
			break;\
	default:\
		THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,instruction)," Cannot perform pre/post operator (%s)",stk_var->toString());\
		break;\
	}\
}


#define SHARE_LIST_INSERT(list,_node){\
		if(list.first == NULL){\
			_node->previous=_node->next= list.last = list.first =_node;\
		}\
		else{\
			list.last->next=_node;\
			_node->previous=list.last;\
			list.last=_node;\
			list.first->previous=list.last;\
			list.last->next=list.first;\
		}\
	}

#define SHARE_LIST_DEATTACH(list,_node) \
{\
		if(_node->next == _node){/*one  node: trivial ?*/\
			list.first = list.last = _node->next = _node->previous=NULL;\
		}else{/* >1 node */\
			PInfoSharedPointerNode previous=_node->previous;\
			PInfoSharedPointerNode next=_node->next;\
			if(_node==list.first){\
				list.first = next;\
			}\
			else if(_node==list.last){\
				list.last = previous;\
			}\
			previous->next = next;\
			next->previous = previous;\
		}\
}

#define PUSH_VM_SCOPE(_scope,_ptr_info_function, _ptr_local_var,_properties) {\
	if(vm_current_scope >=  vm_scope_max){THROW_RUNTIME_ERROR("reached max scope");}\
	*vm_current_scope++={(Scope *)_scope,_ptr_info_function,_ptr_local_var,_properties};\
}

#define CALL_GC SHARED_LIST_DESTROY(zero_shares[idx_stk_current])

namespace zetscript{


	void VirtualMachine::doStackDump(){
		// derefer all variables in all scopes (except main )...
		while(vm_scope<(vm_current_scope)){
			popVmScope(idx_stk_current,NULL,0);
		}
		idx_stk_current=0;
	}

	VirtualMachine::VirtualMachine(ZetScript *_zs){
		//-----------------------------------------------------------
		// set memory manager

		memset(zero_shares,0,sizeof(zero_shares));
		memset(shared_var,0,sizeof(shared_var));
		StackElement *aux=vm_stack;

		for(int i=0; i < VM_STACK_LOCAL_VAR_MAX;i++){
			*aux++={0,NULL,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED};
		}

		idx_stk_current=0;
		//------------------

		//f_return_value=0;
		//s_return_value="unknow";

		cancel_execution=false;

		vm_stk_current=NULL;
		vm_current_scope = vm_scope;

		f_aux_value1=0;
		f_aux_value2=0;

		custom_error = "unknow_error";

		idx_last_statment=0;

		//vm_str_last=&vm_str[VM_MAX_AUX_STRINGS-1]; // aux values for std::string ...
		//vm_str_current=NULL;

		vm_scope_max = &vm_scope[VM_SCOPE_MAX-1];

		main_function_object = NULL;
		main_class_object=NULL;

		vm_foreach_current=NULL;
		current_call_c_function = NULL;
		zs=_zs;

		script_function_factory=NULL;
		script_class_factory=NULL;
		scope_factory=NULL;
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

	PInfoSharedPointerNode VirtualMachine::newSharedPointer(ScriptVar *_var_ptr){
		//int index = VirtualMachine::getFreeCell();
		PInfoSharedPointerNode _node = (PInfoSharedPointerNode)malloc(sizeof(InfoSharedPointerNode));
		_node->data.n_shares=0;
		_node->data.shared_ptr=_var_ptr;
		_node->currentStack = idx_stk_current;

		// insert node into shared nodes ...
		SHARE_LIST_INSERT(zero_shares[idx_stk_current],_node);
		//zero_shares[idx_stk_current].InsertNode(_node);
		return _node;
	}

	void VirtualMachine::sharePointer(PInfoSharedPointerNode _node){

		unsigned char *n_shares = &_node->data.n_shares;

		bool move_to_shared_list=*n_shares==0;

		if(*n_shares >= MAX_SHARES_VARIABLE){
			THROW_RUNTIME_ERROR("MAX SHARED VARIABLES (Max. %i)",MAX_SHARES_VARIABLE);
		}

		(*n_shares)++;

		if(move_to_shared_list){

			// Mov to shared pointer...
			SHARE_LIST_DEATTACH(zero_shares[_node->currentStack],_node);
			// update current stack due different levels from functions!
			_node->currentStack=idx_stk_current;
			SHARE_LIST_INSERT(shared_var[idx_stk_current],_node);
		}
	}

	void VirtualMachine::unrefSharedScriptVar(PInfoSharedPointerNode _node, bool remove_if_0){

		unsigned char *n_shares = &_node->data.n_shares;
		if(*n_shares > 0){
			if(--(*n_shares)==0){ // mov back to 0s shares (candidate to be deleted on GC check)

				SHARE_LIST_DEATTACH(shared_var[_node->currentStack],_node);

				if(remove_if_0){ // remove node and data instead...
					delete _node->data.shared_ptr;
					free(_node);
				}
				else{ // insert into zero array.. if not referenced anymore will be removed by REMOVE_0_SHARED
					// update current stack due different levels from functions!
					_node->currentStack=idx_stk_current;
					SHARE_LIST_INSERT(zero_shares[idx_stk_current],_node);
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
		//writeError("custom_user",-1,str);
		custom_error = str;
		cancel_execution = true;
	}

	void VirtualMachine::cancelExecution(){
		custom_error=NULL;
		cancel_execution = true;
	}

	void VirtualMachine::clearGlobalVars(){

		// remove all shared 0 pointers
		if(main_function_object->registered_symbols->count > 0){
			// set global top stack element
			StackElement *vm_stk_element=&vm_stack[main_function_object->registered_symbols->count-1];

			for (int v = main_function_object->registered_symbols->count-1
						 ;v>=0
						 ;v--) {
				Symbol *symbol=(Symbol *)main_function_object->registered_symbols->items[v];
				if((symbol->properties & SYMBOL_PROPERTY_C_OBJECT_REF) != SYMBOL_PROPERTY_C_OBJECT_REF){

					StackElement *ptr_ale =&vm_stack[v];
					ScriptVar *var = NULL;

					if(ptr_ale->properties &MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){
						var =((ScriptVar *)(ptr_ale->var_ref));
						if(var){
							if(var->ptr_shared_pointer_node != NULL){
								var->unrefSharedPtr();
							}
						}
					}

					main_function_object->registered_symbols->pop_back();

					// clear global function/variable
					*vm_stk_element--={0,NULL,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED};
				}
				else{
					break;
				}
			}
		}

		removeEmptySharedPointers(0,NULL);

		memset(zero_shares,0,sizeof(zero_shares));
		memset(shared_var,0,sizeof(shared_var));
		idx_stk_current=0;
	}

	StackElement  VirtualMachine::execute(
			 ScriptFunction *calling_function
			 ,ScriptVar *this_object
			 ,StackElement *  stk_params
			 ,unsigned	char  n_stk_params
			){

		StackElement stk_result={0,0,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED};
		StackElement info={0,0,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED};

		if(calling_function==NULL){
			throw "calling function NULL";
		}

		if(idx_stk_current==0){ // set stack and Init vars for first call...

			if(main_function_object->instructions == NULL){ // no statments
				throw "instructions NULL";
			}
			cancel_execution=false;

			vm_stk_current=vm_stack;

			// calls script function from C : preserve stack space for global vars
			if(calling_function->idx_script_function != IDX_SCRIPT_FUNCTION_MAIN){
				vm_stk_current=&vm_stack[main_function_object->registered_symbols->count];
			}
			vm_foreach_current=&vm_foreach[0];
		}else{ // Not main function -> allow params for other functions
			// push param stack elements...
			for(unsigned i = 0; i < n_stk_params; i++){
				*vm_stk_current++=stk_params[i];
			}
		}

		// byte code executing starts here. Later script function can call c++ function, but once in c++ function is not possible by now call script function again.
		try{
			info=callFunctionScript(
				calling_function,
				this_object,
				vm_stk_current,
				n_stk_params);
		}catch(std::exception & ex){
			// it was error so reset stack and stop execution ? ...
			doStackDump();

			throw std::runtime_error(ex.what());
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
				case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER:
				case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN:
				case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT:
					break;
				case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING:
					delete (ScriptVarString *)stk->var_ref;
					break;
				case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR:
					delete (ScriptVar *)stk->var_ref;
					break;

				}
			}
		}

		//destroyCache();
	}
}


#include "VirtualMachine_call_function_native.cpp"
#include "VirtualMachine_call_function_script.cpp"
