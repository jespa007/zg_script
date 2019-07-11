namespace zetscript{
	namespace string_utils{

		enum{
			MAX_STRING_BUFFERS=256,
			MAX_LENGTH_BUFFER=512,
			MAX_BUFFER_COPY_FROM_INTERVAL=4096
		};


		enum{
			STRING_IS_INVALID_NUMBER=0,
			STRING_IS_HEXA,
			STRING_IS_INT,
			STRING_IS_DOUBLE
		};

		char m_buffer[MAX_STRING_BUFFERS][MAX_LENGTH_BUFFER]={{0}};
		char aux_str_copy[MAX_BUFFER_COPY_FROM_INTERVAL] = {0};
		unsigned char m_index_buffer=0;
		char  _sformat_buffer[4096] = { 0 };

		const char *  sformat(const  char  *input_text, ...){


			va_list  ap;
			va_start(ap,  input_text);
			vsprintf(_sformat_buffer,  input_text,  ap);
			va_end(ap);

			return _sformat_buffer;
		}




		int *  parse_intenger(const std::string & val){

			int *n=NULL;
			int type_number = is_number(val);
			int numberValue=0;
			if(type_number == STRING_IS_INT){
				numberValue=strtol(val.c_str(), NULL, 10);
			}else if(type_number == STRING_IS_HEXA){
				numberValue=strtol(val.c_str(), NULL, 16);
			}else if(is_binary(val)){
				std::string binary = val.substr(0,val.size()-1);
				numberValue=strtol(binary.c_str(), NULL, 2);
			}
			else{

				return NULL;
			}

			n=new int;
			*n = numberValue;


			// TODO: develop exception handler.
			return n;
		}

		bool * parse_boolean(const std::string & s){

			if(to_lower(s)=="true"){
				bool *b=new bool;
				*b=true;
				return b;

			}else if(to_lower(s)=="false"){
				bool *b=new bool;
				*b=false;
				return b;
			}

			// TODO: develop exception handler.
			return NULL;
		}

		float *  parse_float(const std::string & s){
			char *p;bool ok=true;
			float *n=NULL;

			float numberValue=0;
			if(s!="0") {// trivial case
			  numberValue=strtof((char *)s.c_str(),&p);
			  ok = *p == '\0';
			}

			if(ok){
				n=new float;
				*n = numberValue;

			}

			// TODO: develop exception handler.
			return n;
		}


		char * copy_from_pointer_diff(const char *p1, const char *p2){

			if(p1 == NULL || p2 == NULL){
				THROW_RUNTIME_ERROR(sformat("NULL entry (%p %p)",p1,p2));
				return NULL;
			}

			int var_length=p2-p1;

			if(var_length < 0 || var_length >= (MAX_BUFFER_COPY_FROM_INTERVAL+1)){
				THROW_RUNTIME_ERROR(sformat("array out of bounds (Max:%i Min:%i Current:%i)",0,MAX_BUFFER_COPY_FROM_INTERVAL,var_length));
				return NULL;
			}

			if(p1 == p2){
				THROW_RUNTIME_ERROR("Nothing to copy");
				return NULL;
			}

			memset(aux_str_copy,0,sizeof(aux_str_copy));
			strncpy(aux_str_copy,p1,var_length);

			return aux_str_copy;
		}


		std::string int_2_string(int number){

		   std::stringstream ss;//create a stringstream
		   ss << number;//add number to the stream
		   return ss.str();//return a std::string with the contents of the stream
		}

		std::string float_2_string(float number){

			char buff[100];
			sprintf(buff, "%f",number);

			std::string ss = buff;
			ss = replace(ss,',','.');

		   return ss;//return a std::string with the contents of the stream
		}

		std::string double_2_string(double number){

			char buff[100];
			sprintf(buff, "%f",number);
			std::string ss=buff;

			ss=replace(ss,',','.');


		   return ss;//return a std::string with the contents of the stream
		}

		std::string to_lower(const std::string & str){

			std::string ret = str;
			for(unsigned short l = 0; l < ret.size();l++)
				ret[l] = tolower(ret[l]);
			return ret;
		}

		std::string to_upper(const std::string & str){

			std::string ret = str;
			for(unsigned short l = 0; l < ret.size();l++)
				ret[l] = toupper(ret[l]);
			return ret;
		}

		std::wstring to_wstring(const std::string& s)
		{
			std::wstring wsTmp(s.begin(), s.end());
			return wsTmp;
		}
		//------------------------------------------------------------------------------------------------------------------------

		std::vector<std::string> split(const std::string &s, char delim, std::vector<std::string> &elems) {
			std::stringstream ss(s);
			std::string item;
			while (std::getline(ss, item, delim)) {
				elems.push_back(item);
			}
			return elems;
		}


		std::vector<std::string> split(const std::string &s, char delim) {
			std::vector<std::string> elems;
			split(s, delim, elems);
			return elems;
		}

		bool is_empty(const std::string & str){
			return str.empty();
		}

		bool ends_with(const std::string & fullString, const std::string & ending){
			if (fullString.length() >= ending.length()) {
				return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
			}

			return false;
		}


		bool is_digit(char c){
			return (('0' <= c) && (c<='9'));
		}


		bool is_hexa_digit(char c){
			return ((('0' <= c) && (c<='9')) || ('a'<=(tolower(c))&&(tolower(c)<='f')));
		}

		char *advance_digits(char *aux_p){

			while(is_digit(*aux_p))	aux_p++;
			return aux_p;
		}

		char *advance_hexadigits(char *aux_p){

			while(is_hexa_digit(*aux_p))	aux_p++;
			return aux_p;
		}

		bool is_binary(const std::string & test_str_number){


			char *start_p=(char *)test_str_number.c_str();
			char *aux_p =start_p;

			while(is_digit(*aux_p)){
				if(!(*aux_p=='0' || *aux_p=='1')){
					break;
				}else{
					aux_p++;
				}
			}

			return (*aux_p=='b' || *aux_p=='B') && (aux_p > start_p);

		}

		int is_number(const std::string & test_str_number){
			bool isHexa=false;
			char *str = (char *)test_str_number.c_str();

			switch(*str){
			case '-': str++; // negative numbers ...
					   break;
			case '0':
					  if(tolower(*(str+1))=='x')  {
						  isHexa = true;
						  str+=2;
					  }
					  break;
			default:
					break;
			};

			char *start_str = str; // saves position...
			if(isHexa) {
				str = advance_hexadigits(str);
				if(str == start_str)
					return STRING_IS_INVALID_NUMBER;

				if(*str == ' ' || *str == 0) return STRING_IS_HEXA;
			}else{

				str = advance_digits(str);
				if(*str=='.') { // is candidate to double

					str++;
					str = advance_digits(str);

					if(*str != 'e'){
						if(*str == ' ' || *str == 0)
							return STRING_IS_DOUBLE;
					}
				}

				if(*str == 'e'){  // is candidate to double

					str++;

					if(*str == '+' ||*str == '-'){
						str++;
					}

					str = advance_digits(str);
					if(*str == ' ' || *str == 0)
						return STRING_IS_DOUBLE;

					return STRING_IS_INVALID_NUMBER;
				}

				if(*str == ' ' || *str == 0)
					return STRING_IS_INT;

			}

			return STRING_IS_INVALID_NUMBER;


		}

		std::string  replace(const std::string & str_old, const char old_ch, char new_ch){
			std::string str = str_old;
			for (unsigned i = 0; i < str.length(); ++i) {
				if (str[i] == old_ch)
				  str[i] = new_ch;
			}

			return str;
		}

		void replace(std::string & str_input, const std::string & toReplace, const std::string & replaceWith){

			std::size_t found;;
			while((found = str_input.find(toReplace)) != std::string::npos){
				str_input = (str_input.replace(found, toReplace.length(), replaceWith));
			}


		}

		std::string  remove(std::string & str_old, char ch_to_remove){
			std::string str = str_old;
			std::string str_new="";

			for (unsigned i = 0; i < str_old.length(); ++i) {
				if (str_old[i] != ch_to_remove)
						str_new+=str_old[i];
			}

			return str_new;
		}

		int count(const std::string & s,char c){
			int n_items=0;

			for(unsigned i=0; i < s.size(); i++)
				if(s[i] == c)
					n_items++;

			return n_items;
		}
	}


}