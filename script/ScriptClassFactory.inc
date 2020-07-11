#define 				MAX_REGISTER_CLASSES 	100
#define 				ZS_INVALID_CLASS		((unsigned char)ZS_UNDEFINED_IDX)

namespace zetscript{

	/**
	 * Register C function
	 */
	template <typename F>
	bool ScriptClassFactory::register_C_Function(const char * function_name,F function_ptr, const char *registered_file,int registered_line)
	{
		int idx_return_type=-1;
		std::string return_type;
		std::vector<std::string> arg;
		std::vector<ParamArgInfo> arg_info;
		intptr_t ref_ptr=0;

		if(!script_function_factory->checkCanregister_C_Function(function_name)){
			return false;
		}

		if(main_function == NULL){
			THROW_RUNTIME_ERROR("main function is not created");
			return false;
		}

		// 1. check all parameters ok.
		using Traits3 = FunctionTraits<decltype(function_ptr)>;
		getParamsFunction<Traits3>(0,return_type, arg, MakeIndexSequence<Traits3::arity>{});

		if(arg.size()>6){
			THROW_RUNTIME_ERROR("Max argyments reached");
		}

		// check valid parameters ...
		if((idx_return_type=getIdxClassFromIts_C_Type(return_type))==-1){
			THROW_RUNTIME_ERROR(zs_strutils::format("Return type \"%s\" for function \"%s\" not registered"
					,zs_rtti::demangle(return_type).c_str()
					,function_name));
			return false;
		}

		for(unsigned int i = 0; i < arg.size(); i++){
			int idx_type = getIdxClassFromIts_C_Type(arg[i]);

			if(idx_type==IDX_BUILTIN_TYPE_FLOAT_C || idx_type==IDX_BUILTIN_TYPE_BOOL_C){
				THROW_RUNTIME_ERROR(zs_strutils::format("Argument (%i) type \"%s\" for function \"%s\" is not supported as parameter, you should use pointer instead (i.e %s *)"
						,i
						,zs_rtti::demangle(arg[i]).c_str()
						,function_name
						,zs_rtti::demangle(arg[i]).c_str()));
				return false;
			}

			if(idx_type ==ZS_INVALID_CLASS){
				THROW_RUNTIME_ERROR(zs_strutils::format("Argument (%i) type \"%s\" for function \"%s\" not registered"
						,i
						,zs_rtti::demangle(arg[i]).c_str()
						,function_name));
				return false;
			}

			arg_info.push_back({idx_type,arg[i]});
		}

		ref_ptr=(intptr_t)function_ptr;

		// Init struct...
		main_function->registerFunction(registered_file,registered_line,function_name,arg_info,idx_return_type,ref_ptr,SYMBOL_INFO_PROPERTY_C_OBJECT_REF | SYMBOL_INFO_PROPERTY_STATIC_REF);

		ZS_PRINT_DEBUG("Registered function name: %s",function_name);
		return true;
	}

	/**
	 * Register C Class. Return index registered class
	 */
	template<class T>
	 bool ScriptClassFactory::register_C_SingletonClass(const std::string & class_name, const char *registered_file,int registered_line){//, const std::string & base_class_name=""){

		//std::vector<ScriptClass *> script_classes=getVectorScriptClassNode();
		// to make compatible MSVC shared library
		int size=script_classes.size();

		if(size>=MAX_REGISTER_CLASSES){
			THROW_RUNTIME_ERROR(zs_strutils::format("Max register classes reached (Max:%i)",MAX_REGISTER_CLASSES));
			return NULL;
		}

		// after MAX_BASIC_CLASS_TYPES all registered C classes should follow a registered C class ...
		if(size > 1){ // because = 0 is reserved for main class and >= 1 is for C registered classes
			if((
				((script_classes[size-1]->symbol_info.symbol_info_properties&SYMBOL_INFO_PROPERTY_C_OBJECT_REF)!=SYMBOL_INFO_PROPERTY_C_OBJECT_REF)
			)){
				THROW_RUNTIME_ERROR(zs_strutils::format("C class \"%s\" should register after C classes. Register C classes after script classes are not allowed",class_name.c_str()));
				return NULL;
			}
		}

		if(!isClassRegistered(class_name)){

			std::string str_class_name_ptr = typeid( T *).name();

			if(getIdx_C_RegisteredClass(str_class_name_ptr)!=ZS_INVALID_CLASS){
				THROW_RUNTIME_ERROR(zs_strutils::format("this %s is already registered",zs_rtti::demangle(typeid( T).name()).c_str()));
				return NULL;
			}

			unsigned char idx_class=(short)(script_classes.size());
			Scope * scope = scope_factory->newScope(ZS_UNDEFINED_IDX,true);
			Symbol *symbol=scope->registerSymbol(registered_file,registered_line,class_name, NO_PARAMS_IS_CLASS);
			if(symbol == NULL){
				return NULL;
			}

			ScriptClass *irc = new ScriptClass(zs,idx_class);
			scope->setScriptClass(irc);

			irc->symbol_info.symbol=symbol;

			// in C there's no script constructor ...
			irc->idx_function_member_constructor=-1;
			// allow dynamic constructor in function its parameters ...

			irc->str_class_ptr_type=str_class_name_ptr;
			irc->symbol_info.symbol_info_properties=SYMBOL_INFO_PROPERTY_C_OBJECT_REF;

			irc->c_constructor = NULL;
			irc->c_destructor = NULL;
			script_classes.push_back(irc);

			irc->idx_class=(unsigned char)(script_classes.size()-1);
			ZS_PRINT_DEBUG("* C++ class \"%s\" registered as (%s).",class_name.c_str(),zs_rtti::demangle(str_class_name_ptr).c_str());

			return true;
		}
		else{
			THROW_RUNTIME_ERROR(zs_strutils::format("%s already exist", class_name.c_str()));
		}
		return false;
	}

	/**
	 * Register C Class. Return index registered class
	 */
	template<typename T>
	bool ScriptClassFactory::register_C_Class(const std::string & class_name, const char *registered_file,int registered_line){//, const std::string & base_class_name=""){

		if(register_C_SingletonClass<T>(class_name)){
			// get class...
			ScriptClass *irc =getScriptClass(class_name);

			if(irc->idx_class < IDX_BUILTIN_TYPE_MAX && irc->idx_class != IDX_BUILTIN_TYPE_STACK_ELEMENT){
				THROW_RUNTIME_ERROR(zs_strutils::format("The class to register \"%s\"  should BYTE_CODE_NOT BE a built in class",irc->str_class_ptr_type.c_str()));
				return false;
			}

			if(irc==NULL){
				return false;
			}

			//put the constructor/destructor...
			irc->c_constructor = new std::function<void *()>([=](){
				T *t=new T();
				return t;
			});

			irc->c_destructor = new std::function<void (void *)>([=](void *p){
				delete (T *)p;
			});

			return true;
		}
		return NULL;
	}

	/**
	 * Register C Class. Return index registered class
	 */
	template<typename T>
	bool ScriptClassFactory::register_C_ClassBuiltIn(const std::string & class_name, const char *registered_file,int registered_line){//, const std::string & base_class_name=""){

		if(register_C_SingletonClass<T>(class_name)){
			ScriptClass *irc =getScriptClass(class_name);

			if(irc->idx_class >= IDX_BUILTIN_TYPE_MAX){
				THROW_RUNTIME_ERROR(zs_strutils::format("The class to register \"%s\" should be a built in class",irc->str_class_ptr_type.c_str()));
				return false;
			}

			// get class...
			if(irc==NULL){
				return false;
			}

			//put the constructor/destructor...
			ZetScript *_zs=zs;
			irc->c_constructor = new std::function<void *()>([_zs](){
				T* t=new T(_zs);
				return t;
			});

			irc->c_destructor = new std::function<void (void *)>([=](void *p){
				delete (T *)p;
			});

			return true;
		}

		return NULL;
	}


	template<class T, class B>
	bool ScriptClassFactory::class_C_BaseOf(){
		std::string base_class_name=typeid(B).name();
		std::string base_class_name_ptr=typeid(B *).name();
		std::string class_name=typeid(T).name();
		std::string class_name_ptr=typeid(T *).name();

		int idx_base_class = getIdxClassFromIts_C_Type(base_class_name_ptr);
		if(idx_base_class == -1) {
			THROW_RUNTIME_ERROR(zs_strutils::format("base class %s not registered",base_class_name_ptr.c_str()));
			return false;
		}


		int register_class = getIdxClassFromIts_C_Type(class_name_ptr);
		if(register_class == -1) {
			THROW_RUNTIME_ERROR(zs_strutils::format("class %s not registered",class_name_ptr.c_str()));
			return false;
		}

		if(class_C_BaseOf(register_class,idx_base_class)){
			THROW_RUNTIME_ERROR(zs_strutils::format("C++ class \"%s\" is already registered as base of \"%s\" ",zs_rtti::demangle(class_name).c_str(), zs_rtti::demangle(base_class_name).c_str()));
			return false;
		}

		// check whether is in fact base of ...
		if(!std::is_base_of<B,T>::value){
			THROW_RUNTIME_ERROR(zs_strutils::format("C++ class \"%s\" is not base of \"%s\" ",zs_rtti::demangle(class_name).c_str(), zs_rtti::demangle(base_class_name).c_str()));
			return false;
		}

		ScriptClass *sc=script_classes[register_class];
		while( sc->idx_base_class.size()>0){

			sc=getScriptClass(sc->idx_base_class[0]); // get base class...
			if(sc->str_class_ptr_type ==base_class_name_ptr){
				THROW_RUNTIME_ERROR(zs_strutils::format("C++ class \"%s\" already base of \"%s\" ",zs_rtti::demangle(class_name).c_str(), zs_rtti::demangle(base_class_name).c_str()));
				return false;
			}
		}


		ScriptClass *irc_class = script_classes[register_class];
		irc_class->idx_base_class.push_back(idx_base_class);

		//std::map<int, std::map<int, ConversionType>>  *	local_map_type_conversion=	getConversionTypes();
		(conversion_types)[irc_class->idx_class][idx_base_class]=[](intptr_t entry){ return (intptr_t)(B *)((T *)entry);};


		if(register_c_base_symbols){
			//----------------------------
			//
			// DERIVATE STATE
			//
			// disabled for polymorphic classes because its function pointer std::map change at runtime)
			// https://stackoverflow.com/questions/48572734/is-possible-do-a-later-function-binding-knowing-its-function-type-and-later-the
			//

			ScriptClass *irc_base = script_classes[idx_base_class];

			unsigned short derivated_symbol_info_properties=SYMBOL_INFO_PROPERTY_C_OBJECT_REF| SYMBOL_INFO_PROPERTY_IS_DERIVATED;
			if(std::is_polymorphic<B>::value==true){
				derivated_symbol_info_properties|=SYMBOL_INFO_PROPERTY_IS_POLYMORPHIC;
			}

			// register all symbols function from base ...
			// vars ...
			for(unsigned i = 0; i < irc_base->local_variable.size(); i++){

				SymbolInfo *irs_source = &irc_base->local_variable[i];

				SymbolInfo irs;
				// Init struct...
				//irs.idx_class = idx_base_class;
				irs.ref_ptr=irs_source->ref_ptr;
				irs.c_type = irs_source->c_type;
				//irs.
				irs.symbol=irs_source->symbol;
				irs.symbol_info_properties = derivated_symbol_info_properties;
				irs.idx_symbol = (short)(irc_class->local_variable.size());
				irc_class->local_variable.push_back(irs);

			}

			// functions ...
			for(unsigned i = 0; i < irc_base->local_function.size(); i++){

				ScriptFunction *irs_source = irc_base->local_function[i];
				irc_class->registerFunction(
						irs_source->symbol_info.symbol->file,
						irs_source->symbol_info.symbol->line,
						irs_source->symbol_info.symbol->name,
						irs_source->arg_info,
						irs_source->idx_return_type,
						irs_source->symbol_info.ref_ptr,
						derivated_symbol_info_properties
						);
			}
		}

		//
		// DERIVATE STATE
		//
		//----------------------------
		// finally maps object type ...
		return true;
	}


	/**
	 * Register C Member function Class
	 */
	template < typename C, typename R, class T, typename..._A>
	bool ScriptClassFactory::register_C_FunctionMember(
			const char *function_name
			,R (T:: *function_type)(_A...)
			, const char *registered_file
			,int registered_line
	)
	{
		// to make compatible MSVC shared library
		std::string return_type;
		std::vector<std::string> arg;
		std::vector<ParamArgInfo> arg_info;
		int idx_return_type=-1;
		intptr_t ref_ptr=0;
		std::string str_class_name_ptr = typeid( C *).name();

		if(!script_function_factory->checkCanregister_C_Function(function_name)){
			return false;
		}

		ScriptClass * sc=getScriptClass(str_class_name_ptr);

		if(sc == NULL){
			return false;
		}

		// 1. check all parameters ok.
		using Traits3 = FunctionTraits<decltype(function_type)>;
		getParamsFunction<Traits3>(0,return_type, arg, MakeIndexSequence<Traits3::arity>{});


		// check valid parameters ...
		if((idx_return_type=getIdxClassFromIts_C_Type(return_type)) == -1){
			THROW_RUNTIME_ERROR(zs_strutils::format("Return type \"%s\" for function \"%s\" not registered",zs_rtti::demangle(return_type).c_str(),function_name));
			return false;
		}

		for(unsigned int i = 0; i < arg.size(); i++){
			int idx_type=getIdxClassFromIts_C_Type(arg[i]);

			if(idx_type==IDX_BUILTIN_TYPE_FLOAT_C || idx_type==IDX_BUILTIN_TYPE_BOOL_C){
				THROW_RUNTIME_ERROR(zs_strutils::format("Argument (%i) type \"%s\" for function \"%s\" is not supported as parameter, you should use pointer instead (i.e %s *)",i,zs_rtti::demangle(arg[i]).c_str(),function_name,zs_rtti::demangle(arg[i]).c_str()));
				return false;
			}

			if(idx_type==ZS_INVALID_CLASS){
				THROW_RUNTIME_ERROR(zs_strutils::format("Argument (%i) type \"%s\" for function \"%s\" not registered",i,zs_rtti::demangle(arg[i]).c_str(),function_name));
				return false;
			}

			arg_info.push_back({idx_type,arg[i]});

		}

		if((ref_ptr=((intptr_t)proxy_function_factory->newProxyFunctionMember<C>(arg.size(),function_type)))==0){
			return false;
		}
		// register member function...
		ScriptFunction *sf = sc->registerFunction(registered_file,registered_line,function_name,arg_info,idx_return_type,ref_ptr,SYMBOL_INFO_PROPERTY_C_OBJECT_REF);

		ZS_PRINT_DEBUG("Registered member function name %s::%s",zs_rtti::demangle(typeid(C).name()).c_str(), function_name);

		if(ZS_STRCMP(ByteCodeMetamethodToStr(BYTE_CODE_METAMETHOD_SET),==,function_name)){
			sc->metamethod_operator[BYTE_CODE_METAMETHOD_SET].push_back(sf);
			ZS_PRINT_DEBUG("Registered metamethod %s::%s",zs_rtti::demangle(typeid(C).name()).c_str(), function_name);
		}
		return true;
	}


	/**
	 * Register C Member function Class
	 */
	template <typename C, typename F>
	bool ScriptClassFactory::register_C_FunctionMemberStatic(const char *function_name,F function_ptr, const char *registered_file,int registered_line)
	{
		// to make compatible MSVC shared library
		//std::vector<ScriptClass *> * script_classes = getVecScriptClassNode();

		std::string return_type;
		std::vector<std::string> params;
		std::vector<std::string> arg;
		std::vector<ParamArgInfo> arg_info;
		int idx_return_type=-1;
		intptr_t ref_ptr=0;
		std::string str_class_name_ptr = typeid( C *).name();
		std::string function_class_name = zs_rtti::demangle(typeid(C).name())+"::"+function_name;


		if(!script_function_factory->checkCanregister_C_Function(function_class_name)){
			return false;
		}

		ScriptClass *c_class = getScriptClassBy_C_ClassPtr(str_class_name_ptr);

		if(c_class == NULL){
			return false;
		}

		// 1. check all parameters ok.
		using Traits3 = FunctionTraits<decltype(function_ptr)>;
		getParamsFunction<Traits3>(0,return_type, arg, MakeIndexSequence<Traits3::arity>{});

		if(arg.size()>6){
			THROW_RUNTIME_ERROR("Max argyments reached");
		}

		// check valid parameters ...
		if((idx_return_type=getIdxClassFromIts_C_Type(return_type)) == -1){
			THROW_RUNTIME_ERROR(zs_strutils::format("Return type \"%s\" for function \"%s\" not registered",zs_rtti::demangle(return_type).c_str(),function_name));
			return false;
		}

		for(unsigned int i = 0; i < arg.size(); i++){
			int idx_type = getIdxClassFromIts_C_Type(arg[i]);

			if(idx_type==IDX_BUILTIN_TYPE_FLOAT_C || idx_type==IDX_BUILTIN_TYPE_BOOL_C){
				THROW_RUNTIME_ERROR(zs_strutils::format("Argument (%i) type \"%s\" for function \"%s\" is not supported as parameter, you should use pointer instead (i.e %s *)",i,zs_rtti::demangle(arg[i]).c_str(),function_name,zs_rtti::demangle(arg[i]).c_str()));
				return false;
			}

			if(idx_type==ZS_INVALID_CLASS){
				THROW_RUNTIME_ERROR(zs_strutils::format("Argument (%i) type \"%s\" for function \"%s\" not registered",i,zs_rtti::demangle(arg[i]).c_str(),function_name));
				return false;
			}

			arg_info.push_back({idx_type,arg[i]});
		}

		ref_ptr=(intptr_t)function_ptr;

		// register member function...
		ScriptFunction * sf = c_class->registerFunction(registered_file,registered_line,function_name,arg_info, idx_return_type, ref_ptr, SYMBOL_INFO_PROPERTY_C_OBJECT_REF | SYMBOL_INFO_PROPERTY_STATIC_REF);
		ZS_PRINT_DEBUG("Registered member function name %s::%s",zs_rtti::demangle(typeid(T).name()).c_str(), function_name);

		// check whether is static metamethod...
		if(ZS_STRCMP(ByteCodeMetamethodToStr(BYTE_CODE_METAMETHOD_SET),!=,function_name)){

			for(int i = 0; i < BYTE_CODE_METAMETHOD_MAX; i++){
				if(ZS_STRCMP(ByteCodeMetamethodToStr((ByteCodeMetamethod)i),==,function_name)){

					// check if they are gte,gt,equ, not_equ, lt, lte
					if(  i == BYTE_CODE_METAMETHOD_EQU //STRCMP(function_name, == ,"_equ")
					  || i == BYTE_CODE_METAMETHOD_NOT_EQU //STRCMP(function_name, ==, "_nequ")
					  || i == BYTE_CODE_METAMETHOD_LT//STRCMP(function_name, ==, "_lt")
					  || i == BYTE_CODE_METAMETHOD_LTE//STRCMP(function_name, ==, "_lte")
					  || i == BYTE_CODE_METAMETHOD_GT//STRCMP(function_name, ==, "_gt")
					  || i == BYTE_CODE_METAMETHOD_GTE//STRCMP(function_name, ==, "_gte")
					  || i == BYTE_CODE_METAMETHOD_NOT//STRCMP(function_name, ==, "_gte")
					  ){
						// return type must be bool...
						if(ZS_STRCMP(return_type.c_str(), != ,typeid(bool).name())){
							THROW_RUNTIME_ERROR(zs_strutils::format("error registering metamethod %s::%s. Expected return bool but it was %s",
									zs_rtti::demangle(typeid(C).name()).c_str(),
									function_name,
									zs_rtti::demangle(return_type.c_str()).c_str()));
							return false;

						}
					}else if((return_type != str_class_name_ptr) && (i!= BYTE_CODE_METAMETHOD_SET)){

						THROW_RUNTIME_ERROR(zs_strutils::format("error registering metamethod %s::%s. Expected return %s but it was %s",
								zs_rtti::demangle(typeid(C).name()).c_str(),
								function_name,
								zs_rtti::demangle(str_class_name_ptr.c_str()).c_str(),
								zs_rtti::demangle(return_type.c_str()).c_str()));
						return false;
					}

					c_class->metamethod_operator[i].push_back(sf);

					ZS_PRINT_DEBUG("Registered metamethod %s::%s",zs_rtti::demangle(typeid(T).name()).c_str(), function_name);
					break;
				}
			}
		}else{
			THROW_RUNTIME_ERROR(zs_strutils::format("error! cannot register metamethod set on static function. Must be member function"));
			return false;
		}
		return true;
	}

	template <typename F>
	bool ScriptClassFactory::register_C_FunctionAsFunctionMember(const char *function_name,F function_type, const char *registered_file,int registered_line){
		// to make compatible MSVC shared library
		//std::vector<ScriptClass *> * script_classes = getVecScriptClassNode();

		std::string return_type;
		std::vector<std::string> params;
		std::vector<std::string> arg;
		std::vector<ParamArgInfo> arg_info;
		int idx_return_type=-1;
		intptr_t ref_ptr=0;
		std::string function_class_name;// = zs_rtti::demangle(typeid(T).name())+"::"+function_name;

		// 1. check all parameters ok.
		using Traits3 = FunctionTraits<decltype(function_type)>;
		getParamsFunction<Traits3>(0,return_type, arg, MakeIndexSequence<Traits3::arity>{});

		if(arg.size()>6){
			THROW_RUNTIME_ERROR("Max argyments reached");
		}

		if(arg.size()==0){
			THROW_RUNTIME_ERROR(zs_strutils::format("register_C_FunctionAsFunctionMember at least need first parameter that defines the object to add function %s",function_name));
		}

		ScriptClass * c_class=	getScriptClassBy_C_ClassPtr(arg[0]);
		if(c_class == NULL){
			THROW_RUNTIME_ERROR(zs_strutils::format("class %s is not registered",arg[0].c_str()));
			return false;
		}

		function_class_name = c_class->symbol_info.symbol->name+"::"+function_name;

		if(!script_function_factory->checkCanregister_C_Function(function_class_name)){
			return false;
		}

		// check valid parameters ...
		if((idx_return_type=getIdxClassFromIts_C_Type(return_type)) == -1){
			THROW_RUNTIME_ERROR(zs_strutils::format("Return type \"%s\" for function \"%s\" not registered",zs_rtti::demangle(return_type).c_str(),function_name));
			return false;
		}

		for(unsigned int i = 0; i < arg.size(); i++){
			int idx_type = getIdxClassFromIts_C_Type(arg[i]);

			if(idx_type==IDX_BUILTIN_TYPE_FLOAT_C || idx_type==IDX_BUILTIN_TYPE_BOOL_C){
				THROW_RUNTIME_ERROR(zs_strutils::format("Argument (%i) type \"%s\" for function \"%s\" is not supported as parameter, you should use pointer instead (i.e %s *)",i,zs_rtti::demangle(arg[i]).c_str(),function_name,zs_rtti::demangle(arg[i]).c_str()));
				return false;
			}

			if(idx_type==ZS_INVALID_CLASS){
				THROW_RUNTIME_ERROR(zs_strutils::format("Argument (%i) type \"%s\" for function \"%s\" not registered",i,zs_rtti::demangle(arg[i]).c_str(),function_name));
				return false;
			}

			arg_info.push_back({idx_type,arg[i]});
		}

		ref_ptr=(intptr_t)function_type;

		// register member function...
		c_class->registerFunction(
				registered_file
				,registered_line
				,function_name
				,arg_info
				, idx_return_type
				, ref_ptr
				, SYMBOL_INFO_PROPERTY_C_OBJECT_REF | SYMBOL_INFO_PROPERTY_STATIC_REF | SYMBOL_INFO_PROPERTY_SET_FIRST_PARAMETER_AS_THIS
		);
		ZS_PRINT_DEBUG("Registered C function %s as function member %s::%s",function_name, function_class_name.c_str());

		return true;
	}

	/**
	 * Register C Member var
	 */
	//<o, decltype(o::s)>(STR(s),ZetScript::offset_of(&o::s)) &CVar::mierda
	template <typename C, typename R,typename T>
	bool ScriptClassFactory::register_C_VariableMember(const char *var_name, R T::*var_pointer, const char *registered_file,int registered_line) //unsigned int offset)
	{
		// to make compatible MSVC shared library
		//std::vector<ScriptClass *> * script_classes = getVecScriptClassNode();

		std::string var_type = typeid(R *).name(); // we need the pointer type ...
		std::string return_type;
		//std::vector<std::string> params;
		std::string str_class_name_ptr = typeid( C *).name();
		unsigned int ref_ptr=offsetOf<C>(var_pointer);

		ScriptClass *c_class = getScriptClassBy_C_ClassPtr(str_class_name_ptr);

		if(c_class == NULL){
			return false;
		}

		// 1. check all parameters ok.
		// check valid parameters ...
		if(getIdxClassFromIts_C_Type(var_type) == -1){
			THROW_RUNTIME_ERROR(zs_strutils::format("%s::%s has not valid type (%s)"
					,c_class->symbol_info.symbol->name.c_str()
					,var_name
					,zs_rtti::demangle(typeid(R).name()).c_str()));
			return false;
		}

		// register variable...
		c_class->registerVariable(registered_file,registered_line,var_name,var_type,ref_ptr,SYMBOL_INFO_PROPERTY_C_OBJECT_REF);
		return true;

	}

}