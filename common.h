/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once


#ifdef __DEBUG__ // incoment __ZETSCRIPT_VERBOSE_MESSAGE__ to print all messages (wrning is going to be slow because of the prints)
//#define __ZETSCRIPT_VERBOSE_MESSAGE__
#endif

#define ZS_IDX_UNDEFINED 						-1
#define MAX_NATIVE_FUNCTION_ARGS				6

#define DEFAULT_NO_FILENAME						"no_file"
//#define FUNCTION_MEMBER_CONSTRUCTOR_NAME 		"constructor"
//#define ZS_CONTRUCTOR_NAME						FUNCTION_MEMBER_CONSTRUCTOR_NAME

// HELPER FUNCTIONSs
#define REGISTER_NATIVE_BASE_SYMBOLS(script_class_factory,o)		   			(zs)->registerNativeBaseSymbols(o))

// if 0 is in main <> 0, else.
#define DO_CAST																	((this->zs))->doCast
#define GET_IDX_BUILTIN_TYPE_FROM_ITS_C_TYPE								((this->zs))->getIdxClassFromItsNativeType
#define INSTANCE_SCRIPT_VARIABLE_BY_IDX											((this->zs))->instanceScriptObjectiableByIdx
#define GET_METAMETHOD(m)														((this->zs))->getMetamethod(m)
#define IS_IDX_BUILTIN_TYPE_INSTANCEOF(zs,idx_class,idx_class_instance)	((this->script_class_factory))->isClassInheritsFrom(idx_class,idx_class_instance)

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif


namespace zetscript{

	class Scope;
	class ScriptObject;
	class ScriptClass;
	class ScriptFunction;
	class ScriptObjectClass;
	//typedef unsigned char 									ClassTypeIdx;
	typedef intptr_t zs_int;

	typedef enum:int{

		// !!! HERE WE DEFINE THE BUILTINT CLASSES
		// !!! WARNING THE REGISTER HAS TO RESPECT THE SAME ORDER
		// built-in types...
		IDX_BUILTIN_TYPE_MAIN = 0, 	// Main class ...

		// built in C types...
		IDX_BUILTIN_TYPE_VOID_C,

		IDX_BUILTIN_TYPE_ZS_INT_C,
		IDX_BUILTIN_TYPE_ZS_INT_PTR_C,
		IDX_BUILTIN_TYPE_CONST_CHAR_PTR_C,
		IDX_BUILTIN_TYPE_STRING_PTR_C,
		IDX_BUILTIN_TYPE_BOOL_C,
		IDX_BUILTIN_TYPE_BOOL_PTR_C,
		IDX_BUILTIN_TYPE_FLOAT_C,
		IDX_BUILTIN_TYPE_FLOAT_PTR_C,
		IDX_BUILTIN_TYPE_CONST_FLOAT_PTR_C,

		IDX_BUILTIN_TYPE_STACK_ELEMENT,
		IDX_BUILTIN_TYPE_ZETSCRIPT,
		IDX_BUILTIN_TYPE_FUNCTION, // function a(){}
		IDX_BUILTIN_TYPE_SCRIPT_OBJECT_VAR_REF,
		IDX_BUILTIN_TYPE_SCRIPT_OBJECT_OBJECT, //  {}
		IDX_BUILTIN_TYPE_SCRIPT_OBJECT_STRING, // "string"
		IDX_BUILTIN_TYPE_SCRIPT_OBJECT_VECTOR, // vector []
		IDX_BUILTIN_TYPE_SCRIPT_OBJECT_CLASS, // class A{}
		// !!!
		// !!! DO NOT DECLARE NOTHING HERE!! RESERVED FOR USER CLASSES !!!!!
		// !!!
		IDX_BUILTIN_TYPE_MAX
	}IdxBuiltInType;


	typedef void  (* PrintFunctionCallback)(const char *filename, int line, const  char  *string_text);

	typedef zs_int (*ConversionType)(zs_int);
	//-----------------------------


	//-----------------------------
	#pragma pack(push, 1)

	struct _InfoSharedList;
	//struct EvalInstruction;

	typedef struct _SharedPointerInfo {
		ScriptObject 			*ptr_script_object_shared;
		unsigned short 			n_shares;
		_InfoSharedList *zero_shares;
	} SharedPointerInfo;

	typedef struct _InfoSharedPointerNode{
		SharedPointerInfo data;
	//		unsigned short currentStack;
		_InfoSharedPointerNode *previous, *next;
	} InfoSharedPointerNode;

	typedef struct _InfoSharedList{
		InfoSharedPointerNode *first, *last;
	}InfoSharedList;

	struct FunctionArg{
		int idx_type;
		std::string arg_name; //arg c++ type or arg name
		int line;
		bool by_ref;
		bool var_args;
		StackElement default_var_value; // constant int/float/bool/string or anonymous function that return an expression or other object

		FunctionArg(){
			idx_type=ZS_IDX_UNDEFINED;
			arg_name="";
			line=ZS_IDX_UNDEFINED;
			by_ref=false;
			var_args=false;
			default_var_value=stk_undefined;
		}

		FunctionArg(int _idx_type, std::string _arg_name){
			idx_type=_idx_type;
			arg_name=_arg_name;
			line=ZS_IDX_UNDEFINED;
			by_ref=false;
			var_args=false;
			default_var_value=stk_undefined;
		}

		FunctionArg( const FunctionArg & _function_param){
			idx_type=_function_param.idx_type;
			arg_name=_function_param.arg_name;
			line=_function_param.line;
			by_ref=_function_param.by_ref;
			var_args=_function_param.var_args;
			default_var_value=_function_param.default_var_value;
		}
	};

	struct FunctionMember{
	public:
		ScriptFunction 		*so_function; // make function pointer first to make compatible with stk
		ScriptObjectClass	*so_object;

		FunctionMember(
				ScriptObjectClass	*_so_object
				,ScriptFunction 	*_so_function){
			so_object = _so_object;
			so_function= _so_function;
		}
	};

	#pragma pack(pop)


}
