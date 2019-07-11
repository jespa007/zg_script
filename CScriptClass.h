/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once



/*
//	______           _     _                    _   _____ _
//	| ___ \         (_)   | |                  | | /  __ \ |
//	| |_/ /___  __ _ _ ___| |_ ___ _ __ ___  __| | | /  \/ | __ _ ___ ___
//	|    // _ \/ _` | / __| __/ _ \ '__/ _ \/ _` | | |   | |/ _` / __/ __|
//	| |\ \  __/ (_| | \__ \ ||  __/ | |  __/ (_| | | \__/\ | (_| \__ \__ \
//	\_| \_\___|\__, |_|___/\__\___|_|  \___|\__,_|  \____/_|\__,_|___/___/
//            __/ |
//           |___/
// _________________________________________________
//  __________________________________
*/



namespace zetscript{

/**
 * Stores the basic information to build a object through built AST structure
 */

	class  CScriptClass:public CScriptClassBase{

	public:

		//------------- VARIABLES STRUCT ---------------
		char  idx_function_member_constructor;

		std::function<void * ()> 			* 	c_constructor;
		std::function<void (void *  p)> 	*	c_destructor;
		std::string classPtrType; // type_id().name();
		std::vector<unsigned char > idxBaseClass;

		std::vector<CScriptFunction * > metamethod_operator[MAX_METAMETHOD_OPERATORS]; // overrided metamethod

		 CScriptClass(unsigned char _idxClass);

		virtual CScriptFunction						*  			registerFunctionMember(const std::string & file, short line,const std::string & function_name, std::vector<tArgumentInfo> args={}, int idx_return_type=ZS_UNDEFINED_IDX,intptr_t ref_ptr=0, unsigned short properties=0);

		bool isToClass();

		virtual ~CScriptClass();
	};
}


