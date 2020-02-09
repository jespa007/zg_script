/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define NEW_VECTOR_VAR (new CScriptVarVector(this->zs)) //CScriptClass::getInstance()->getRegisteredClassByIdx(CScriptClass::getInstance()->getIdxClassVector())))//,(void *)(new CScriptVarVector())))

namespace zetscript{

	class CZetgine;
	class  CScriptVarVector: public CScriptVar{


	public:

		template<typename T>
		static std::vector<T> toStdVector(CScriptVarVector *v_in){
			std::vector<T> v_out;
			const char * dst_convert_type = typeid(T).name();
			float aux_flt;
			std::vector<StackElement> * variable = v_in->getVectorVariable();

			if(v_in){
				for(unsigned i = 0; i < variable->size(); i++){

					StackElement sv=variable->at(i);

					switch(sv.properties & MASK_VAR_PRIMITIVE_TYPES)
					{
						default:
						case STK_PROPERTY_TYPE_UNDEFINED:
						case STK_PROPERTY_TYPE_NULL:
						case STK_PROPERTY_TYPE_NUMBER:
							if(ZS_STRCMP(dst_convert_type, ==,typeid(float).name())){
								memcpy(&aux_flt, &sv.stkValue, sizeof(float));
								v_out.push_back(aux_flt);
							}else if(ZS_STRCMP(dst_convert_type, ==,typeid(int).name())){
								v_out.push_back((intptr_t)sv.stkValue);
							}else{
								THROW_RUNTIME_ERROR("Error trying to cast element on std::vector<float>");
								return v_out;
							}
							break;
						case STK_PROPERTY_TYPE_BOOLEAN:
						case STK_PROPERTY_TYPE_STRING:
						case STK_PROPERTY_TYPE_FUNCTION:
							THROW_RUNTIME_ERROR("type not implemented yet");
							return v_out;
							break;
						case STK_PROPERTY_TYPE_INTEGER:
							if(ZS_STRCMP(dst_convert_type, ==,typeid(int).name()) || ZS_STRCMP(dst_convert_type, ==,typeid(float).name())){// typeid(int).name()) || ){
								v_out.push_back((intptr_t)sv.stkValue);
							}else{
								THROW_RUNTIME_ERROR("Error trying to cast element on std::vector<int>");
								return v_out;
							}
							break;
					}
				}
			}
			return v_out;
		}



		StackElement return_callc;
		//int _i_size;

		//std::vector<StackElement> m_objVector;

		CScriptVarVector(){}
		CScriptVarVector(CZetScript *_zs);

		virtual bool unrefSharedPtr();


		//StackElement 		  * getValue(const std::string & s);
		//virtual StackElement * getValue(int idx);


		StackElement * push();
		void 			push(StackElement  * v);
		StackElement * pop();

		 int size();

		virtual void destroy();

		virtual ~CScriptVarVector();


	};

}