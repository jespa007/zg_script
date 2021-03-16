/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_NEW_OBJECT_VECTOR(zs) (ScriptObjectVector::newScriptObjectVector(zs)) //ScriptClass::getInstance()->getRegisteredClassByIdx(ScriptClass::getInstance()->getIdxClassVector())))//,(void *)(new ScriptObjectVector())))

namespace zetscript{

	class CZetgine;
	class  ScriptObjectVector: public ScriptObject{
	public:

		//----------------------------------------------
		//
		// Helpers
		//
		static ScriptObjectVector * newScriptObjectVector(ZetScript *zs);
		static ScriptObjectVector * newScriptObjectVectorAdd(ZetScript *zs,ScriptObjectVector *v1,ScriptObjectVector *v2);

		template<typename T>
		static std::vector<T> 	toStdVector(ScriptObjectVector *sv);
		static void    			pushSf(ScriptObjectVector *sv,StackElement  * stk);
		static void   			popSf(ScriptObjectVector *sv);
		static zs_int 			sizeSf(ScriptObjectVector *sv);
		static void 			insertAtSf(ScriptObjectVector *sv, zs_int idx,StackElement  * stk);
		static void 			eraseAtSf(ScriptObjectVector *sv, zs_int idx);
		static void 			clearSf(ScriptObjectVector *sv);
		static ScriptObjectString	*joinSf(ScriptObjectVector *sv, zs_int idx_char);

		//
		// Helpers
		//
		//----------------------------------------------

		ScriptObjectVector();

		virtual StackElement * 	getUserElementAt(short idx);
		bool 				eraseUserElementAt(short idx);
		void				eraseAllUserElements();

		virtual size_t length();
		virtual zs_vector * getAllUserElements();

		StackElement *			pushNewUserSlot();
		void 					push(StackElement  * stk);
		void 					pop();

		virtual ~ScriptObjectVector();
	protected:
		zs_vector 			stk_user_elements;




	};
}

#include "ScriptObjectVector.tcc"
