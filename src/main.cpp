#include "CZetScript.h"
#include <sys/time.h>
#define N_INTS 1000000000

int print_hello(int hh){
	return hh+1;
}

int main(int argc, char * argv[]){

/*	vector<char> vp1;
	char *vp2= (char *)malloc(N_INTS);
	struct timeval start, end;


	for(int i = 0; i < N_INTS; i++){
		vp1.push_back(0);
	}

	gettimeofday(&start, NULL);
	int result=0;
	// compare method 1...
	for(int i = 0; i < N_INTS; i++){

		for(int j = 0; j < N_INTS; j++){

			for(int k = 0; k < N_INTS; k++){
				result+=print_hello(vp1[i]+vp1[j]+vp1[k]+1);
			}
		}
	}




	gettimeofday(&end, NULL);
	printf("\nTime vector:%lu",end.tv_usec-start.tv_usec);
	gettimeofday(&start, NULL);


	// compare method 2...
	for(int i = 0; i < N_INTS; i++){

		for(int j = 0; j < N_INTS; j++){

			for(int k = 0; k < N_INTS; k++){
				result+=print_hello(vp2[i]+vp2[j]+vp2[k]+1);
			}
		}
	}

	gettimeofday(&end, NULL);
	printf("\nTime array_c:%lu",end.tv_usec-start.tv_usec);
	return 0;*/
/*
	print_info_cr("sizeof(short):%i sizeof(tInfoAsmOp):%i",sizeof(short),sizeof(tInfoAsmOp));
	print_info_cr("sizeof(short):%i sizeof(CVirtualMachine):%.04fMB",sizeof(short),float(sizeof(CVirtualMachine))/(1024*1024));
	print_info_cr("sizeof(short):%i sizeof(CAstNode):%.04fMB",sizeof(short),float(sizeof(CASTNode))/(1024*1024));
	print_info_cr("sizeof(short):%i sizeof(CCompiler):%.04fMB",sizeof(short),float(sizeof(CCompiler))/(1024*1024));
	print_info_cr("sizeof(short):%i sizeof(CScope):%.04fMB",sizeof(short),float(sizeof(CScope))/(1024*1024));
	print_info_cr("sizeof(short):%i sizeof(CScriptVariable):%.04fMB",sizeof(short),float(sizeof(CScriptVariable))/(1024*1024));
	print_info_cr("sizeof(short):%i sizeof(CScriptClass):%.04fMB",sizeof(short),float(sizeof(CScriptClass))/(1024*1024));
	return 0;
*/
	CLog::setUseAnsiEscape(true);
	CZetScript *zet_script = CZetScript::getInstance();








	if(argc < 2){
		printf("Put file to evaluate.\n");
		printf("\n");
		printf("Example:\n");
		printf("\n");
		printf("file.zs");
		printf("\n");
		printf("\n");
		return 0;
	}

	char *data_buffer = argv[1];
	ByteBuffer *buffer = NULL;


	if((buffer = CIO_Utils::readFile(argv[1]))!=NULL){

		data_buffer=(char *)buffer->data_buffer;
	}

	if(zet_script->eval(data_buffer)){
		zet_script->execute();


		auto f=zet_script->script_call("rec");

		if(f != NULL){
			CScriptVariable *r;
			CInteger *iu = new CInteger(10);
			r = (*f)({iu});
			if(r != UNDEFINED_SYMBOL){//CScriptVariable::UndefinedSymbol){
				delete r;
			}


			delete iu;
			delete f;
		}
	}

	if(buffer != NULL){
		delete buffer;
	}


	CZetScript::destroy();



#if defined(__DEBUG__) && defined(__MEMMANAGER__)
  MEM_ViewStatus();
#endif

	return 0;
}
