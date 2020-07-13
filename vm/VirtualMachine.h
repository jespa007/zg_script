#pragma once

namespace zetscript{


	//#define MAX_PER_TYPE_OPERATIONS 32

	#define VM_STACK_LOCAL_VAR_MAX				256
	#define VM_SCOPE_MAX						64
	#define VM_FOREACH_MAX						64
	#define MAX_FUNCTION_CALL 					128
	#define VM_MAX_AUX_STRINGS					128
	#define MAX_SHARES_VARIABLE 				64


	#define SET_INTEGER_RETURN(i)	CURRENT_VM->setIntegerReturnValue(i)
	#define SET_BOOLEAN_RETURN(b) 	CURRENT_VM->setBooleanReturnValue(b)
	#define SET_FLOAT_RETURN(f)   	CURRENT_VM->setFloatReturnValue(f)
	#define SET_STRING_RETURN(s)  	CURRENT_VM->setStringReturnValue(s)

	#define NO_PARAMS std::vector<StackElement>{}
	#define ZS_VM_FUNCTION_TYPE std::function<ScriptVar * (const std::vector<ScriptVar *> & param)>

	class ScriptFunction;
	class ZetScript;
	class  VirtualMachine{

		//===================================================================================================
		//
		// POINTER MANAGER ...
		//

		typedef struct {
			PInfoSharedPointerNode first, last;
		}InfoSharedList;

		InfoSharedList zero_shares[MAX_FUNCTION_CALL];
		InfoSharedList shared_var[MAX_FUNCTION_CALL];

		//===================================================================================================

		typedef struct{
			ByteCode byte_code;
			const char *str;
		}OpCodeInfo;

		ScriptFunction  *main_function_object;
		ScriptFunction 	**script_functions;
		unsigned				size_vec_script_function_object_node;


		int idx_stk_current;
		int idx_last_statment;
		const ScriptFunction *current_call_c_function;
		ZetScript *zs;
		ScriptFunctionFactory *script_function_factory;
		ScriptClassFactory 	*script_class_factory;

		float f_return_value;
		std::string s_return_value;
		StackElement stk_aux;

		StackElement  callFunctionInternal(
				ScriptFunction *info_function,
				ScriptVar * this_object,
				bool & error,
				StackElement 		  * _ptrStartOp=NULL,
				std::string 		  		  * _ptrStartStr=NULL,
				unsigned char n_args=0,
				Instruction *calling_instruction = NULL);

		bool cancel_execution;
		const char *custom_error;


	public:

		//===================================================================================================
		//
		// POINTER MANAGER ...
		//

		PInfoSharedPointerNode newSharedPointer(ScriptVar *var_ptr);
		bool sharePointer( PInfoSharedPointerNode _node);
		void unrefSharedScriptVar( PInfoSharedPointerNode _node, bool remove_if_0=false);
		void removeSharedScriptVar( PInfoSharedPointerNode _node);

		const ScriptFunction * getCurrent_C_FunctionCall();


		inline float *setFloatReturnValue(float f){
			f_return_value = f;
			return &f_return_value;
		}

		inline std::string *setStringReturnValue(std::string s){
			s_return_value = s;
			return &s_return_value;
		}

		void addGlobalVar(const StackElement & stk);

		void clearGlobalVars();

		 StackElement callFunction(
			 ScriptFunction *script_function,
			 ScriptVar *this_object,
			 bool & error,
			const std::vector<StackElement> &  argv=NO_PARAMS
		);

		StackElement *getLastStackValue();
		StackElement * getStackElement(unsigned int idx_glb_element);

		void buildCache();
		void destroyCache();

		void cancelExecution();
		void setError(const char *str);

		VirtualMachine(ZetScript * _zs);
		~VirtualMachine();

	private:

		struct VM_Scope{
			short				index;
			ScriptFunction 		*script_function;
			StackElement 		*stk_local_vars;
			unsigned char 		properties;
		};

		struct VM_Foreach{
			StackElement 		   	*key; // iterator element can be std::string or integer...
			ScriptVar				*ptr; // can be struct or std::vector...
			unsigned int 		   	idx_current;

		};

		char				str_aux[8192];
		float 				f_aux_value1,f_aux_value2;
		 std::string 		aux_string,symbol_to_find,error_str;
		 VM_Foreach 		vm_foreach[VM_FOREACH_MAX];
		 VM_Foreach 		*vm_foreach_current;



		 std::string		aux_string_param[MAX_N_ARGS]; // for std::string params...

		 VM_Scope		*vm_scope_current;
		 VM_Scope		vm_scope[VM_SCOPE_MAX];
		 VM_Scope		*vm_scope_max;


		std::string			vm_str[VM_MAX_AUX_STRINGS]; // aux values for std::string ...
		std::string			*vm_str_last;
		std::string			*vm_str_current;

		 StackElement     	vm_stack[VM_STACK_LOCAL_VAR_MAX];
		 int n_globals;

		 // global vars show be initialized to stack array taking the difference (the registered variables on the main function) - global_vars ...
		 std::vector<StackElement> global_var;
		StackElement *stk_current;


		 StackElement  callFunction_C(
			 intptr_t fun_ptr,
			 const ScriptFunction *calling_function,
			 bool & error,
			 StackElement *stk_arg_calling_function,
			 unsigned char n_args,
			 Instruction *ins,
			 ScriptVar  * this_object
		);

		/**
		 * Reserve for N vars. Return base pointer.
		 */

		//const char * toString(StackElement * index);
		inline void  removeEmptySharedPointers(int idx_stk_current,void *ptr_callc_result);
		//std::string  convertStackElementVarTypeToStr(StackElement stk_v)

		inline ScriptFunction *  findFunction(
			ScriptVar *calling_object
			,ScriptFunction *info_function
			,Instruction *instruction
			,Instruction * call_ale_instruction

			,std::vector<FunctionSymbol> *function_symbol
			,std::vector<ScriptFunction *> *global_functions

			,bool is_constructor
			,const std::string & symbol_to_find


			,StackElement *stk_result_op1
			,StackElement *stk_result_op2
			,StackElement *start_arg
			,unsigned char n_args
			,const char * metamethod_str
		);

		//inline bool assignStackVar(StackElement *dst_ins, StackElement *src_ins,Instruction *instruction);


		inline bool popVmScope(int idx_stack,void * ptr_callc_result, unsigned char properties);

		inline bool applyMetamethod(
			ScriptVar *calling_object
			,ScriptFunction *info_function
			,Instruction *instruction
			,const char *op_code_str
			,ByteCodeMetamethod op_code_methamethod
			,StackElement *stk_result_op1
			,StackElement *stk_result_op2

		);

		void 				doStackDump();

	};

}


#include "VirtualMachine.inc"
