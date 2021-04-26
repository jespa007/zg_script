#include "zetscript.h"

#ifdef _MSC_VER
#include <conio.h>
#define getch _getch
#endif

namespace zetscript{

	zs_int ModuleSystem::clock(){
		return std::clock()*(1000.0f/CLOCKS_PER_SEC);
	}

	void ModuleSystem::makeReadOnly(StackElement *stk){
		stk->properties|=MSK_STK_PROPERTY_READ_ONLY;
	}


	void ModuleSystem::eval(ZetScript *zs,StackElement *str, StackElement *args){
		ScriptObjectString *str_out=ScriptObjectString::formatSf(zs,str,args);
		printf("not implemented");
		delete str_out;
	}

	void ModuleSystem::assert(ZetScript *zs,bool *chk_assert, StackElement *str, StackElement *args){
		if(*chk_assert == false){
			ScriptObjectString *str_out=ScriptObjectString::formatSf(zs,str,args);
			vm_set_error(zs->getVirtualMachine(),str_out->toString().c_str());
			delete str_out;
		}
	}
}

