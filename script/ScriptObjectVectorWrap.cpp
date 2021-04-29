/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "zetscript.h"

namespace zetscript{

	void    			ScriptObjectVectorWrap_push(ScriptObjectVector *sv,StackElement  * stk){
		return sv->push(stk);
	}

	void   	ScriptObjectVectorWrap_pop(ScriptObjectVector *sv){
		sv->pop();
	}

	zs_int 			ScriptObjectVectorWrap_size(ScriptObjectVector *sv){
		return sv->length();
	}

	void 			ScriptObjectVectorWrap_insertAt(ScriptObjectVector *sv, zs_int idx,StackElement  * _stk){

		StackElement *new_stk=(StackElement *)malloc(sizeof(StackElement));
		*new_stk=*_stk;
		ZetScript *zs=sv->getZetScript();
		zs_vector *stk_user_list_elements=sv->getStkUserListElements();

		// update n_refs +1
		if(_stk->properties&MSK_STK_PROPERTY_SCRIPT_OBJECT){
			ScriptObject *so_param=(ScriptObject *)_stk->stk_value;
			if(so_param->idx_script_class == IDX_BUILTIN_TYPE_SCRIPT_OBJECT_STRING && so_param->shared_pointer==NULL){
				//STK_IS_SCRIPT_OBJECT_STRING(stk_arg)){ // remove
				ScriptObjectString *sc=ZS_NEW_OBJECT_STRING(zs);
				if(!vm_create_shared_pointer(zs->getVirtualMachine(),sc)){
					return;
				}
				sc->set(so_param->toString());
				so_param=sc;
				new_stk->stk_value=so_param; // update pointer
			}

			vm_share_pointer(zs->getVirtualMachine(),so_param);

		}

		stk_user_list_elements->insert(idx,(zs_int)new_stk);
	}

	void 			ScriptObjectVectorWrap_eraseAt(ScriptObjectVector *sv, zs_int idx){
		sv->eraseUserElementAt(idx);
	}

	void 			ScriptObjectVectorWrap_clear(ScriptObjectVector *sv){
		sv->eraseAllUserElements();
	}

	ScriptObjectString *		ScriptObjectVectorWrap_join(ScriptObjectVector *sv, zs_int idx){
		ScriptObjectString *so_ref=NULL;
		//std::string *str;
		ScriptObjectString *so_string = ZS_NEW_OBJECT_STRING(sv->getZetScript());
		std::string *ptr_str=(std::string *)so_string->value;
		zs_vector *stk_user_list_elements=sv->getStkUserListElements();

		for(unsigned i=0; i < stk_user_list_elements->count;i++){
			StackElement *stk=(StackElement *)stk_user_list_elements->items[i];
			if(i>0){
				ptr_str->push_back(idx);
			}
			*ptr_str+=stk->toString();
		}

		return so_string;
	}
}
