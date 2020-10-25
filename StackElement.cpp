#include "zetscript.h"

namespace zetscript{

	const char * StackElement::toString(){
		StackElement *stk=this;
		const char * result="undefined";
		if(STK_VALUE_IS_ZS_INT(stk))
			result= "Integer";
		else if(STK_VALUE_IS_FLOAT(stk))
			result= "Float";
		else if(STK_VALUE_IS_BOOLEAN(stk))
			result= "Boolean";
		else if(STK_VALUE_IS_STRING(stk))
			result= "String";
		else if(STK_VALUE_IS_FUNCTION(stk))
			result= "Function";

		else if(STK_VALUE_IS_SCRIPT_VAR(stk)){

			if(this->properties & MSK_STACK_ELEMENT_PROPERTY_PTR_STK){
				stk=(StackElement *)stk->var_ref;
			}
			result=((ScriptObject *)stk->var_ref)->getClassName().c_str();
		}

		return result;
	}


	/*inline	std::string convertStackElementVarTypeToStr(StackElement stk_v){
			if(stk_v.properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_ZS_INT){
				return zs_rtti::demangle(typeid(int).name());
			}else if(stk_v.properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT){
				return zs_rtti::demangle(typeid(float).name());
			}else if(stk_v.properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOL){
				return zs_rtti::demangle(typeid(bool).name());
			}else if(stk_v.properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING){
				return zs_rtti::demangle(typeid(std::string).name());
			}else if(stk_v.properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPT_OBJECT){

				ScriptClass *c = GET_SCRIPT_CLASS(this,((ScriptObject *)(stk_v.var_ref))->idx_class);

				if(c!=NULL){
					return zs_rtti::demangle(c->str_class_ptr_type);
				}
			}
			return "unknow";
		}*/

}
