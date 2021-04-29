/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{


	class  ScriptObjectStringIterator:public ScriptObject{

	public:

		ScriptObjectStringIterator();

		StackElement get();
		void next();
		bool end();

	};

}
