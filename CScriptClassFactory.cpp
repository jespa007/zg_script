#include "CZetScript.h"

namespace zetscript{

	#define FORMAT_PRINT_INSTRUCTION "%04i"

	#define REGISTER_BUILT_IN_CLASS(type_class, idx_class)\
		if(vec_script_class_node.size()!=idx_class){\
			THROW_RUNTIME_ERROR("Error: class built in type %s doesn't match its id",STR(type_class));\
			return;\
		}\
		register_C_ClassInt<type_class>(STR(type_class));


	#define REGISTER_BUILT_IN_TYPE(type_class, idx_class)\
		if(vec_script_class_node.size()!=idx_class){\
			THROW_RUNTIME_ERROR("Error initializing C built in type: %s",STR(type_class));\
			return;\
		}else{\
			registerClass(STR(type_class),"");\
			vec_script_class_node.at(idx_class)->symbol_info.properties=SYMBOL_INFO_PROPERTY::PROPERTY_C_OBJECT_REF;\
			vec_script_class_node.at(idx_class)->classPtrType=(typeid(void).name());\
		}

	CScriptClassFactory * CScriptClassFactory::script_class_factory=NULL;


	CScriptClassFactory * CScriptClassFactory::getInstance(){
		if(script_class_factory ==NULL){
			script_class_factory = new CScriptClassFactory();
		}

		return script_class_factory;
	}

	void CScriptClassFactory::destroySingleton(){

		if(script_class_factory !=NULL){
			delete script_class_factory;
			script_class_factory = NULL;
		}
	}

	tStackElement CScriptClassFactory::C_REF_InfoVariable_2_StackElement(tVariableSymbolInfo *ir_var, void *ptr_variable){

		if(ir_var->properties & PROPERTY_C_OBJECT_REF){

			if(INT_PTR_TYPE_STR==ir_var->c_type){
				return {
						STK_PROPERTY_TYPE_INTEGER|STK_PROPERTY_IS_C_VAR,
						0,
						ptr_variable
				};

			}else if(FLOAT_PTR_TYPE_STR==ir_var->c_type){
				return {
						STK_PROPERTY_TYPE_NUMBER|STK_PROPERTY_IS_C_VAR,
						0,
						ptr_variable
				};
			}else if(CONST_CHAR_PTR_TYPE_STR==ir_var->c_type){

				return {
						STK_PROPERTY_TYPE_STRING,
						ptr_variable,
						0
				};
			}else if(STRING_PTR_TYPE_STR==ir_var->c_type){

				return {
						STK_PROPERTY_TYPE_STRING|STK_PROPERTY_IS_C_VAR,
						(void *)((string *)ptr_variable)->c_str(),
						ptr_variable
				};
			}else if(BOOL_PTR_TYPE_STR==ir_var->c_type){
				return {
						STK_PROPERTY_TYPE_BOOLEAN|STK_PROPERTY_IS_C_VAR,
						0,
						ptr_variable
				};
			}else{
				CScriptClass *info_registered_class = GET_SCRIPT_CLASS_INFO_BY_C_PTR_NAME(ir_var->c_type);//  CScriptClass::getInstance()->getRegisteredClassBy_C_ClassPtr(ir_var->c_type);

				if(info_registered_class){
					CScriptVariable *var = new CScriptVariable();
					var->init(info_registered_class,ptr_variable);

					return{
							STK_PROPERTY_TYPE_SCRIPTVAR|STK_PROPERTY_IS_C_VAR,
							NULL,
							var
					};
				}
		}
		}else{
			THROW_RUNTIME_ERROR("Variable %s is not c referenced as C symbol",ir_var->symbol_ref.c_str());
		}

		return{STK_PROPERTY_TYPE_UNDEFINED,
			0,
			NULL};
	}



	//----------------------------------------------------------------------------------------------------------------------------------------------------------------
	 // PRINT ASM INFO


	 string CScriptClassFactory::getStrTypeLoadValue(PtrInstruction m_listStatements, int current_instruction){

		 char print_aux_load_value[512] = {0};
		 char object_access[512] = "";

		 tInstruction * iao =&m_listStatements[current_instruction];
		 tInfoConstantValue *icv;
		 string symbol_value="????";
		 if(iao->op_code != LOAD){
			 return "ERROR";
		 }



		 sprintf(print_aux_load_value,"UNDEFINED");

		 if(iao->instruction_properties & INS_PROPERTY_ACCESS_SCOPE){

			 sprintf(object_access,
					"[" FORMAT_PRINT_INSTRUCTION "]."

					,(int)iao->index_op2);
		 }
		 else if(iao->instruction_properties & INS_PROPERTY_THIS_SCOPE){
			sprintf(object_access,"this.");
		 }

		 switch(iao->index_op1){

			case LOAD_TYPE::LOAD_TYPE_CONSTANT:
				icv=(tInfoConstantValue *)iao->index_op2;
				switch(icv->properties){
				case STK_PROPERTY_TYPE_BOOLEAN:
				case STK_PROPERTY_TYPE_INTEGER:
					sprintf(print_aux_load_value,"CONST(%i)",(int)((intptr_t)icv->stkValue));
					break;
				case STK_PROPERTY_TYPE_NUMBER:
					sprintf(print_aux_load_value,"CONST(%f)",*((float *)&icv->stkValue));
					break;
				case STK_PROPERTY_TYPE_STRING:
					sprintf(print_aux_load_value,"CONST(%s)",((string *)icv->stkValue)->c_str());
					break;

				}
				break;

			case LOAD_TYPE::LOAD_TYPE_VARIABLE:
				sprintf(print_aux_load_value,"%sVAR(%s)",object_access,symbol_value.c_str());
				break;
			case LOAD_TYPE::LOAD_TYPE_FUNCTION:

				sprintf(print_aux_load_value,"%sFUN(%s)",object_access,symbol_value.c_str());
				break;

			case LOAD_TYPE::LOAD_TYPE_ARGUMENT:
				sprintf(print_aux_load_value,"ARG(%s)",symbol_value.c_str());
				break;
			default:

				break;
		}
		return print_aux_load_value;
	 }

	 void CScriptClassFactory::printGeneratedCode(CScriptFunction *sfo){

		// PRE: it should printed after compile and updateReferences.
		string pre="";
		string post="";

		if(sfo->instruction != NULL){

			unsigned idx_instruction=0;
			for(tInstruction * instruction=sfo->instruction; instruction->op_code!= END_FUNCTION; instruction++,idx_instruction++){

				int n_ops=0;
				int index_op1 = instruction->index_op1;
				int index_op2 = instruction->index_op2;

				if(index_op1 != -1)
					n_ops++;

				 if(index_op2 != -1)
					 n_ops++;

				 pre="";
				 post="";

					switch(GET_INS_PROPERTY_PRE_POST_OP(instruction->instruction_properties)){
					case INS_PROPERTY_PRE_NEG:
						pre="-";
						break;
					case INS_PROPERTY_PRE_INC:
						pre="++";
						break;
					case INS_PROPERTY_PRE_DEC:
						pre="--";
						break;
					case INS_PROPERTY_POST_INC:
						post="++";
						break;
					case INS_PROPERTY_POST_DEC:
						post="--";
						break;
					default:
						// check whether is constant and numeric
						if(instruction->op_code==OP_CODE::LOAD && instruction->index_op1==LOAD_TYPE_CONSTANT){
							tInfoConstantValue *icv = (((tInfoConstantValue *)instruction->index_op2));
							float n;

							// change the sign
							switch(icv->properties){
							default:
								break;
							case STK_PROPERTY_TYPE_INTEGER:
								if(((intptr_t)icv->stkValue)<0){
									pre="-";
								}
								break;
							case STK_PROPERTY_TYPE_NUMBER:
								memcpy(&n,&icv->stkValue,sizeof(float));
								if(n<0){
									pre="-";
								}
								break;
							}
						}
						break;

					}
				switch(instruction->op_code){

				case  NEW:
					printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t%s\n",idx_instruction,CEval::getOpCodeStr(instruction->op_code),instruction->index_op1!=ZS_INVALID_CLASS?GET_SCRIPT_CLASS_NAME(instruction->index_op1):"???");
					break;
				case  LOAD:
					printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t%s%s%s\n"
							,idx_instruction,
							CEval::getOpCodeStr(instruction->op_code),
							pre.c_str(),
							getStrTypeLoadValue(sfo->instruction,idx_instruction),
							post.c_str());
					break;
				case JNT:
				case JT:
				case JMP:
					printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t%03i\n"
							,idx_instruction
							,CEval::getOpCodeStr(instruction->op_code)
							,(int)instruction->index_op2);
					break;
				case PUSH_SCOPE:
					printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s%c%s%s%s%c\n"
							,idx_instruction
							,CEval::getOpCodeStr(instruction->op_code)
							,instruction->index_op1!=0?'(':' '
							,instruction->index_op1&SCOPE_PROPERTY::BREAK?"BREAK":""
							,instruction->index_op1&SCOPE_PROPERTY::CONTINUE?" CONTINUE":""
							,instruction->index_op1&SCOPE_PROPERTY::FOR_IN?" FOR_IN":""
							,instruction->index_op1!=0?')':' '
							);
					break;
				case POP_SCOPE:
					printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s%c%s%s%s%c\n"
							,idx_instruction
							,CEval::getOpCodeStr(instruction->op_code)
							,instruction->index_op1!=0?'(':' '
							,instruction->index_op1&SCOPE_PROPERTY::BREAK?"BREAK":""
							,instruction->index_op1&SCOPE_PROPERTY::CONTINUE?" CONTINUE":""
							,instruction->index_op1&SCOPE_PROPERTY::FOR_IN?" FOR_IN":""
							,instruction->index_op1!=0?')':' '
							);
					break;
				default:

					if(n_ops==0){
						printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\n",idx_instruction,CEval::getOpCodeStr(instruction->op_code));
					}else if(n_ops==1){
						printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s%s\n"
								,idx_instruction
								,CEval::getOpCodeStr(instruction->op_code)
								,(instruction->instruction_properties & STK_PROPERTY_READ_TWO_POP_ONE)?"_CS":""
								);
					}else{
						printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\n"
								,idx_instruction
								,CEval::getOpCodeStr(instruction->op_code)
								);
					}
					break;
				}
			}
		}

		// and then print its functions ...
		vector<CScriptFunction *> * m_vf = &sfo->m_function;

		for(unsigned j =0; j < m_vf->size(); j++){

			CScriptFunction *local_irfs = (*m_vf)[j];

			if(( local_irfs->symbol_info.properties & PROPERTY_C_OBJECT_REF) != PROPERTY_C_OBJECT_REF){
				string symbol_ref="????";


				//strcpy(symbol_ref,AST_SYMBOL_VALUE_CONST_CHAR(local_irfs->symbol_info.idxAstNode));

				if(local_irfs->idxClass!=ZS_INVALID_CLASS){
					CScriptClass *sc = GET_SCRIPT_CLASS(local_irfs->idxClass);
					if(sc->idxClass == IDX_CLASS_MAIN){
						symbol_ref="Main";
					}else{
						symbol_ref=sfo->symbol_info.symbol_ref+string("::")+string("????");
					}
				}

				printf("-------------------------------------------------------\n");
				printf("\nCode for function \"%s\"\n\n",symbol_ref.c_str());
				printGeneratedCode(m_vf->at(j));
			}
		}
	 }


	 void CScriptClassFactory::printGeneratedCode(){

		 // for all classes print code...
		 for(unsigned i = 0; i < vec_script_class_node.size(); i++){
			 CScriptClass *rc=vec_script_class_node.at(i);
			 for(unsigned f = 0; f < rc->m_function.size(); f++){
				 printGeneratedCode(rc->m_function[f]);
			 }
		 }
	 }
	 // PRINT ASM INFO
	 //----------------------------------------------------------------------------------------------------------------------------------------------------------------



	void  internal_print_error(const char *s){
		 CURRENT_VM->setError(s);
	}


	void CScriptClassFactory::setPrintOutCallback(void (* _printout_callback)(const char *)){
		print_out_callback=_printout_callback;
	}
	 void  CScriptClassFactory::print(const char *s){

		printf("%s\n",s);
		fflush(stdout);

		if(print_out_callback){
			print_out_callback(s);
		}
	 }


	CScriptClassFactory::CScriptClassFactory(){
		CScriptClass *main_class=NULL;
		register_c_base_symbols=false;
		//mapTypeConversion = new map<int, map<int, fntConversionType>>();

		VOID_TYPE_STR = typeid(void).name();
		INT_PTR_TYPE_STR = typeid(int *).name();
		FLOAT_PTR_TYPE_STR = typeid(float *).name();
		CONST_CHAR_PTR_TYPE_STR = typeid(const char *).name();
		STRING_PTR_TYPE_STR = typeid(string *).name();
		BOOL_PTR_TYPE_STR = typeid(bool *).name();

		// particular case for return ... for not to force user returning pointer (this is the only vars allowed, sorry!)
		BOOL_TYPE_STR =  typeid(bool).name();
		INT_TYPE_STR =  typeid(int).name();
		UNSIGNED_INT_TYPE_STR =  typeid(unsigned int).name();
		INTPTR_T_TYPE_STR =  typeid(intptr_t).name();
		FLOAT_TYPE_STR =  typeid(float).name();
		STACK_ELEMENT_STR=  typeid(tStackElement *).name();

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------

		// REGISTER BUILT IN SCRIPT TYPES
		// MAIN CLASS (0)
		main_class=registerClass(MAIN_SCRIPT_CLASS_NAME,""); // 0
		if(main_class->idxScope!=IDX_GLOBAL_SCOPE){
			THROW_RUNTIME_ERROR("Error initializing global scope");
			return;
		}

		// REGISTER BUILT IN C TYPES
		REGISTER_BUILT_IN_TYPE(void,IDX_TYPE_VOID_C);
		REGISTER_BUILT_IN_TYPE(int *,IDX_TYPE_INT_PTR_C);
		REGISTER_BUILT_IN_TYPE(float *,IDX_TYPE_FLOAT_PTR_C);
		REGISTER_BUILT_IN_TYPE(const char *,IDX_TYPE_CONST_CHAR_PTR_C);
		REGISTER_BUILT_IN_TYPE(string *,IDX_TYPE_STRING_PTR_C);
		REGISTER_BUILT_IN_TYPE(bool *,IDX_TYPE_BOOL_PTR_C);
		REGISTER_BUILT_IN_TYPE(int,IDX_TYPE_INT_C);
		REGISTER_BUILT_IN_TYPE(unsigned int,IDX_TYPE_UNSIGNED_INT_C);
		REGISTER_BUILT_IN_TYPE(intptr_t,IDX_TYPE_INTPTR_T_C);
		REGISTER_BUILT_IN_TYPE(float,IDX_TYPE_FLOAT_C);
		REGISTER_BUILT_IN_TYPE(bool,IDX_TYPE_BOOL_C);

		// REGISTER BUILT IN CLASS TYPES
		REGISTER_BUILT_IN_CLASS(tStackElement,IDX_STACK_ELEMENT);
		REGISTER_BUILT_IN_CLASS(CScriptVariable,IDX_CLASS_SCRIPT_VAR);
		REGISTER_BUILT_IN_CLASS(CStringScriptVariable,IDX_CLASS_STRING);
		REGISTER_BUILT_IN_CLASS(CVectorScriptVariable,IDX_CLASS_VECTOR);
		REGISTER_BUILT_IN_CLASS(CFunctorScriptVariable,IDX_CLASS_FUNCTOR);
		REGISTER_BUILT_IN_CLASS(CStructScriptVariable,IDX_CLASS_STRUCT);


		//------------------------------------------------------------------------------------------------------------------------------------------------------------------

		// rgister basic classes (Warning!!! must match idx !!! and the order is important!!!)

		//----------------------------------------------------------------------
		// From here you defined all basic, start define hierarchy

		// register custom functions ...
		class_C_baseof<CVectorScriptVariable,CScriptVariable>();
		class_C_baseof<CFunctorScriptVariable,CScriptVariable>();
		class_C_baseof<CStructScriptVariable,CScriptVariable>();


		//------------------------------------------------------------------------------------------------------------
		// Let's register functions,...
		// register c function's

		// MAIN FUNCTION (0)...
		main_class->registerFunctionMember(MAIN_SCRIPT_FUNCTION_OBJECT_NAME);

		register_C_Function("print",print);

		register_C_Function("error",internal_print_error);

		register_C_FunctionMember<CVectorScriptVariable>("size",&CVectorScriptVariable::size);
		register_C_FunctionMember<CVectorScriptVariable>("push",static_cast<void (CVectorScriptVariable:: *)(tStackElement *)>(&CVectorScriptVariable::push));
		register_C_FunctionMember<CVectorScriptVariable>("pop",&CVectorScriptVariable::pop);


		register_C_FunctionMember<CStructScriptVariable>("add",&CStructScriptVariable::add_attr);
		register_C_FunctionMember<CStructScriptVariable>("remove",&CStructScriptVariable::remove_attr);
		register_C_FunctionMember<CStructScriptVariable>("size",&CStructScriptVariable::size);

	}

	void (* CScriptClassFactory::print_out_callback)(const char *) = NULL;


	unsigned char CScriptClassFactory::getIdxClassFromIts_C_TypeInternal(const string & c_type_str){

		// 1. we have to handle primitives like void, (int *), (bool *),(float *) and (string *).
		 // 2. Check for rest registered C classes...
		 for(unsigned i = 0; i < vec_script_class_node.size(); i++){
			 if(vec_script_class_node.at(i)->classPtrType==c_type_str)
			 {
				 return i;
			 }
		 }

		 return ZS_INVALID_CLASS;
	 }

	unsigned char 			CScriptClassFactory::getIdxClassFromIts_C_Type(const string & c_type_str){
		return CScriptClassFactory::getIdxClassFromIts_C_TypeInternal(c_type_str);
	}

	bool 	CScriptClassFactory::isIdxClassInstanceOf(unsigned char idxSrcClass, unsigned char class_idx){

		if(idxSrcClass == class_idx){
			return true;
		}

		CScriptClass * theClass = vec_script_class_node.at(idxSrcClass);

		for(unsigned i=0; i < theClass->idxBaseClass.size(); i++){
			if(isIdxClassInstanceOf(theClass->idxBaseClass[i],class_idx)){
				return true;
			}
		}

		return false;
	}

	void CScriptClassFactory::register_C_BaseSymbolsInt(bool _register){
		register_c_base_symbols = _register;
	}


	CScriptClass * CScriptClassFactory::registerClass(const string & class_name, const string & base_class_name){
		unsigned char  index;
		CScriptClass *sci=NULL;


		if(vec_script_class_node.size()>=MAX_REGISTER_CLASSES){
			THROW_RUNTIME_ERROR("Max register classes reached (Max:%i)",MAX_REGISTER_CLASSES);
			return NULL;
		}


		if(class_name.empty()){
			THROW_RUNTIME_ERROR("Class name empty");
			return NULL;
		}

		CScriptClass *base_class=NULL;
		if(base_class_name != ""){
			if((base_class = getScriptClass(base_class_name)) == NULL){
				return NULL;
			}
		}

		if((index = getIdxScriptClass_Internal(class_name))==ZS_INVALID_CLASS){ // check whether is local var registered scope ...

			// NEW SCOPE C and register ...
			unsigned char idxClass=(unsigned char)vec_script_class_node.size()-1;

			CScope * scope = NEW_SCOPE();
			sci = new CScriptClass(idxClass,scope->idxScope);
			scope->setScriptClass(sci);


			sci->classPtrType = TYPE_SCRIPT_VARIABLE;

			sci->symbol_info.symbol_ref = class_name;

			vec_script_class_node.push_back(sci);

			if(base_class != NULL){
				sci->idxBaseClass.push_back(base_class->idxClass);
			}

			return sci;

		}else{
			THROW_RUNTIME_ERROR("class \"%s\" already registered",class_name.c_str());
		}

		return NULL;
	}

	const char * CScriptClassFactory::getMetamethod(METAMETHOD_OPERATOR op){

		switch (op) {
		case	EQU_METAMETHOD:		return  "_equ";  // ==
		case	NOT_EQU_METAMETHOD: return  "_nequ";  // !=,
		case	LT_METAMETHOD:		return  "_lt";  // <
		case	LTE_METAMETHOD:		return  "_lte";  // <=
		case	NOT_METAMETHOD:		return  "_not"; // !
		case	GT_METAMETHOD:		return  "_gt";  // >
		case	GTE_METAMETHOD:		return  "_gte"; // >=

		case	NEG_METAMETHOD:		return  "_neg"; // -a, !a
		case	ADD_METAMETHOD:		return  "_add"; // +
		case	DIV_METAMETHOD:		return  "_div"; // /
		case	MUL_METAMETHOD:		return  "_mul"; // *
		case	MOD_METAMETHOD:		return  "_mod";  // %
		case	AND_METAMETHOD:		return  "_and"; // binary and
		case	OR_METAMETHOD:		return  "_or"; //   binary or
		case	XOR_METAMETHOD:		return  "_xor"; // binary xor
		case	SHL_METAMETHOD:		return  "_shl"; // binary shift left
		case	SHR_METAMETHOD:		return  "_shr"; // binary shift right
		case	SET_METAMETHOD:		return  "_set"; // set
		default:
			return "none";
		}

		return "none";
	}

	vector<CScriptClass *> * CScriptClassFactory::getVectorScriptClassNode(){
		return & vec_script_class_node;
	}

	map<unsigned char, map<unsigned char, fntConversionType>>  *	 CScriptClassFactory::getMapTypeConversion() {
		return & mapTypeConversion;
	}

	CScriptClass 	* CScriptClassFactory::getScriptClass(unsigned char idx){
		if(idx == ZS_INVALID_CLASS){
			THROW_RUNTIME_ERROR("CScriptClass node out of bound");
			return NULL;
		}

		return vec_script_class_node.at(idx);
	}

	CScriptClass 	* CScriptClassFactory::getScriptClass(const string & class_name){
		unsigned char index;
		if((index = getIdxScriptClass_Internal(class_name))!=ZS_INVALID_CLASS){ // check whether is local var registered scope ...
			return vec_script_class_node.at(index);
		}

		return NULL;
	}

	CScriptClass *CScriptClassFactory::getScriptClassBy_C_ClassPtr(const string & class_type){

		for(unsigned i = 0; i < vec_script_class_node.size(); i++){
			if(class_type == vec_script_class_node.at(i)->classPtrType){//metadata_info.object_info.symbol_info.c_type){
				return vec_script_class_node.at(i);
			}
		}
		return NULL;
	}

	unsigned char CScriptClassFactory::getIdxScriptClass_Internal(const string & class_name){

		for(unsigned i = 0; i < vec_script_class_node.size(); i++){
			if(class_name == vec_script_class_node.at(i)->symbol_info.symbol_ref){
				return i;
			}
		}
		return ZS_INVALID_CLASS;
	}

	bool CScriptClassFactory::isClassRegistered(const string & v){
		return getIdxScriptClass_Internal(v) != ZS_INVALID_CLASS;
	}


	void CScriptClassFactory::clear(){
		bool end=false;
		do{
			CScriptClass * sc = vec_script_class_node.at(vec_script_class_node.size()-1);
			end=(sc->symbol_info.properties & PROPERTY_C_OBJECT_REF) == PROPERTY_C_OBJECT_REF;

			if(!end){

				delete sc;
				vec_script_class_node.pop_back();

			}

		}while(!end);
	}


	CScriptVariable *		CScriptClassFactory::instanceScriptVariableByClassName(const string & class_name){

		 // 0. Search class info ...
		 CScriptClass * rc = getScriptClass(class_name);

		 if(rc != NULL){
			 return instanceScriptVariableByIdx(rc->idxClass);
		 }

		 return NULL;
	 }

	 CScriptVariable 		 * CScriptClassFactory::instanceScriptVariableByIdx(unsigned char idx_class, void * value_object){

		 CScriptVariable *class_object=NULL;

		 // 0. Search class info ...
		 CScriptClass *rc = getScriptClass(idx_class);

		 if(rc != NULL){

			 // Is a primitive ?
			 switch(rc->idxClass){

			 case IDX_TYPE_VOID_C:
			 case IDX_TYPE_INT_PTR_C:
			 case IDX_TYPE_FLOAT_PTR_C:
			 case IDX_TYPE_STRING_PTR_C:
			 case IDX_TYPE_BOOL_PTR_C:
				 THROW_RUNTIME_ERROR("Internal error");
				 return NULL;
				 break;

			 case IDX_CLASS_VECTOR:
			 case IDX_CLASS_STRUCT:
				 class_object = (CScriptVariable *)value_object;
				 break;
			 default:
				 class_object = new CScriptVariable();
				 class_object->init(rc, value_object);
				 break;
			 }
		 }
		 return class_object;
	 }

	/**
	 * Register C variable
	 */
	 tVariableSymbolInfo *  CScriptClassFactory::register_C_VariableInt(const string & var_name,void * var_ptr, const string & var_type)
	{
		//CScope *scope;
		tVariableSymbolInfo *irs;
		//int idxVariable;

		if(var_ptr==NULL){
			THROW_RUNTIME_ERROR("cannot register var \"%s\" with NULL reference value", var_name.c_str());
			return NULL;
		}

		CScriptFunction *main_function=MAIN_FUNCTION;

		if(main_function == NULL){
			THROW_RUNTIME_ERROR("main function is not created");
			return  NULL;
		}

		if(getIdxClassFromIts_C_Type(var_type) == ZS_INVALID_CLASS){
			THROW_RUNTIME_ERROR("%s has not valid type (%s)",var_name.c_str(),var_type.c_str());
			return  NULL;
		}


		if((irs = main_function->registerVariable(var_name,var_type,(intptr_t)var_ptr,::PROPERTY_C_OBJECT_REF | PROPERTY_STATIC_REF)) != NULL){

			zs_print_debug_cr("Registered variable name: %s",var_name.c_str());

			return irs;
		}

		return NULL;
	}

	unsigned char CScriptClassFactory::getIdx_C_RegisteredClass(const string & str_classPtr){
			// ok check c_type
			for(unsigned i = 0; i < vec_script_class_node.size(); i++){
				if(vec_script_class_node[i]->classPtrType == str_classPtr){
					return i;
				}
			}

		  /*if(throw_if_not_found){
				THROW_RUNTIME_ERROR("C class %s is not registered",str_classPtr.c_str());
			}*/

			return ZS_INVALID_CLASS;
	}

	bool CScriptClassFactory::addArgumentFunctionSymbol(const string & class_name,const string & function_name, const string & arg_name){

			CScriptClass *rc = getScriptClass(class_name);

			if(rc==NULL){
					THROW_RUNTIME_ERROR("C class \"%s\" not registered",class_name.c_str());
					return false;
			}


			CScriptFunction *sc = rc->getFunctionByName(function_name);//getIdxScriptFunctionObjectByClassFunctionName(class_name,function_name);

			if(sc==NULL){
					THROW_RUNTIME_ERROR("Function member \"%s::%s\" not registered",class_name.c_str(),function_name.c_str());
					return false;
			}


			sc->m_arg.push_back({ZS_UNDEFINED_IDX,arg_name});

			return true;

		}


		intptr_t CScriptClassFactory::doCast(intptr_t obj, unsigned char idx_src_class, unsigned char idx_convert_class){//c_class->idxClass,idx_return_type){

			CScriptClass *src_class = getScriptClass(idx_src_class);
			CScriptClass *convert_class = getScriptClass(idx_convert_class);

			//local_map_type_conversion
			if(mapTypeConversion.count(idx_src_class) == 0){
				THROW_RUNTIME_ERROR("There's no type src conversion class \"%s\".",demangle(src_class->classPtrType).c_str());
				return 0;
			}

			if((mapTypeConversion)[idx_src_class].count(idx_convert_class) == 0){
				THROW_RUNTIME_ERROR("There's no dest conversion class \"%s\".",demangle(convert_class->classPtrType).c_str());
				return 0;
			}

			return (mapTypeConversion)[idx_src_class][idx_convert_class](obj);
		}



		const char * CScriptClassFactory::getScriptClassName(unsigned char idx){
			if(idx != ZS_INVALID_CLASS){
				return vec_script_class_node[idx]->symbol_info.symbol_ref.c_str();
			}
			 return "class_unknow";
		}

	CScriptClassFactory::~CScriptClassFactory(){
		// we have to destroy all allocated constructor/destructor ...
		for (unsigned i = 0; i < vec_script_class_node.size(); i++) {

			delete vec_script_class_node.at(i);
		}

		vec_script_class_node.clear();



	}
};
