#pragma once


#define STR_EXPAND(tok) #tok
#define STR(tok) STR_EXPAND(tok)
#define ZS_STRCMP(a, R, b) (strcmp(a,b) R 0)
#define ARRAY_LENGTH(s) (sizeof(s)/sizeof(s[0]))

namespace zetscript{
	namespace zs_strutils{

		typedef enum{
			NUMBER_TYPE_INVALID=0,
			NUMBER_TYPE_HEXA,
			NUMBER_TYPE_INT,
			NUMBER_TYPE_DOUBLE
		}NumberType;

		 bool  * parse_bool(const std::string & );
		 int   * parse_int(const std::string & );
		 float * parse_float(const std::string & );


		 std::string int_to_str(int number);
		 std::string to_lower(const std::string & str);
		 std::string to_upper(const std::string & str);
		 std::wstring to_wstring(const std::string &s);
		  std::string  format(const  char  *string_text, ...);

		 bool ends_with(const std::string & fullString, const std::string & ending);

		  std::vector<std::string> split(const std::string &s, char delim, std::vector<std::string> &elems);
		  std::vector<std::string> split(const std::string &s, char delim);
		 bool isEmpty(const std::string & str);
		 NumberType is_number(const std::string & test_str);
		 bool 		 is_binary(const std::string & test_str);

		 std::string remove(std::string & str_old, char ch_to_remove);

		 int count(const std::string & s,char c);

		/**
		 * Given two pointers within that points within a std::string, this function copies std::string between its interval.
		 * @p1:start pointer
		 * @p2:end pointer
		 */
		bool copy_from_ptr_diff(std::string & str_dst,const char *p1, const char *p2);

	}
}
