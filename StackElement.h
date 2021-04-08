/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

// properties shared by compiler + VM
typedef enum:unsigned char {
	//-- COMPILER/VM TYPE VAR
	BIT_STK_PROPERTY_ZS_CHAR,								// 0x0001
	BIT_STK_PROPERTY_ZS_INT,								// 0x0002
	BIT_STK_PROPERTY_ZS_FLOAT,								// 0x0004
	BIT_STK_PROPERTY_BOOL,									// 0x0008
	BIT_STK_PROPERTY_FUNCTION,								// 0x0010
	BIT_STK_PROPERTY_MEMBER_FUNCTION,						// 0x0020
	BIT_STK_PROPERTY_MEMBER_ATTRIBUTE,						// 0x0040
	BIT_STK_PROPERTY_CLASS,									// 0x0080
	BIT_STK_PROPERTY_SCRIPT_OBJECT,							// 0x0100
	BIT_STK_PROPERTY_MAX,
	//-- VM RUNTIME
	BIT_STK_PROPERTY_IS_VAR_C = BIT_STK_PROPERTY_MAX, 		// 0x0200 ptr to C
	BIT_STK_PROPERTY_PTR_STK,	 							// 0x0400
	BIT_STK_PROPERTY_READ_ONLY,				 				// 0x0800
	MAX_BIT_RUNTIME

}StkProperty;



#define		MSK_STK_PROPERTY_NULL 			0 //(0x1 << BIT_STK_PROPERTY_ZS_UNDEFINED) // let's say a variable not defined...
#define 	MSK_STK_PROPERTY_ZS_CHAR 			(0x1 << BIT_STK_PROPERTY_ZS_CHAR)
#define		MSK_STK_PROPERTY_ZS_INT 			(0x1 << BIT_STK_PROPERTY_ZS_INT) // primitive int
#define		MSK_STK_PROPERTY_ZS_FLOAT			(0x1 << BIT_STK_PROPERTY_ZS_FLOAT) // primitive number
#define		MSK_STK_PROPERTY_BOOL				(0x1 << BIT_STK_PROPERTY_BOOL) // primitive bool
#define		MSK_STK_PROPERTY_FUNCTION			(0x1 << BIT_STK_PROPERTY_FUNCTION) // primitive function
#define		MSK_STK_PROPERTY_MEMBER_FUNCTION 	(0x1 << BIT_STK_PROPERTY_MEMBER_FUNCTION) // function member
#define		MSK_STK_PROPERTY_MEMBER_ATTRIBUTE 	(0x1 << BIT_STK_PROPERTY_MEMBER_ATTRIBUTE) // attribute member
#define		MSK_STK_PROPERTY_CLASS				(0x1 << BIT_STK_PROPERTY_CLASS) // primitive function
#define		MSK_STK_PROPERTY_SCRIPT_OBJECT		(0x1 << BIT_STK_PROPERTY_SCRIPT_OBJECT) // always is an script class...

#define IS_STK_FUNCTION(stk)	((stk->properties & MSK_STK_PROPERTY_FUNCTION) || (stk->properties & MSK_STK_PROPERTY_MEMBER_FUNCTION))

#define MSK_STK_PROPERTY_TYPE_PRIMITIVES					((0x1<<BIT_STK_PROPERTY_FUNCTION)-1)
#define GET_STK_PROPERTY_PRIMITIVE_TYPES(prop)				((prop)&MSK_STK_PROPERTY_TYPE_PRIMITIVES)

#define MSK_STK_PROPERTY_TYPES								((0x1<<BIT_STK_PROPERTY_MAX)-1)
#define GET_MSK_STK_PROPERTY_TYPES(prop)					((prop)&MSK_STK_PROPERTY_TYPES)

#define STK_IS_SCRIPT_OBJECT_OBJECT(stk) 					(((stk)->properties & MSK_STK_PROPERTY_SCRIPT_OBJECT) && (((ScriptObject *)(stk)->stk_value)->idx_script_class==IDX_BUILTIN_TYPE_SCRIPT_OBJECT_OBJECT))
#define STK_IS_SCRIPT_OBJECT_STRING(stk) 					(((stk)->properties & MSK_STK_PROPERTY_SCRIPT_OBJECT) && (((ScriptObject *)(stk)->stk_value)->idx_script_class==IDX_BUILTIN_TYPE_SCRIPT_OBJECT_STRING))
#define STK_IS_SCRIPT_OBJECT_VECTOR(stk) 					(((stk)->properties & MSK_STK_PROPERTY_SCRIPT_OBJECT) && (((ScriptObject *)(stk)->stk_value)->idx_script_class==IDX_BUILTIN_TYPE_SCRIPT_OBJECT_VECTOR))
#define STK_IS_SCRIPT_OBJECT_CLASS(stk) 					(((stk)->properties & MSK_STK_PROPERTY_SCRIPT_OBJECT) && (((ScriptObject *)(stk)->stk_value)->idx_script_class>IDX_BUILTIN_TYPE_SCRIPT_OBJECT_CLASS))
#define STK_IS_SCRIPT_OBJECT_VAR_REF(stk) 					(((stk)->properties & MSK_STK_PROPERTY_SCRIPT_OBJECT) && (((ScriptObject *)(stk)->stk_value)->idx_script_class==IDX_BUILTIN_TYPE_SCRIPT_OBJECT_VAR_REF))
#define STK_GET_STK_VAR_REF(stk)  							(((ScriptObjectVarRef *)((stk))->stk_value)->getStackElementPtr())



enum:unsigned short {
	MSK_STK_PROPERTY_IS_VAR_C = (0x1 << BIT_STK_PROPERTY_IS_VAR_C),
	MSK_STK_PROPERTY_PTR_STK = (0x1 << BIT_STK_PROPERTY_PTR_STK),
	MSK_STK_PROPERTY_READ_ONLY = (0x1 << BIT_STK_PROPERTY_READ_ONLY),
	//MSK_STK_PROPERTY_ARRAY_STK = (0x1 << BIT_STK_PROPERTY_ARRAY_STK),
	//MSK_STK_PROPERTY_CONST_CHAR = 0xffff 	// special use const char
	//MSK_STK_PROPERTY_RETURN = (0x1 << BIT_STK_PROPERTY_RETURN)
};

#define MSK_STK_PROPERTY_ZS_INT_PTR (MSK_STK_PROPERTY_IS_VAR_C | MSK_STK_PROPERTY_ZS_INT)
#define MSK_STK_PROPERTY_FLOAT_PTR (MSK_STK_PROPERTY_IS_VAR_C | MSK_STK_PROPERTY_ZS_FLOAT)
#define MSK_STK_PROPERTY_BOOL_PTR (MSK_STK_PROPERTY_IS_VAR_C | MSK_STK_PROPERTY_BOOL)

#define MSK_STK_PROPERTY_RUNTIME					(((0x1<<(MAX_BIT_RUNTIME-BIT_STK_PROPERTY_IS_VAR_C))-1)<<(BIT_STK_PROPERTY_IS_VAR_C))
#define GET_MSK_STK_PROPERTY_RUNTIME(prop)			((prop)&MSK_STK_PROPERTY_RUNTIME)

// Check types
#define STK_VALUE_IS_FLOAT(stk) \
(stk->properties & MSK_STK_PROPERTY_ZS_FLOAT)


#define STK_VALUE_IS_ZS_INT(stk) \
(stk->properties & MSK_STK_PROPERTY_ZS_INT)

#define STK_VALUE_IS_CHAR_PTR(stk) \
(stk->properties & STK_PROPERTY_TYPE_CHAR_PTR)

#define STK_VALUE_IS_BOOLEAN(stk) \
(stk->properties & MSK_STK_PROPERTY_BOOL)

#define STK_VALUE_IS_UNDEFINED(stk) \
(stk->properties & MSK_STK_PROPERTY_NULL)

#define STK_VALUE_IS_NULL(stk) \
(stk->properties & MSK_STK_PROPERTY_NULL)

#define STK_VALUE_IS_FUNCTION(stk) \
(stk->properties & MSK_STK_PROPERTY_FUNCTION)

#define STK_VALUE_IS_CLASS(stk) \
(stk->properties & MSK_STK_PROPERTY_CLASS)

#define STK_VALUE_IS_MEMBER_ATTRIBUTE(stk) \
(stk->properties & MSK_STK_PROPERTY_MEMBER_ATTRIBUTE)

#define STK_VALUE_IS_MEMBER_FUNCTION(stk) \
(stk->properties & MSK_STK_PROPERTY_MEMBER_FUNCTION)

#define STK_VALUE_IS_SCRIPT_VAR(stk) \
(stk->properties & MSK_STK_PROPERTY_SCRIPT_OBJECT)

#define STK_VALUE_IS_VECTOR(stk) \
(( stk->properties & MSK_STK_PROPERTY_SCRIPT_OBJECT) &&\
 (((ScriptObjectObject *)(stk->stk_value))->idx_class==IDX_BUILTIN_TYPE_SCRIPT_OBJECT_VECTOR))

#define STK_VALUE_TO_ZS_INT(ptr_result_instruction) \
		((zs_int)((zs_int)(ptr_result_instruction->stk_value)))

#define STK_VALUE_TO_BOOL(ptr_result_instruction) \
		(((bool)(ptr_result_instruction->stk_value)))

#define STK_SET_NULL(stk) memset(stk,0,sizeof(StackElement))
//#define STK_ASSIGN(dst,src) memcpy(dst,src,sizeof(StackElement))

namespace zetscript{

	#pragma pack(push, 1)

	struct StackElement {
		//VALUE_INSTRUCTION_TYPE 		type; // tells what kind of variable is. By default is object.
		void * stk_value; // operable value
		//void * var_ref; // stack ref in case to assign new value.
		uint16_t properties; // it tells its properties

		// it gives stackelement as string (the result should be deleted)
		const char * typeStr();
		std::string toString();
		void setUndefined();

	};

	#pragma pack(pop)


	extern const StackElement stk_undefined;



}
