#include "script/zg_script.h"

CInteger * CInteger::Parse(const string & val){

	CInteger *n=NULL;
	int type_number = CStringUtils::isNumber(val);
	int numberValue=0;
	if(type_number == CStringUtils::STRING_IS_INT){
		numberValue=strtol(val.c_str(), NULL, 10);
	}else if(type_number == CStringUtils::STRING_IS_INT){
		numberValue=strtol(val.c_str(), NULL, 16);
	}else{
		return NULL;
	}


   	n=NEW_INTEGER();
   	n->m_value = numberValue;

	// TODO: develop exception handler.
	return n;
}

int *  CInteger::ParsePrimitive(const string & val){

	int *n=NULL;
	int type_number = CStringUtils::isNumber(val);
	int numberValue=0;
	if(type_number == CStringUtils::STRING_IS_INT){
		numberValue=strtol(val.c_str(), NULL, 10);
	}else if(type_number == CStringUtils::STRING_IS_HEXA){
		numberValue=strtol(val.c_str(), NULL, 16);
	}else{
		return NULL;
	}

    n=new int;
    *n = numberValue;


	// TODO: develop exception handler.
	return n;
}


CInteger::CInteger(){ m_value=0;}
CInteger::CInteger(float v){m_value=v;}

string CInteger::toString(){string s; return s;}