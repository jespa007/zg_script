/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once


#ifdef __DEBUG__ // incoment __ZETSCRIPT_VERBOSE_MESSAGE__ to print all messages (wrning is going to be slow because of the prints)
#define __ZETSCRIPT_VERBOSE_MESSAGE__
#endif


#define ZS_IDX_SYMBOL_SOLVE_AT_RUNTIME		 0
#define ZS_IDX_UNDEFINED 					-1
//#define ZS_FUNCTION_NOT_FOUND_IDX	 		-2
#define ZS_IDX_SYMBOL_THIS					-3

#define MAX_N_ARGS						 6
#define NO_PARAMS_IS_VARIABLE			-1
//#define NO_PARAMS_SYMBOL_ONLY			-2
#define NO_PARAMS_IS_CLASS				-2
#define DEFAULT_NO_FILENAME				"no_file"

// HELPER FUNCTIONS

#define REGISTER_C_BASE_SYMBOLS(script_class_factory,o)		   			(zs)->register_C_BaseSymbols(o))

// if 0 is in main <> 0, else.
#define SCOPE_IN_MAIN_CLASS(idx)						((this->scope_factory)->getScope(idx)->getIdxScopeBase()==IDX_GLOBAL_SCOPE)

#define DO_CAST											((this->zs))->doCast
#define GET_IDX_BUILTIN_TYPE_CLASS_FROM_ITS_C_TYPE		((this->zs))->getIdxClassFromIts_C_Type
#define INSTANCE_SCRIPT_VARIABLE_BY_IDX					((this->zs))->instanceScriptVariableByIdx
#define GET_METAMETHOD(m)								((this->zs))->getMetamethod(m)
#define IS_IDX_BUILTIN_TYPE_CLASS_INSTANCEOF(zs,_class,_instance)	((this->script_class_factory))->class_C_BaseOf(_class,_instance)




namespace zetscript{

	//typedef CASTNode *PASTNode;
	class ScriptFunction;
	class Scope;
	class ScriptClass;
	class ScriptVar;
	struct Symbol;

	struct ScopeBlockVars;


	typedef enum
		:unsigned char {

		LOAD_TYPE_NOT_DEFINED = 0,
		LOAD_TYPE_NULL,
		LOAD_TYPE_UNDEFINED,
		LOAD_TYPE_CONSTANT,
		LOAD_TYPE_VARIABLE,
		LOAD_TYPE_FUNCTION,
		LOAD_TYPE_ARGUMENT
	}LoadType;



	//#define MSK_INSTRUCTION_PROPERTY_CALL_TYPE					(((0x1<<(BIT_INSTRUCTION_PROPERTY_MAX-INS_PROPERTY_CALLING_OBJECT))-1)<<(INS_PROPERTY_CALLING_OBJECT))
	//#define GET_MSK_INSTRUCTION_PROPERTY_CALL_TYPE(prop)		((prop)&MSK_INSTRUCTION_PROPERTY_CALL_TYPE)

	#define MAIN_SCRIPT_FUNCTION_NAME 			"__MainFunction__"

	typedef enum {

		// built-in types...
		IDX_BUILTIN_TYPE_CLASS_MAIN = 0, 	// Main class ...

		// built in C types...
		IDX_BUILTIN_TYPE_VOID_C,
		IDX_BUILTIN_TYPE_INT_PTR_C,
		IDX_BUILTIN_TYPE_FLOAT_PTR_C,
		IDX_BUILTIN_TYPE_CONST_CHAR_PTR_C,
		IDX_BUILTIN_TYPE_STRING_PTR_C,
		IDX_BUILTIN_TYPE_BOOL_PTR_C,
		IDX_BUILTIN_TYPE_INT_C,
		IDX_BUILTIN_TYPE_UNSIGNED_INT_C,
		IDX_BUILTIN_TYPE_INTPTR_T_C,
		IDX_BUILTIN_TYPE_FLOAT_C,
		IDX_BUILTIN_TYPE_BOOL_C,

		// built in classes...
		IDX_BUILTIN_TYPE_STACK_ELEMENT,
		IDX_BUILTIN_TYPE_CLASS_SCRIPT_VAR,
		IDX_BUILTIN_TYPE_CLASS_STRING,
		IDX_BUILTIN_TYPE_CLASS_VECTOR,
		IDX_BUILTIN_TYPE_CLASS_FUNCTOR,
		IDX_BUILTIN_TYPE_CLASS_DICTIONARY,

		IDX_BUILTIN_TYPE_MAX
	}IdxBuiltInType;

	//typedef tInfoStatementOp *PInfoStatementOp;
	typedef enum {
		SYMBOL_INFO_PROPERTY_C_OBJECT_REF 					= 0x1 << 0,
		SYMBOL_INFO_PROPERTY_IS_DERIVATED 					= 0x1 << 1,
		SYMBOL_INFO_PROPERTY_STATIC_REF 					= 0x1 << 2, // C function or C++ static functions
		SYMBOL_INFO_PROPERTY_IS_POLYMORPHIC					= 0x1 << 3,
		SYMBOL_INFO_PROPERTY_SET_FIRST_PARAMETER_AS_THIS	= 0x1 << 4  // will pass object this as first parameter
	}SymbolInfoProperty;


	typedef enum
		:unsigned char{
		 SCOPE_PROPERTY_BREAK=0x1 << 0
		,SCOPE_PROPERTY_CONTINUE=0x1 << 1
		,SCOPE_PROPERTY_FOR_IN=0x1 << 2

	}ScopeProperty;


	typedef void  (* PrintFunctionCallback)(const char *filename, int line, const  char  *string_text);

	typedef intptr_t (*ConversionType)(intptr_t);



	//-----------------------------

	struct Symbol {
		//public:
		std::string file;
		short line;
		short idx_scope;

		std::string name;

		char n_params;

		Symbol(){
			file="";
			line=-1;

			idx_scope = ZS_IDX_UNDEFINED;
			name="";
			n_params = NO_PARAMS_IS_VARIABLE;
		}

		/*Symbol(const std::string & _name, char _n_params= NO_PARAMS_IS_VARIABLE){
			file=ZS_IDX_UNDEFINED;
			line=-1;
			idx_scope = ZS_IDX_UNDEFINED;

			name=_name;
			n_params=_n_params;
		}*/

		bool operator == (const Symbol & s1){
			return this->name == s1.name
				  && this->idx_scope == s1.idx_scope
				  && this->n_params == s1.n_params;
		}

		bool isFunction(){
			return n_params != NO_PARAMS_IS_VARIABLE;
		}

		bool matchSymbol(const Symbol & s1){
			return this->name == s1.name
				  && this->n_params == s1.n_params;
		}
	};

	//-----------------------------


	#pragma pack(push, 1)




	struct LinkSymbolFirstAccess{

		short idx_script_function;
		short idx_scope;
		std::string value;
		char n_params;

		LinkSymbolFirstAccess(){

			idx_script_function=ZS_IDX_UNDEFINED;
			idx_scope=ZS_IDX_UNDEFINED;
			value = "";
			n_params=ZS_IDX_UNDEFINED;
		}

		LinkSymbolFirstAccess(
				 int _idx_script_function
				,short _idx_scope
				,const std::string & _value
				,char _n_params=0
				){
			idx_script_function=_idx_script_function;
			idx_scope=_idx_scope;
			value=_value;
			n_params=_n_params;
		}
	};





	//-------------------------------------------------------


	/**
	 * Scope register
	 */
	struct ScopeBlockVars {
		int *		idx_local_var;
		uint8_t 	n_local_vars;
		short 		idx_scope;
	};

	typedef struct _SharedPointerInfo *PInfoSharedPointer;

	typedef struct _SharedPointerInfo {
		ScriptVar *shared_ptr;
		unsigned char n_shares;

		//short idx_0_shares;
		//PInfoSharedPointer next;
	} SharedPointerInfo;

	typedef struct _tNode * PInfoSharedPointerNode;
	typedef struct _tNode {
		SharedPointerInfo data;
		unsigned short currentStack;
		PInfoSharedPointerNode previous, next;
	} tInfoSharedPointerNode;

	typedef struct{
		int idx_type;
		std::string arg_name; //arg c++ type or arg name
	}ParamArgInfo;

	#pragma pack(pop)


}
