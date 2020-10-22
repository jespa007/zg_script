/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "zetscript.h"

//#include "zetscript.h"

/*

Constants (C)
-------------

C <= Integer,Number,Boolean,String


Pre operators (Pe)
-------------

Pe <= [ --, ++ ]

Post operators (Po)
-------------

Po <= [ --, ++ ]


Symbol (S)
-------------

S <=  [A-Za-Z ][A-Za-z0-9 ]*  --> token variable
S <=  [0-9]*.[0-9]* | [e|E][-|+][0-9]*  --> token integer or number
S <=  0x[0-9a-fA-F]* --> token hexadecimal
S <=  [0-1]*b --> token binary
S <= "[all characters]" --> token std::string


Punctuator (P)
-------------

Arithmetic

P <= [  /, *, %, -, + ]

Bit

P <= [  &, |, ^, <<, >> ]

Logical

P <= [  ==, >=, <=, >, <, !=, &&, || ]

Assignments

P <= [  =, +=, -=, /=, %=, *=, <<=, >>=, ^=, |=, &=]

Object

P <= [  . ]

Ternary

P <= [  ?,: ]


Anonymous Objects (O)
-------------

O <= [E1,E2,E3,...,En] 
O <= function(a1,a2,...,an){ B }
O <= {S1:E1,S2:E2,...,Sn:Em}

Expression (E)
-------------

E <= (E) // parentesis E
E <= E P E // punctuator
E <= !E // negate result of expression
E <= -E // Prerest (negate result of expression)
E <= +E // Presum (it does nothing)
E <= [Pe]S[Po] // variable with/out pre/post operator.
E <= (S|O)[E1][E2]...[En] // array access
E <= (S|O)(E1,E2,..,En) // function call
E(t)<= E; | E,


Keyword (K)
-------------

- var S=E;
- for(var S1=Ei1,S2=Ei2,...,Sn=Eim;Ec;Ep1,Ep2,Ep3,...,Epk){ B } //
- for(var S1 in S2) { B } //
- while(Ec) { B }
- do{ B } while(Ec);
- switch(E){ // switch should have constants on case
	default:
	case C0:
		� break;
	case C1:
		� break;
		� ..
	case CN:
		� break;
	}

-if(Ec){ B1 } else if (Ec2) { B2 } else { B3 }


Body (B)
----------

Starts with '{' and ends with '}'

B <- [E;|K]* // A set of expressions ended with ; or Keyword

*/



#ifdef  __ZETSCRIPT_VERBOSE_MESSAGE__

#define print_eval_cr ZS_PRINT_DEBUG
#else
#define print_eval_cr(s,...)
#endif

#include "eval_data.cpp"
#include "eval_object.cpp"
#include "eval_expression.cpp"
#include "eval_keyword.cpp"

namespace zetscript{

	namespace eval{

		void eval(ZetScript *zs,const char * str, const char *  _filename, int _line){
			EvalData *eval_data=new EvalData(zs);
			int line =_line;
			eval_data->current_parsing_file=_filename;
			char *end_char = NULL;

			push_function(eval_data,MAIN_FUNCTION(eval_data));
			end_char=eval_recursive(eval_data,str,line,MAIN_SCOPE(eval_data));
			pop_function(eval_data);

			delete eval_data;

			if(*end_char != 0){
				THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"Unexpected \'%c\' ",*end_char);
			}

		}

		Scope * eval_new_scope(EvalData *eval_data, Scope *scope_parent, bool is_function){
			Scope *new_scope = NEW_SCOPE(eval_data,scope_parent);
			scope_parent->registered_scopes->push_back((intptr_t)new_scope);
			new_scope->is_scope_function=is_function;
			if(is_function){
				new_scope->tmp_idx_instruction_push_scope=0;
			}
			else{
				new_scope->tmp_idx_instruction_push_scope=(int)eval_data->current_function->instructions.size();
			}

			return new_scope;
		}

		void inc_jmp_codes(EvalData *eval_data, int idx_start_instruction, int idx_end_instruction, unsigned inc_value){

			if(idx_end_instruction>(int)eval_data->current_function->instructions.size()){
				THROW_RUNTIME_ERROR("idx_end_instruction out of bounds ");
			}
			EvalInstruction **it=&eval_data->current_function->instructions[idx_start_instruction];
			size_t size = eval_data->current_function->instructions.size();

			for(unsigned i=idx_start_instruction; i < size; i++,it++){
				switch((*it)->vm_instruction.byte_code){
				case BYTE_CODE_JMP:
				case BYTE_CODE_JNT:
				case BYTE_CODE_JE:
				case BYTE_CODE_JT:
					(*it)->vm_instruction.value_op2+=inc_value;
					break;
				default: // ignore
					break;
				}
			}
		}

		void eval_check_scope(EvalData *eval_data, Scope *scope, unsigned idx_instruction_start){
			if(scope->n_registered_symbols_as_variables > 0){ // if there's local symbols insert push/pop scope for there symbols
				if(scope->tmp_idx_instruction_push_scope!=ZS_IDX_UNDEFINED){
					eval_data->current_function->instructions.insert(
							eval_data->current_function->instructions.begin()+scope->tmp_idx_instruction_push_scope
							,new EvalInstruction(BYTE_CODE_PUSH_SCOPE,0,(intptr_t)scope)
					);

					inc_jmp_codes(eval_data,idx_instruction_start,(int)(eval_data->current_function->instructions.size()),1);

					// and finally insert pop scope
					eval_data->current_function->instructions.push_back(new EvalInstruction(BYTE_CODE_POP_SCOPE,0));
				}
			}
			else{ // remove scope
				scope->markAsUnusued();
			}
		}

		char * eval_block(EvalData *eval_data,const char *s,int & line,  Scope *scope_info){
			// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
			char *aux_p = (char *)s;

			Scope *new_scope_info=  NULL;
			IGNORE_BLANKS(aux_p,eval_data,aux_p,line);

			// check for keyword ...
			if(*aux_p == '{'){

				int idx_instruction_start_block=(int)(eval_data->current_function->instructions.size());

				aux_p++;

				new_scope_info = eval_new_scope(eval_data,scope_info); // special case... ast is created later ...

				if((aux_p = eval_recursive(
						eval_data
						,aux_p
						, line
						, new_scope_info
				)) != NULL){

					if(*aux_p != '}'){
						THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"Expected '}' ");
					}

					eval_check_scope(eval_data,new_scope_info,idx_instruction_start_block);
					return aux_p+1;
				}
			}
			return NULL;
		}

		char * eval_recursive(EvalData *eval_data,const char *s, int & line, Scope *scope_info, bool return_on_break_or_case){
			// PRE: *node_to_be_evaluated must be created (the pointer is only read mode)
			bool custom_quit = false;
			char *aux = (char *)s;
			char *end_expr=0;

			Keyword keyw;
			Directive directive;

			IGNORE_BLANKS(aux,eval_data,aux, line);

			while(*aux != 0 && !custom_quit){

				// ignore all ;
				while(*aux==';' && *aux != 0){
					IGNORE_BLANKS(aux,eval_data,aux+1, line);
				}

				if(*aux==0){ // custom case exit..
					return aux;
				}

				keyw=is_keyword(aux);

				if(*aux == '}'){ // ending block trivial cases...
					return aux;
				}else if(keyw != Keyword::KEYWORD_UNKNOWN && keyw < KEYWORDS_WITHIN_EXPRESSIONS){ // it cuts current expression to link breaks...

					if(((keyw == Keyword::KEYWORD_BREAK) || (keyw == Keyword::KEYWORD_CASE)) && return_on_break_or_case){
						return aux;
					}

					end_expr = eval_keyword(
						eval_data
						,aux
						, line
						, scope_info
						, keyw
					);

				}else if((directive = is_directive(aux)) != Directive::DIRECTIVE_UNKNOWN){ // eval directive
					// try directive ...
					char *start_var,* end_var;
					std::string str_symbol;
					if(directive != Directive::DIRECTIVE_UNKNOWN){
						switch(directive){
						case DIRECTIVE_INCLUDE:
							aux += strlen(eval_info_directives[directive].str);
							IGNORE_BLANKS(aux,eval_data,aux,line);
							if(*aux != '\"'){
								THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"expected starting \" directive");
							}
							aux++;
							start_var=aux;

							while(*aux != '\n' && *aux!=0 && !(*aux=='\"' && *(aux-1)!='\\')) aux++;

							if(*aux != '\"'){
								THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"expected end \" directive");
							}

							end_var=aux;

							zs_strutils::copy_from_ptr_diff(str_symbol,start_var,end_var);

							ZS_PRINT_DEBUG("include file: %s",str_symbol);

							// save current file info...
							eval_data->zs->evalFile(str_symbol);

							aux++;// advance ..
							break;
						default:
							THROW_SCRIPT_ERROR(eval_data->current_parsing_file,line,"directive \"%s\" not supported",eval_info_directives[directive].str);
							break;
						}

						end_expr=aux;
					}
				}else if(*aux == '{') { // eval block

					// 2nd. check whether eval a block
					end_expr = eval_block(
						eval_data
						,aux
						,line
						, scope_info
					);

				}else{ // eval expression

					end_expr = eval_expression(
						eval_data
						,aux
						,line
						, scope_info
						,&eval_data->current_function->instructions
					);
				}

				aux=end_expr;
				IGNORE_BLANKS(aux,eval_data,aux, line);
			}
			return aux;
		}

		void push_function(EvalData *eval_data,ScriptFunction *script_function){
			eval_data->functions.push_back(eval_data->current_function=new EvalFunction(script_function));
		}

		void pop_function(EvalData *eval_data){
			eval_data->current_function->script_function->instructions=NULL;

			// get total size op + 1 ends with NULL
			size_t size = (eval_data->current_function->instructions.size() + 1) * sizeof(Instruction);
			eval_data->current_function->script_function->instructions = (PtrInstruction)malloc(size);
			memset(eval_data->current_function->script_function->instructions, 0, size);

			for(unsigned i=0; i < eval_data->current_function->instructions.size(); i++){

				Symbol *vis=NULL;
				EvalInstruction *instruction = eval_data->current_function->instructions[i];
				LinkSymbolFirstAccess *ls=&instruction->link_symbol_first_access;

				if(  (ls->idx_script_function != ZS_IDX_UNDEFINED)
				  && (instruction->vm_instruction.byte_code == ByteCode::BYTE_CODE_LOAD_TYPE_FIND) // try to define...
				){ // solve first symbol first access...

					ScriptFunction *sf=GET_SCRIPT_FUNCTION(eval_data,ls->idx_script_function);
					ScriptClass *sc = GET_SCRIPT_CLASS(eval_data,sf->idx_class);

					if(instruction->vm_instruction.properties & MSK_INSTRUCTION_PROPERTY_ACCESS_TYPE_THIS){ // search the symbol within class.

						if(instruction->is_symbol_super_method){ // trivial super.
							Symbol *symbol_sf_foundf=NULL;
							std::string str_symbol_to_find = sf->symbol.name;

							for(int i = sf->symbol.idx_position-1; i >=0 && symbol_sf_foundf==NULL; i--){
								Symbol *symbol_member = (Symbol *)sc->symbol_members->items[i];
								if(symbol_member->properties & SYMBOL_PROPERTY_IS_FUNCTION){
									ScriptFunction *sf=(ScriptFunction *)symbol_member->ref_ptr;
									bool match_params=(symbol_member->properties & SYMBOL_PROPERTY_C_OBJECT_REF?str_symbol_to_find == symbol_member->name:true);
									if(
											(sf->symbol.name == str_symbol_to_find)
										&& (match_params)
										){
										symbol_sf_foundf = symbol_member;
									}
								}
							}

							// ok get the super function...
							if(symbol_sf_foundf == NULL){
								THROW_SCRIPT_ERROR(instruction->instruction_source_info.file,instruction->instruction_source_info.line,"Cannot find parent function %s::%s",sf->symbol.name.c_str(),ls->value.c_str());
								return;
							}
							instruction->vm_instruction.byte_code=BYTE_CODE_LOAD_TYPE_VARIABLE;
							instruction->vm_instruction.value_op2=symbol_sf_foundf->idx_position;
							instruction->instruction_source_info.str_symbol =get_compiled_symbol(eval_data,str_symbol_to_find);
							instruction->vm_instruction.properties=MSK_INSTRUCTION_PROPERTY_ACCESS_TYPE_THIS;

						}else{ // this symbol

							// is automatically created on vm...
							if(ls->n_params==NO_PARAMS_SYMBOL_ONLY){ // it will search at runtime
								instruction->vm_instruction.value_op2=ZS_IDX_UNDEFINED;//vis->idx_symbol;
							}
							else{
								Symbol *symbol_function=sc->getSymbol(ls->value,NO_PARAMS_SYMBOL_ONLY);
								if(symbol_function==NULL){
									THROW_SCRIPT_ERROR(instruction->instruction_source_info.file,instruction->instruction_source_info.line,"Cannot find function %s::%s",sc->symbol.name.c_str(),ls->value.c_str());
									return;
								}

								// it stores the script function in the op...
								instruction->vm_instruction.value_op2=symbol_function->idx_position;
							}
						}
					}else{ // find locally, if not will be resolved at runt-time //else if(sf->existArgumentName(ls->value)==ZS_IDX_UNDEFINED){ // search symbol within local or global  ...

						bool local_found=false;
						ScriptFunction *script_function_found=NULL;
						//LoadType load_type=LoadType::LOAD_TYPE_NOT_DEFINED;
						std::string symbol_to_find = ls->value;

						// try find local symbol  ...
						Scope *scope=ls->scope;
						Symbol * sc_var = scope->getSymbol(ls->value, NO_PARAMS_SYMBOL_ONLY,ScopeDirection::SCOPE_DIRECTION_DOWN);

						if(sc_var != NULL){ // local symbol found

							if(sc_var->n_params==NO_PARAMS_SYMBOL_ONLY){ // symbol is variable...
								if((vis=sf->getSymbol(sc_var->scope,ls->value))!=NULL){
									instruction->vm_instruction.byte_code=BYTE_CODE_LOAD_TYPE_VARIABLE;
									instruction->vm_instruction.value_op2=vis->idx_position;
									instruction->vm_instruction.properties |=MSK_INSTRUCTION_PROPERTY_ACCESS_TYPE_LOCAL;
									local_found=true;
								}
							}
							else{ // symbol is function...
								Symbol *function_symbol=sf->getSymbol(sc_var->scope,ls->value,NO_PARAMS_SYMBOL_ONLY);
								if(function_symbol!=NULL){
									script_function_found=(ScriptFunction *)function_symbol->ref_ptr;
									instruction->vm_instruction.byte_code=BYTE_CODE_LOAD_TYPE_FUNCTION;
									instruction->vm_instruction.value_op2=function_symbol->idx_position; // store script function
									instruction->vm_instruction.properties |=MSK_INSTRUCTION_PROPERTY_ACCESS_TYPE_LOCAL;
									local_found =true;
								}
							}
						}

					}
				}

				// save instruction ...
				eval_data->current_function->script_function->instructions[i]=instruction->vm_instruction;

				//------------------------------------
				// symbol value to save at runtime ...
				InstructionSourceInfo instruction_info=instruction->instruction_source_info;

				// Save str_symbol that was created on eval process, and is destroyed when eval finish.
				if(instruction->instruction_source_info.str_symbol != NULL){
					instruction_info.str_symbol=new std::string(*instruction->instruction_source_info.str_symbol);
				}

				eval_data->current_function->script_function->instruction_source_info[i]=instruction_info;
			}

			// delete and popback function information...
			delete eval_data->current_function;
			eval_data->functions.pop_back();

			eval_data->current_function = NULL;
			if(eval_data->functions.size() > 0){
				eval_data->current_function = eval_data->functions.at(eval_data->functions.size()-1);
			}
		}
	}
}
