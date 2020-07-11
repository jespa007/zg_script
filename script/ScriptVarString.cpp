/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "ZetScript.h"

namespace zetscript{

	void ScriptVarString::setup(){
		value = &str_value;
		str_value = "";


	}

	ScriptVarString::ScriptVarString(ZetScript *_zs):ScriptVar(_zs){
		str_value = "";
		Init(SCRIPT_CLASS_STRING(this), (void *)this);
	}

	ScriptVarString::ScriptVarString(ZetScript *_zs, const std::string & s):ScriptVar(_zs){


		Init(SCRIPT_CLASS_STRING(this), (void *)this);
		str_value = s;
	}

}