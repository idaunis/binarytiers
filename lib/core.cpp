/* 
** Copyright (C) 2012 Uselabs and/or its subsidiary(-ies).
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
** http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Uselabs
*/

#include "core.h"
#include "node.h"
#include "taxonomy.h"
#include "profile.h"
#include "ckeditor.h"
#include "upload.h"
#include "translation.h"
#include "locale.h"
#include "system.h"
#include "menu.h"
#include "path.h"
#include "comment.h"
#include "aggregator.h"
#include "zip/GZipHelper.h"
#include "md5/md5.h"
#include "md5/sha1.h"

#include <sys/types.h>
#include <sys/stat.h>

#include "gnu/parsedate.h"

#define PCRE_STATIC
#include "pcre/pcre.h"
#include "regex/regex.h"

#ifdef FASTCGI
//#include "fcgio.h"
	#include "fcgi_stdio.h"
#endif

#ifdef _MSC_VER
	#include <io.h>
	#include <fcntl.h>
	void set_binary(FILE *fp)
	{
	#ifdef FASTCGI
		_setmode( _fileno( fp->stdio_stream ), O_BINARY);
	#endif
	#ifndef FASTCGI
		_setmode(_fileno(fp), O_BINARY);
	#endif	
	}
	#define close closesocket
#endif
#ifndef _MSC_VER
	#include <sys/socket.h>
	#include <sys/types.h>
	#include <netdb.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	void set_binary(FILE *fp){}
#endif

#include <openssl/rand.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

// Simple structure to keep track of the handle, and
// of what needs to be freed later.
typedef struct {
    int socket;
    SSL *sslHandle;
    SSL_CTX *sslContext;
} SSLConnection;

bool disable_errors = false;
time_t timer_t1, timer_t2;

vector <AssocOf(Assoc) > schema;
vector <Assoc > controllers;
map <string, map<string,string> > _ELEMENTS;
map <string, void (*)(void)> _HOOKS;
map <string, string (*)( map<string,string> ) > _THEME_HOOKS;
map <string, bool (*)(string)> _ACCESS_CALLBACKS;
map <string, string (*)(void)> _CALLBACKS;
map <string, string (*)(void)> _SUBMITS;
map <string, bool (*)(void)> _VALIDATES;

/**
 * _FILES is an associative array of the files uploaded via the HTTP POST method.
 * @par Example:
 * @code
 * string out;
 * out += "Mime Type: "+_FILES["image"]["type"];
 * out += "File Size: "+_FILES["image"]["size"];
 * out += "Temporary Path: "+_FILES["image"]["tempname"];
 * out += "File Name: "+_FILES["image"]["filename"];
 * print( out );
 * @endcode @hideinitializer @ingroup global
 */
map <string, Assoc> _FILES;

/** _POST is an associative array of variables sent to BinaryTiers via the HTTP POST method.
 * @par Example:
 * @code
 * print("Hello " + _POST["name"] + "!");
 * @endcode @hideinitializer @ingroup global
 */
Assoc _POST; 
Assoc _GET; /**< _GET is an associative array of variables sent to BinaryTiers via the URL parameters. @hideinitializer @ingroup global*/

/** _COOKIE is an associative array of variables sent to BinaryTiers via HTTP Cookies. 
 * @par BinaryTiers System Cookies:
 * \n<table>
 * <tr><th>System Cookies</th><th>Usage</th></tr>
 * <tr><td>_COOKIE["BTFID"]</td><td>Specifies a File ID for tracking the uploading progress</td></tr>
 * <tr><td>_COOKIE["BTMCL"]</td><td>Specifies a new value for max_content_length when uploading files</td></tr>
 * <tr><td>_COOKIE["BTLANG"]</td><td>Set language for not registered users</td></tr>
 * <tr><td>_COOKIE["SESS"]</td><td>Session ID</td></tr>
 * </table> 
 * @hideinitializer @ingroup global
 */
Assoc _COOKIE;
map <string, int> sent;
map <string, string> conf;
map <string, map<string,string> > node_types;
map <int, map<string,bool> > perm;
int sent_location = 0;
string stored_head;
string stored_title;
map <string, string> stored_extra;
map <string, string> language;
vector <string> _MODELS;
vector <string> _THEMES;
map <string, map<string,string> > _PLUGINS_STRINGS;
vector <string> _TEMP_FILES;
vector <string> _ERROR_MESSAGES;
vector <string> _STATUS_MESSAGES;
int _TEMP_FILES_COUNT = 0;
int _ACTIVE_CONTROLLER = -1;

char get_host_data[256];
int _CONTENT_LENGTH = 0;
string _CONTENT_TYPE = "";
bool htmlout = false;
bool load_form_only = false;
bool validate_form = false;

string cur_form_id;
map <string, map<string,string> > cur_form;
list < pair<int, string> > cur_order;
map <string, string> cur_node;
vector <map <string,string> > cur_files;
map <string, string> user;

int pager_total_items = 0;
int	pager_page = 0;
int	pager_total = 0;

string	main_output;
string	help;

MYSQL mysql;
redisContext * redis = NULL;

string install_submit();
bool install_validate();
string install();
string watchdog_overview();
string theme_menu_tree(int pid );

string form_fieldset( map <string, string> *item, map<string, map<string,string> > &form);
string form_render(string item,  map<string, map<string,string> > &form);
string form_radio( map <string, string> *item);

regex_t preg;

void debug( string str )
{
	printf( "Content-Type: text/html; charset=utf-8\r\n\r\n" );
	printf( "%s", str.c_str() );
}


/**
 * The add_html_head() function adds string pointed by html_code to the global variable stored_head that will be used when rendering the page.
 * @param data Single line of HTML code.
 * @returns None
 * @par Example:
 * @code
 * add_html_head("<script type=\"text/javascript\" src=\"etc/jquery.js\"></script>");
 * @endcode
 * @ingroup group7
 */
void add_html_head( string data )
{
	stored_head += data + "\n";
}

/**
 * The get_page_title() function obtains the current page title.
 * @returns	Returns the current page title.
 * @par Example:
 * @code
 * string title;
 * title = get_page_title();
 * @endcode
 * @ingroup group7
 */
string get_page_title()
{
	return stored_title;
}

/**
 * The set_page_title() function sets the current page title.
 * @param title String containing the title of the page.
 * @returns	None.
 * @par Example:
 * @code
 * set_page_title("Home Page");
 * @endcode
 * @ingroup group7
 */
void set_page_title(string title)
{
	stored_title = title;
}

/**
 * The get_page_extra() function obtains in an associative array pointed by item the extra fields that are passed to the page_theme hook when rendering a page.
 * @param item Associative array that receives the extra fields.
 * @returns	None.
 * @par Example:
 * @code
 * map <string, string> extra;
 * get_page_extra( extra );
 * extra["top_banner"] += "<img src=\"img/banner.jpg\">";
 * set_page_extra("", extra);
 * @endcode
 * @ingroup group7
 */
void get_page_extra( map <string, string> &item )
{
	for( map <string, string>::iterator curr = stored_extra.begin(), end = stored_extra.end();  curr != end;  curr++ ) 
	{
		item[ curr->first ] = curr->second;
	}
}

/**
 * The set_page_extra() function sets the extra fields that are passed to the page_theme interceptor when rendering a page. The extra fields are sent by an associative array pointed by item. Additionally a prefix might by specified by the string prefix to create a group of fields with the same prefix. This function might be also used to overwrite existing fields for the page_theme interceptor.
 * @param prefix Text string to be attached at the front of each name of the field.
 * @param extra Associative array with the existing extra fields.
 * @returns	None.
 * @par Example:
 * @code
 * map <string, string> extra;
 * get_page_extra( extra );
 * extra["top_banner"] += "<img src=\"img/banner.jpg\">";
 * set_page_extra("", extra);
 * @endcode
 * @ingroup group7
 */
void set_page_extra(string prefix, map <string, string> extra)
{
	for( map <string, string>::iterator curr = extra.begin(), end = extra.end();  curr != end;  curr++ ) 
	{
		stored_extra[ prefix + curr->first ] = curr->second;
	}
}

void add_css(string file, bool nocache) 
{
	string postfix = nocache ? "?"+str( time() ) : "";

	if ( !sent[file] ) 
	{
		add_html_head("<link rel=\"stylesheet\" media=\"all\" type=\"text/css\" href=\""+ file + postfix +"\" />");
		sent[file] = true;
	}
}

void add_js(string file, bool nocache) 
{
	string postfix = nocache ? "?"+str( time() ) : "";

	if ( !sent["etc/jquery.js"] ) 
	{    
		add_html_head("<script type=\"text/javascript\" src=\"etc/jquery.js\"></script>");
		sent["etc/jquery.js"] = true;
	}
	
	if ( !sent[file] ) 
	{
		add_html_head("<script type=\"text/javascript\" src=\""+ file + postfix +"\"></script>");
		sent[file] = true;
	}
}

void set_page_message(string message, string type)
{
	if(type == "error") _ERROR_MESSAGES.push_back( message );
	if(type == "status") _STATUS_MESSAGES.push_back( message );
}

string file_directory_path()
{
  return variable_get("file_directory_path", "files");
}

string file_directory_temp()
{
  return variable_get("file_directory_temp", TEMP_PATH ? string(TEMP_PATH) : string("") );
}

bool is_dir( string path )
{
	struct stat buf;
	int result;

	if( path.length() == 2 && isalpha( path[0] ) && path[1] == ':' ) path += "/";
	
	result = stat( path.c_str(), &buf );
	if( result < 0 ) return false;

	return ( buf.st_mode & S_IFDIR );
}

int filecopy ( string filename, string dest )
{
	if( filename.size() == 0 ) return 0;
	FILE * fp = fopen(filename.c_str(), "rb");
	if(!fp) return 0;
	fseek( fp, 0, SEEK_END );
	int size = ftell( fp );
	fseek( fp, 0, SEEK_SET );
	unsigned char * data = (unsigned char *) malloc( size + 1 );
	fread(data, 1, size, fp);
	FILE * fpout = fopen(dest.c_str(), "wb");
	fwrite(data, 1, size, fpout);
	free( data );
	data = NULL;
	fclose(fp);
	fclose(fpout);
	return size;
}

int filesize ( string filename )
{
	if( filename.size() == 0 ) return 0;
	FILE * fp = fopen(filename.c_str(), "rb");
	if(!fp) return 0;
	fseek( fp, 0, SEEK_END );
	int size = ftell( fp );
	fclose(fp);
	return size;
}

bool file_exists( string filename )
{
	FILE * fp = fopen(filename.c_str(), "rb");
	if(!fp) return false;
	else {
		fclose(fp);
		return true;
	}
}

string file_get_contents( string filename ) // Read Text Files
{
	string out;
	FILE * fp = fopen(filename.c_str(), "rb");

	fseek( fp, 0, SEEK_END );
	size_t size = ftell( fp );
	fseek( fp, 0, SEEK_SET );
	unsigned char * data = (unsigned char *) malloc( size + 1 );
	fread( data, 1, size, fp );
	fclose(fp);

	if( data[0] == 0xFF && data[1] == 0xFE ) //Unicode BOM
	{	
		for( size_t i=2; i< size; i+=2 )
		{
			out += data[i];
		}
	}
	else
	{
		data[size] = (unsigned char) NULL;
		out = (char *) data;
	}
	free( data );

	return out;
}

string sizefile( string id )
{
	string s = file_directory_temp() + "/" + num( id ) + ".size";
	_TEMP_FILES.push_back( s );	
	return s;
}

string tempfile( const char * tempdir, const char * prefix )
{
	string s;

	char path[256];
	
	do {
		_TEMP_FILES_COUNT ++;
		sprintf(path, "%s%i", prefix, _TEMP_FILES_COUNT);
		s = ( tempdir ? string(tempdir) : file_directory_temp() ) + "/" + string(path);
	} while( file_exists( s ) );

	_TEMP_FILES.push_back( s );
	
	return s;
}

int removetemp()
{
	unsigned int i;
	for( i = 0; i < _TEMP_FILES.size(); i++ )
	{
		unlink( _TEMP_FILES[i].c_str() );
	}

	return i;
}

static int HexPairValue(const char * code) 
{
  int value = 0;
  const char * pch = code;
  for (;;) {
    int digit = *pch++;
    if (digit >= '0' && digit <= '9') {
      value += digit - '0';
    }
    else if (digit >= 'A' && digit <= 'F') {
      value += digit - 'A' + 10;
    }
    else if (digit >= 'a' && digit <= 'f') {
      value += digit - 'a' + 10;
    }
    else {
      return -1;
    }
    if (pch == code + 2)
      return value;
    value <<= 4;
  }
}

int UrlDecode(const char *source, char *dest)
{
  char * start = dest;

  while (*source) {
    switch (*source) {
    case '+':
      *(dest++) = ' ';
      break;
    case '%':
      if (source[1] && source[2]) {
        int value = HexPairValue(source + 1);
		if (value >= 0) {
          *(dest++) = value;
          source += 2;
        }	
        else {
          *dest++ = '?';
        }
      }
      else {
        *dest++ = '?';
      }
      break;
    default:
      *dest++ = *source;
    }
    source++;
  }
  
  *dest = 0;
  return (int) (dest - start);
}  

int UrlEncode(const char *source, char *dest, unsigned max)  
{
  static const char *digits = "0123456789ABCDEF";
  unsigned char ch;
  unsigned len = 0;
  char *start = dest;

  while (len < max - 4 && *source)
  {
    ch = (unsigned char)*source;
    if (*source == ' ') {
      *dest++ = '+';
    }
    else if (isalnum(ch) || strchr("-_.!~*'()", ch)) {
      *dest++ = *source;
    }
    else {
      *dest++ = '%';
      *dest++ = digits[(ch >> 4) & 0x0F];
      *dest++ = digits[       ch & 0x0F];
    }  
    source++;
  }
  *dest = 0;
  return (int) (start - dest);
}

/**
 * The urldecode() function decodes any entity in the Hex form %## in the given character representation and decodes the plus symbol + to a white space character.
 * @param encoded String to be decoded
 * @returns	The decoded string
 * @par Example:
 * @code
 * urldecode("%22hello+world!%22");
 * @endcode
 * @ingroup encode
 */
string urldecode(string encoded) 
{
	const char * sz_encoded = encoded.c_str();
	size_t needed_length = encoded.length();
	for (const char * pch = sz_encoded; *pch; pch++) 
	{
		if (*pch == '%')
			needed_length += 2;
	}
	needed_length += 10;
	char stackalloc[64];
	char * buf = needed_length > sizeof(stackalloc)/sizeof(*stackalloc) ? (char *)malloc(needed_length) : stackalloc;
	UrlDecode(encoded.c_str(), buf);
	std::string result(buf);
	if (buf != stackalloc)
		free(buf);
	return result;
}

/**
 * The urlencode() function encodes a string in a URL-encoded format where all non alphanumeric characters except - _ . ! ~ * ' ( ) are replaced with the hexadecimal sequences of the form %##
 * @param decoded String to be encoded
 * @returns	The encoded string
 * @par Example:
 * @code
 * urlencode("\"hello world!\"");
 * @endcode
 * @ingroup encode
 */
string urlencode(string decoded) 
{
  size_t needed_length = decoded.length() * 3 + 3;
  char stackalloc[64];
  char * buf = needed_length > sizeof(stackalloc)/sizeof(*stackalloc) ?
    (char *)malloc(needed_length) : stackalloc;
  UrlEncode(decoded.c_str(), buf, (int) needed_length);
  std::string result(buf);
  if (buf != stackalloc) {
    free(buf);
  }
  return result;
}

int dir_index = 0;

#ifdef _MSC_VER
	char * dir_filename = NULL;
	HANDLE	dir_dp[100];
#endif
#ifndef _MSC_VER
	DIR * dir_dp[100];
#endif

void dirclose( int handle )
{
	dir_index --;
}

int diropen( string dir )
{
#ifdef _MSC_VER
	dir += "*";
	dir_filename = (char *) malloc( dir.length() + 1 );
	strcpy( dir_filename, dir.c_str() );
	dir_filename[ dir.length() ] = NULL;
	dir_index ++;
	return dir_index;
#endif
#ifndef _MSC_VER
	dir += ".";
	dir_index ++;
	dir_dp[dir_index] = opendir(dir.c_str());
	if(dir_dp[dir_index] == NULL) 
		return 0;
	return dir_index;
#endif
}

int dirread( int handle, string &filename )
{
	if( handle == 0 ) return 0;

#ifdef _MSC_VER
	WIN32_FIND_DATAA FindFileData;
	if( dir_filename != NULL )
	{
		dir_dp[handle] = FindFirstFileA( dir_filename, &FindFileData );
		free( dir_filename );
		dir_filename = NULL;
		if( dir_dp[handle] ==  INVALID_HANDLE_VALUE) return 0;
	}
	else
	{
		if( !FindNextFileA( dir_dp[handle], &FindFileData ) )
		{
			FindClose( dir_dp[handle] );
			handle = 0;
		}
	}
	filename = string( FindFileData.cFileName );
	
	return handle;
#endif
#ifndef _MSC_VER
	struct dirent *dirp;
	dirp = readdir( dir_dp[handle] );
	if( !dirp ) 
	{
		closedir( dir_dp[handle] );
		handle = 0;
	}
	else
	{
		filename = string( dirp->d_name );
	}
	return handle;
#endif
}

string getcwd()
{
	char* buffer;
	if( (buffer = getcwd( NULL, 0 )) != NULL )
	{
		return string( buffer );		
	}
	return "";
}

int RawUrlEncode(const char *source, char *dest, unsigned max)  
{
	static const char *digits = "0123456789ABCDEF";
	unsigned char ch;
	unsigned len = 0;
	char *start = dest;

	while (len < max - 4 && *source)
	{
		ch = (unsigned char)*source;
		if (isalnum(ch) || strchr("-_.", ch)) {
			*dest++ = *source;
		}
		else {
			*dest++ = '%';
			*dest++ = digits[(ch >> 4) & 0x0F];
			*dest++ = digits[       ch & 0x0F];
		}  
		source++;
	}
	*dest = 0;
	return (int) (start - dest);
}

/**
 * The rawurlencode() function  encodes a string in a URL-encoded format where all non alphanumeric characters except - _ . are replaced with the hexadecimal sequences of the form %##
 * @param decoded String to be encoded
 * @returns	The encoded string
 * @par Example:
 * @code
 * rawurlencode("\"hello world!\"");
 * @endcode
 * @ingroup encode
 */
string rawurlencode(string decoded) 
{
	size_t needed_length = decoded.length() * 3 + 3;
	char stackalloc[64];
	char * buf = needed_length > sizeof(stackalloc)/sizeof(*stackalloc) ? (char *)malloc(needed_length) : stackalloc;
	RawUrlEncode(decoded.c_str(), buf, (int) needed_length);
	std::string result(buf);
	if (buf != stackalloc) {
		free(buf);
	}
	return result;
}

/** 
 * The match_right() function performs a search for an exact substring pointed by needle at the end of the main string pointed by haystack.
 * @param haystack String to search in
 * @param needle String to look for
 * @returns A boolean false if needle is not found
 * @par Example:
 * @code
 * if( match_right(filename, ".gif") )
 * {
 *    print( "Found image: "+filename );
 * }
 * @endcode @ingroup group14
 */
bool match_right(string haystack, string needle)
{
	int j=(int) strlen(haystack.c_str())-1;
	int i=(int) strlen(needle.c_str())-1;
	if( i > j ) return false;
	for (; i>=0; i--,j--)
	{
		if (haystack[j] != needle[i]) return false;
	}
	return true;
}

/**
 * The rtrim() function remove whitespace and non visible characters (or others is specified by the parameter @c chr) from the end of a string.
 * @param str The input string
 * @param chr The optional list of characters to be removed from the end of the input string.
 * @returns Returns the trimmed string
 * @par Example:
 * @code
 * string title = "Bill's Blog  \n ";
 * title = trim( title );
 * print( title ); // Outputs Bill's Blog
 * @endcode @ingroup group14
 */
void rtrim(string& str, const char *chr)
{
	string::size_type pos = str.find_last_not_of(chr);
	if(pos != string::npos) {
		str.erase(pos + 1);
	}
}

/**
 * This is an overloaded member function, provided for convenience. It differs from the above function only in what argument(s) it accepts.
 * @ingroup group14
 */
void rtrim(string& str, char chr)
{
	string::size_type pos = str.find_last_not_of(chr);
	if(pos != string::npos) {
		str.erase(pos + 1);
	}
}

/**
 * The ltrim() function remove whitespace and non visible characters (or others is specified by the parameter @c chr) from the beginning of a string.
 * @param str The input string
 * @param chr The optional list of characters to be removed from the beginning of the input string.
 * @returns Returns the trimmed string
 * @par Example:
 * @code
 * string title = "  \n Bill's Blog";
 * title = trim( title );
 * print( title ); // Outputs Bill's Blog
 * @endcode @ingroup group14
 */
void ltrim(string& str, const char *chr)
{
	string::size_type pos = str.find_first_not_of(chr);
	if(pos != string::npos) {
		str = str.substr( pos );
	}
	else str.erase(str.begin(), str.end());
}

/**
 * This is an overloaded member function, provided for convenience. It differs from the above function only in what argument(s) it accepts.
 * @ingroup group14
 */
void ltrim(string& str, char chr)
{
	string::size_type pos = str.find_first_not_of(chr);
	if(pos != string::npos) {
		str = str.substr( pos );
	}
	else str.erase(str.begin(), str.end());
}

/**
 * The trim() function remove whitespace and non visible characters (or others is specified by the parameter @c chr) from the beginning and end of a string.
 * @param str The input string
 * @param chr The optional list of characters to be removed from the beginning and end of the input string.
 * @returns Returns the trimmed string
 * @par Example:
 * @code
 * string title = "  \n Bill's Blog  \n ";
 * title = trim( title );
 * print( title ); // Outputs Bill's Blog
 * @endcode @ingroup group14
 */
void trim(string& str, const char *chr)
{
  string::size_type pos = str.find_last_not_of(chr);
  if(pos != string::npos) {
    str.erase(pos + 1);
    pos = str.find_first_not_of(chr);
    if(pos != string::npos) str.erase(0, pos);
  }
  else str.erase(str.begin(), str.end());
}

/**
 * This is an overloaded member function, provided for convenience. It differs from the above function only in what argument(s) it accepts.
 * @ingroup group14
 */
void trim(string& str, char chr)
{
  string::size_type pos = str.find_last_not_of(chr);
  if(pos != string::npos) {
    str.erase(pos + 1);
    pos = str.find_first_not_of(chr);
    if(pos != string::npos) str.erase(0, pos);
  }
  else str.erase(str.begin(), str.end());
}

/**
 * This is an overloaded member function, provided for convenience. It differs from the above function only in what argument(s) it accepts.
 * @ingroup group14
 */
void trim(vector <string> &pieces, const char *chr )
{
	for(size_t i=0; i<pieces.size(); i++)
	{
		trim( pieces[i], chr );
	}
}

/**
 * This is an overloaded member function, provided for convenience. It differs from the above function only in what argument(s) it accepts.
 * @ingroup group14
 */
void trim( map <string, string> &item, const char *chr )
{
	map <string, string>::iterator curr, end;

	for( curr = item.begin(), end = item.end();  curr != end;  curr++ )
	{
		trim( item[curr->first], chr );
	}
}

/**
 * The sha1() function calculate the sha1 hash of a string
 * @param data The input message string
 * @returns The sha1 hash as a string.
 * @note It is not recommended to use this function to secure passwords, due to the fast nature of this hashing algorithm.
 * @par Example:
 * @code
 * if( "d0be2dc421be4fcd0172e5afceea3970e2f3d940" == sha1( "apple" ) )
 *     print( "You entered: apple" );
 * @endcode
 * @ingroup encode
 */
string sha1( string data )
{
	string out;
	SHA1Context sha;
    int i;
	char str[10];

    SHA1Reset(&sha);
	SHA1Input(&sha, (const unsigned char *) data.c_str(), (int) data.length() );

    if ( SHA1Result(&sha) )
    {
        for(i = 0; i < 5 ; i++)
        {
            sprintf(str, "%08x", sha.Message_Digest[i]);
			out += string(str);
        }
    }

	return out;
}

/**
 * The md5() function calculate the MD5 message digest of a string using the "RSA Data Security, Inc. MD5 Message-Digest Algorithm" see internet RFC 1321.
 * @param data The input message string
 * @returns The 128-bit message digest as a hexadecimal 32-character string.
 * @par Example:
 * @code
 * string stored_pass = db_result( db_query("SELECT pass FROM admin") );
 * if( stored_pass != md5( _POST["pass"] ) )
 *     print( "Access Denied" );
 * @endcode
 * @ingroup encode
 */
string md5( string data )
{
	unsigned char  m_digest[16]; //the numerical value of the digest
	char  m_digestString[33];  //Null terminated string value of the digest expressed in hex digits
	char* m_plainText = (char *) data.c_str();
												 //you could make a local copy of the plain text string.
	m_digestString[32] = (char) NULL;

	MD5_CTX context;
	MD5Init(&context); 

	//the alternative to these ugly casts is to go into the RFC code and change the declarations
	MD5Update(&context, reinterpret_cast<unsigned char *>(m_plainText), (unsigned int) ::strlen(m_plainText));
	MD5Final(reinterpret_cast <unsigned char *>(m_digest),&context);

	//make a string version of the numeric digest value
	int p=0;
	for (int i = 0; i<16; i++)
	{
		sprintf(&m_digestString[p],"%02x", m_digest[i]);
		p+=2;
	}
	return string( m_digestString );
}

u_long crc32_table[256];

#define CRC32_POLY 0x04c11db7    /* AUTODIN II, Ethernet, & FDDI */

void init_crc32(int endian = 0)  // 1: BigEndian  0: LittleEndian
{
	int i, j;
	u_long c;

	for (i = 0; i < 256; ++i) 
	{
		for (c = endian ? (i << 24) : i, j = 8; j > 0; --j)
		{
			if (endian)
				c = c & 0x80000000 ? (c << 1) ^ CRC32_POLY : (c << 1);
			else
				c = c & 1 ? (c >> 1) ^ CRC32_POLY : (c >> 1);
		}
		crc32_table[i] = c;
	}
}

/**
 * The crc32() function calculates the CRC-32 (Cyclic Redundancy Check) of a string.
 * @param data The input string
 * @returns The CRC-32 checksum as an integer.
 * @par Example:
 * @code
 * size_t checksum = crc32("The quick brown fox jumped over the lazy dog.");
 * print( str( checksum ) );
 * @endcode
 * @ingroup encode
 */
size_t crc32( string data )
{
	return crc32( (unsigned char *) data.c_str(), data.length() );
}

/**
 * This is an overloaded member function, provided for convenience. It differs from the above function only in what argument(s) it accepts.
 * @param data The input data pointer
 * @param len The size of the input data buffer
 * @ingroup encode
 */
size_t crc32(unsigned char *data, size_t len)
{
    u_char *p;
    size_t  crc;

    if (!crc32_table[1])    /* if not already done, */
            init_crc32();   /* build table */
    crc = 0xffffffff;       /* preload shift register, per CRC-32 spec */
    for (p = data; len > 0; ++p, --len)
            crc = (crc << 8) ^ crc32_table[(crc >> 24) ^ *p];
    return ~crc;            /* transmit complement, per CRC-32 spec */
}

/**
 * The qpencode() function encodes a string in Quoted-Printable Content-Transfer-Encoding as described in section 5.1 of the RFC 1521.
 * @param str The input string
 * @returns The encoded string.
 * @par Example:
 * @code
 * print( qpencode("<img src=\"ball.gif\" width=100 height=100 />") );
 * @endcode
 * @par The above example will output:
 * @code
 * <img src=3D"ball.gif" width=3D100 height=3D100 />
 * @endcode
 * @ingroup encode
 */
string qpencode(string str)
{
	string	out;
	int count = 0;

	for (int i=0;i<(int) strlen(str.c_str());i++)
	{
		int car = str[i];
		if( car < 0 )
		{
			car += 256;
		}
		if (  car >=0 && car <=255 )
		{
			if ( (car>=33 && car<=60) || (car>=62 && car<=126) )
			{
				out += car;
				count ++;
			}
			else if ( car==32  )
			{
				out += car;
				count ++;
			}
			else if ( car==0x0D || car==0x0A )
			{
				out += car;
				count = 0;
			}
			else
			{
				if( count >= 73 )
				{
					count = 0;
					out += "=\r\n";
					i --;
				}
				else
				{
					char hex[10];
					sprintf(hex, "=%02X", car);
					out += hex;
					count += 3;
				}
			}

			if( count == 75 )
			{
				count = 0;
				out += "=\r\n";
			}
		}
	}
  return out;
}

/**
 * The time() function obtains the system time.
 * @returns The time as seconds elapsed since midnight, January 1, 1970.
 * @par Example:
 * @code
 * int six_hours_ago;
 * six_hours_ago = time()-(6*60*60); 
 * @endcode @ingroup time
 */
int time()
{
	time_t seconds;
	return ( (int) time( &seconds ) );
}

/**
 * The microtime() function obtains the system time in milliseconds
 * @returns The time as milliseconds elapsed since midnight, January 1, 1970.
 * @par Example:
 * @code
 * string sid = md5 ( microtime() );
 * @endcode @ingroup time
 */
string microtime()
{
	char mili[8];
	struct timeb timebuffer;
	ftime( &timebuffer );

	sprintf( mili, "%04i", timebuffer.millitm );

	return str( (size_t) timebuffer.time ) + string(mili);
}

string format_plural(int count, string singular, string plural)
{
	char data[64];

	if(count == 1)
	{
		sprintf( data, singular.c_str(), count );
	}
	else
	{
		sprintf( data, plural.c_str(), count );
	}

	return string(data);
}

string format_interval(int timestamp, int granularity)
{
	map <string, int> units;

	units["0|1 year|%i years"] = 31536000;
	units["1|1 week|%i weeks"] = 604800;
	units["2|1 day|%i days"] = 86400;
	units["3|1 hour|%i hours"] = 3600;
	units["4|1 min|%i min"] = 60;
	units["5|1 sec|%i sec"] = 1;
  
	string output = "";
	
	map <string, int>::iterator curr, end;

	for( curr = units.begin(), end = units.end();  curr != end;  curr++ )
	{
		vector <string> key;
		explode(key, curr->first, '|');
		int value = curr->second;

		if (timestamp >= value) {
			output += (isset(output) ? " " : "") + format_plural( (int) floor( (double) timestamp / (double) value), t(key[1]), t(key[2]));
			timestamp = (int) fmod( (double) timestamp, (double) value );
			granularity--;
		}

		if (granularity == 0) {
			break;
		}
	}
	return isset(output) ? output : "0 sec";
}

string format_date( string timestamp )
{
	return date("%A, %m/%d/%Y - %H:%M", timestamp);
}

string format_size( string str_size )
{
	int size = intval(str_size);
	string	suffix;
	
	if ( size < 1024) 
	{
		return format_plural(size, "%i byte", "%i bytes");
	}
	else 
	{
		char num[15];
		sprintf(num, "%.2d", size / 1024);
		size = atoi( num );
		suffix = "KB";
		if (size >= 1024) 
		{
			sprintf(num, "%.2d", size / 1024);
			size = atoi( num );
			suffix = "MB";
		}
		return str(size)+" "+suffix;	
	}
}

string date( string format, string timestamp )
{
	time_t rawtime;
	struct tm * timeinfo;

	char buffer [80];
	
	if( timestamp.empty() )
	{
		time (&rawtime);
	}
	else
	{
		rawtime = atoi( timestamp.c_str() );
	}

	timeinfo = localtime ( &rawtime );

	strftime (buffer, 80, format.c_str(), timeinfo);

	return string( buffer );
}

string gmdate( string format, string timestamp, int timezone )
{
	time_t rawtime;
	struct tm * timeinfo;

	char buffer [80];
	
	if( timestamp.empty() )
	{
		time (&rawtime);
	}
	else
	{
		rawtime = atoi( timestamp.c_str() );
	}

	rawtime += 60*60*timezone;

	timeinfo = gmtime ( &rawtime );

	strftime (buffer, 80, format.c_str(), timeinfo);

	return string( buffer );
}

void strip(char * s, char c)
{
    char * p1 = s;

    while (*p1++)
    {
        if(*p1 == c)
        {
            char * p2 = p1;
            while(*p2 && *p2 == c) { ++ p2; }

            if(*p2)
            {
                *p1 = *p2;
                *p2 = c;
            }
            else
            {
                *p1 = '\0';
                break;
            }
        }
    }
}

bool is_numeric( string str )
{
	int digit = 0;
	for (int i=0; i<(int) str.length(); i++) {
		if( isdigit( str[i] ) || str[i]=='.' || str[i]=='-' ) digit ++;
	}
	if(str.length()==0) return false;
	return (str.length()==digit);
}

char toupperlatin( char c )
{
	char *upper = (char *) "ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖØÙÚÛÜÝ";
	char *lower = (char *) "àáâãäåæçèéêëìíîïðñòóôõöøùúûüý";

	int i = 0;
	while( lower[i] )
	{
		if( lower[i] == c ) return upper[i];
		i++;
	}
	return c;
}

char tolowerlatin( char c )
{
	char *upper = (char *) "ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖØÙÚÛÜÝ";
	char *lower = (char *) "àáâãäåæçèéêëìíîïðñòóôõöøùúûüý";

	int i = 0;
	while( upper[i] )
	{
		if( upper[i] == c ) return lower[i];
		i++;
	}
	return c;
}

string strtolower(string s) 
{
  for (int i=0;i<(int) strlen(s.c_str());i++)
	s[i] = tolowerlatin( tolower( s[i] ) );
  return s;
}

string strtoupper(string s)
{
  for (int i=0;i<(int) strlen(s.c_str());i++)
	s[i] = toupperlatin( toupper( s[i] ) );
  return s;
}

char *strtoupper(char *s1)
{
   char *p = s1;
   while(*p) {
       *p = toupperlatin( toupper(*p) );
       p++;
   }
   return s1;
}

char *strtolower(char *s1)
{
   char *p = s1;
   while(*p) {
       *p = tolowerlatin( tolower(*p) );
       p++;
   }
   return s1;
}

string capwords(string s)
{
	int up = 1;
	for (int i=0;i<(int) strlen(s.c_str());i++)
	{
		if( up ) s[i] = toupperlatin( toupper( s[i] ) );
		else s[i] = tolowerlatin( tolower( s[i] ) );

		if( s[i] == 0x20 ) up=1;
		else up=0;
	}
	return s;
}

char *capwords(char *s1)
{
	int up = 1;
	char *p = s1;
	while(*p) 
	{       
		if( up ) *p = toupperlatin(toupper(*p));
		else *p = tolowerlatin(tolower(*p));

		if( *p == 0x20 ) up=1;
		else up=0;

		p++;
	}
	return s1;
}

string strtotime( string date )
{
	time_t parsed;
	int rc = bt_parsedate(date.c_str(), &parsed);

	switch(rc) 
	{
		case PARSEDATE_OK:
		case PARSEDATE_LATER:
		case PARSEDATE_SOONER:
			return str( (int) parsed );
	}

	struct tm  when;
		
	int year = 0, mon = 0, day = 0, hour = 0, min = 0, sec = 0;
	
	// Parse ISO 8601
	if( sscanf( date.c_str(), "%4d-%02d-%02d", &year, &mon, &day ) == 3 )
	{
		when.tm_isdst = 0;
		when.tm_yday = 0;
		when.tm_wday = 0;
		when.tm_year = year - 1900;
		when.tm_mon  = mon - 1;
		when.tm_mday = day;
		when.tm_hour = 0;
		when.tm_min = 0;
		when.tm_sec = 0;

		if( date.length() > 10 )
		{
			date = date.substr(10);
			size_t ini_hour = date.find_first_of("0123456789");

			if( ini_hour != string::npos )
			{
				if( sscanf( date.substr( ini_hour ).c_str(), "%02d:%02d:%02d", &hour, &min, &sec ) )
				{
					when.tm_hour = hour;
					when.tm_min = min;
					when.tm_sec = sec;
				}
			}
		}

		parsed = mktime( &when );
		if( parsed > 0)
			return str( (int) parsed );
	}
	// Parse European Time
	if( sscanf( date.c_str(), "%02d/%02d/%04d", &day, &mon, &year ) == 3 )
	{
		when.tm_isdst = -1;
		when.tm_yday = 0;
		when.tm_wday = 0;
		when.tm_year = year - 1900;
		when.tm_mon  = mon - 1;
		when.tm_mday = day;
		when.tm_hour = 0;
		when.tm_min = 0;
		when.tm_sec = 0;

		if( date.length() > 10 )
		{
			date = date.substr(10);
			size_t ini_hour = date.find_first_of("0123456789");

			if( ini_hour != string::npos )
			{
				if( sscanf( date.substr( ini_hour ).c_str(), "%02d:%02d:%02d", &hour, &min, &sec ) )
				{
					when.tm_hour = hour;
					when.tm_min = min;
					when.tm_sec = sec;
				}
			}
		}

		parsed = mktime( &when );
		if( parsed > 0)
			return str( (int) parsed );
	}

	return str( time() );
}

/** 
 * The addslashes() function escapes a string by adding backslashes before the characters single quote ('), double quote ("), backslash (\) and zero (the 0 byte).\n
 * This function is normally used in the comunication between BinaryTiers and other libraries, services, protocols, etc.
 * @param str String to be escaped
 * @returns The escaped string
 * @par Example:
 * @code
 * string title = "Bill's blog";
 * title = addslashes( title );
 * print( title ); // Outputs Bill\'s blog
 * @endcode @ingroup group14
 */
string addslashes(string str)
{
	string	out;
	for (int i=0;i<(int) strlen(str.c_str());i++)
	{
		if(str[i]==0 || str[i]==34 || str[i]==39 || str[i]=='\\' ) out += '\\';
		out += str[i];
	}
	return out;
}

string jsonencode(string out) 
{
	//out = addslashes( out );
	out = str_replace("\"", "\\\"", out );	
	out = str_replace("/", "\\/", out );
	out = str_replace("\t", "\\t", out );
	out = str_replace("\f", "\\f", out );
	out = str_replace("\b", "\\b", out );
	out = str_replace("\r", "\\r", out );
	out = str_replace("\n", "\\n", out );
	out = str_replace("&", "&amp;", out );
	return out;
}

/** 
 * The str_repeat() repeats a string.
 * @param s The string to be repeated
 * @param multiplier The number of times the string should be repeated. 
 * @returns The repeated string.
 * @ingroup group14
 */
string str_repeat( string s, int multiplier )
{
	string out;
	for( int i = 0; i<multiplier ; i++ )
		out += s;
	
	return out;
}

/** 
 * The str() function converts a number to a string. The numbers can be integal type (int, size_t) or a floating type (float, double).
 * @param n The number to be converted
 * @returns A string.
 * @ingroup group13
 */
string str( size_t n )
{
	char s[15];
	sprintf(s, "%i", (int) n);
	return string( s );
}

/**
 * This is an overloaded member function, provided for convenience. It differs from the above function only in what argument(s) it accepts.
 * @ingroup group13
 */
string str( int n )
{
	char s[15];
	sprintf(s, "%i", n);
	return string( s );
}

/**
 * This is an overloaded member function, provided for convenience. It differs from the above function only in what argument(s) it accepts.
 * @ingroup group13
 */
string str( long long n )
{
	char s[15];
	sprintf(s, "%i", n);
	return string( s );
}

/**
 * This is an overloaded member function, provided for convenience. It differs from the above function only in what argument(s) it accepts.
 * @ingroup group13
 */
string str( double n )
{
	char s[15];
	sprintf(s, "%.2f", n);
	return string( s );
}

/** 
 * The num() function ensures that a string is formatted as a number.
 * @param s The string to be converted
 * @returns A string formatted as a number
 * @ingroup group13
 */
string num( string s )
{
	string out;
	for (int i=0;i<(int) strlen(s.c_str());i++)
	{
		if( isdigit(s[i]) || s[i]=='.' || s[i]=='-') out += s[i];
	}
	return out;
}

/** 
 * The print() function outputs a string to the HTTP buffer
 * @param s The string to be sent
 * @returns None.
 * @ingroup group13
 */
void print( string s )
{
	main_output += s;
}

/**
 * The inarray() function search for a value in an array or for a key in an associative array 
 * @param item Array or Associative array to perform the search
 * @param find is the value to search
 * @returns @c true if the value or the key has been found, otherwise returns @c false.
 * @par Example:
 * @code
 * inarray( _POST, "page" );
 * @endcode
 * @see explode() implode()
 * @ingroup group1
 */
bool inarray( vector <string> &item, string find )
{
	for( size_t i = 0;  i < item.size();  i++ )
	{
		if( item[i] == find ) return true;
	}

	return false;
}

/**
 * This is an overloaded member function, provided for convenience. It differs from the above function only in what argument(s) it accepts.
 * @ingroup group1
 */
bool inarray( map <int, string> &item, int find )
{
	for( map <int, string>::iterator curr = item.begin(), end = item.end();  curr != end;  curr++ )
	{
		if( curr->first == find ) return true;
	}

	return false;
}

/**
 * This is an overloaded member function, provided for convenience. It differs from the above function only in what argument(s) it accepts.
 * @ingroup group1
 */
bool inarray( map <string, string> &item, string find )
{
	map <string, string>::iterator curr, end;

	for( curr = item.begin(), end = item.end();  curr != end;  curr++ )
	{
		if( curr->first == find ) return true;
	}

	return false;
}

/**
 * The explain() function makes a variable comprehensible by displaying information of the variable in a way programmers can understand.
 * @param items Vector of string to display
 * @ingroup group13
 */
void explain( vector <string> &items )
{
	for( size_t i = 0; i < items.size(); i++ )
	{	
		print ( "["+str(i)+"]={" );
		print ( "\""+items[i]+"\"" );
		print ("}\n");
	}
}

/**
 * The explain() function makes a variable comprehensible by displaying information of the variable in a way programmers can understand.
 * @param items Associative array of string to display
 * @ingroup group13
 */
void explain( map <string, string> &items )
{
	map <string, string>::iterator curr, end;

	for( curr = items.begin(), end = items.end();  curr != end;  curr++ )
	{
		print ("\t[\""+curr->first+"\"]="+"\""+curr->second+"\";\n" );
	}
}

/**
 * The explain() function makes a variable comprehensible by displaying information of the variable in a way programmers can understand.
 * @param items Vector of associative array of string to display
 * @ingroup group13
 */
void explain( vector <map <string, string> > &items )
{	
	for( size_t i = 0; i < items.size(); i++ )
	{	
		print ( "["+str(i)+"]={\n" );
		explain( items[i] );
		print ("}\n");
	}
}

/**
 * The explain() function makes a variable comprehensible by displaying information of the variable in a way programmers can understand.
 * @param items Associative array of an associative array of string to display
 * @ingroup group13
 */
void explain( map <string, map<string,string> > &items )
{
	map <string, map<string,string> >::iterator curr, end;
	
	for( curr = items.begin(), end = items.end();  curr != end;  curr++ )
	{	
		print ( "[\""+curr->first+"\"]={\n" );
		explain( items[curr->first] );
		print ("}\n");
	}
}

/**
 * The explain() function makes a variable comprehensible by displaying information of the variable in a way programmers can understand.
 * @param items Vector of pairs of string to display
 * @ingroup group13
 */
void explain( vector <pair <string, string> > items )
{
	for( size_t i = 0; i< items.size(); i++ )
		print( "[\"" + items[i].first + "\"]=\"" + items[i].second + "\";\n" );
		
}

int jsonskip( string s )
{
	int count = 0, in = 0;
	for (int i=0;i<(int) strlen(s.c_str());i++)
	{
		if( s[i] == '{' ) count++;
		if( s[i] == '}' ) count--;
		if( s[i] == '\"' ) in++;
	}
	return count + (in%2);
}

string jsonunescape(string encoded)
{
	const char * sz_encoded = encoded.c_str();
	string dest;

	for (const char * pch = sz_encoded; *pch; pch++)
	{
		if( *pch == '\\' )
		{
			pch++;
			if( *pch == '/' ) dest += "/";
			if( *pch == 'u' ) 
			{
				dest += HexPairValue( pch+1 )*255 + HexPairValue( pch+3 );
				pch+=4;
			}

		}
		else
		{
			dest += *pch;
		}
	}
	return dest;
}

void jsondecode(map <string, string> &pieces, string the_string)
{
	size_t first, second;

	ltrim( the_string, "\x09\x0a\x0d\x0b{ " );
	trim( the_string, "\x09\x0a\x0d\x0b} " );

	if( the_string.length()>0 )
	{
		first = 0;
		do {
			second = the_string.find_first_of(',', first);
			while( jsonskip(the_string.substr(first, second-first)) > 0 && second != string::npos )
			{
				second = the_string.find_first_of(',', second+1);
			}

			size_t tag = the_string.find_first_of(':', first);
			string key = the_string.substr( first, tag-first );
			string value = the_string.substr( tag+1, second-tag-1 );

			ltrim( key );
			trim( key );

			ltrim( value );
			trim( value );

			ltrim( key, "\"" );
			trim( key, "\"" );

			ltrim( value, "\"" );
			trim( value, "\"" );

			pieces[key] = jsonunescape(value);
			first = second+1;
		} while( second != string::npos );
	}
}

string menu_item_link(int mid)
{
	string attributes;
	if(mid == _ACTIVE_CONTROLLER) attributes = " class=\"active\"";
	if(_ACTIVE_CONTROLLER >= 0 && _ACTIVE_CONTROLLER <= (int) controllers.size()) {
		if( isset2( controllers[_ACTIVE_CONTROLLER], "visible_tree" ) && mid == intval( controllers[_ACTIVE_CONTROLLER]["visible_tree"] ) ) attributes = " class=\"active\"";
	}

	return "<a href=\""+ url( controllers[mid]["path"] )+"\""+attributes+">"+controllers[mid]["title"]+"</a>";
}

string theme_menu_item(int mid, string children = "", bool leaf = true) {
	string ss = !children.empty() ? "expanded" : "collapsed";
	return "<li class=\"" + (leaf ? "leaf":ss) + "\">" + menu_item_link(mid) + children + "</li>\n";
}

string menu_tree( int pid = -1 )
{
	string spid = str( pid );
	string output = "";
	unsigned int mid;
	for(mid=0; mid< controllers.size(); mid++)
	{
		if(controllers[mid]["parent"] == spid)
		{
			int type = atoi( controllers[mid]["type"].c_str() );
			if( type != MENU_LOCAL_TASK)
			{	
				bool has_children = !(controllers[mid]["children"].empty());

				if(has_children)
				{
					vector <string> children;
					explode(children, controllers[mid]["children"]);
					bool found = false;
					bool visible_parents = false;
					for(size_t j=0; j<children.size(); j++)
					{
						int child = intval(children[j]);

						if( !(intval(controllers[child]["type"]) && MENU_VISIBLE_IN_TREE) ) visible_parents = true;
						if( child == _ACTIVE_CONTROLLER ) found = true;
					}

					if( !found && _ACTIVE_CONTROLLER != mid ) has_children = false;
					if( !visible_parents && _ACTIVE_CONTROLLER == mid ) has_children = false;
				}

				if( !(type && MENU_VISIBLE_IN_TREE) ) // We display in the tree only if the menu item is visible.
				{
					output += theme_menu_item( mid, has_children ? theme_menu_tree( mid ) : "", !has_children );
				}
			}
		}
	}
	
	return output;
}

void build_menu()
{
	int i, j;
	size_t cut;

	for( i = 0; i < (int) controllers.size(); i++ )
	{
		string parent = "";
		cut = controllers[i]["path"].find_last_of('/');
		if( cut != controllers[i]["path"].npos ) parent = controllers[i]["path"].substr(0, cut); // Get legit parent
		
		if( !controllers[i]["parent"].empty() ) parent = controllers[i]["parent"]; // Get parent custom parent
			
		if( !controllers[i]["parent tab"].empty() ) {
			string parent_tab = controllers[i]["parent tab"];
			
			if( (intval( controllers[i]["type"] ) && MENU_IS_LOCAL_TASK) )
			{	
				for( j = 0; j < (int) controllers.size(); j++ ) 	
				{
					if( controllers[j]["path"] == controllers[i]["parent tab"] )
					{
						if( !(intval( controllers[j]["type"] ) && MENU_IS_LOCAL_TASK) ) // Assign children if they are not LOCAL_TASK
						{
							controllers[i]["visible_tree"] = str(j);
						}
					}
				}
			}

			for( j = 0; j < (int) controllers.size(); j++ ) 	{
				if( controllers[j]["path"] == parent_tab ) 
					controllers[i]["parent tab"] = str( j );
			}
		}
			
		//if( !(intval( controllers[i]["type"] ) && MENU_IS_LOCAL_TASK) ) // Assign children if they are not LOCAL_TASK
		{
			for( j = 0; j < (int) controllers.size(); j++ )
			{
				if( controllers[j]["path"] == parent)
				{
					if( !(intval( controllers[j]["type"] ) && MENU_IS_LOCAL_TASK) )
						controllers[i]["parent"] = str( j );
					
					controllers[j]["children"] = controllers[j]["children"] + (!(controllers[j]["children"].empty())?",":"") + str(i);
				}
			}
		}

		bool has_access = true;
		if( !(intval(controllers[i]["type"]) && MENU_VISIBLE_IN_TREE) )
		{
			if( !controllers[i]["access callback"].empty() )
			{
				has_access = _ACCESS_CALLBACKS[ controllers[i]["access callback"] ]( controllers[i]["access arguments"] );
			}
			else if( !controllers[i]["access arguments"].empty() )
			{
				has_access = user_access( controllers[i]["access arguments"] );
			}
		}

		if( !has_access )
		{	
			controllers[i]["type"] = str( ~MENU_VISIBLE_IN_TREE );
		}
	}

	for( i = 0; i < (int) controllers.size(); i++ )
	{
		if( !isset(controllers[i]["parent"]) ) controllers[i]["parent"] = "-1";
	}

	//explain( controllers );
}

string theme_menu_tree(int pid = -1) 
{
	string tree;
	tree = menu_tree(pid);
	if (!tree.empty()) 
		return "\n<ul class=\"menu\">\n" + tree + "\n</ul>\n";
	else
		return "";
}

string username_theme( map <string, string> node )
{
	return "<a href=\""+ url( "user/"+node["uid"] )+"\">"+node["name"]+"</a>";
}

string node_submitted_theme( map <string, string> node )
{
	return "Submitted by " + theme("username", node) + " on " + format_date(node["created"]);
}

void theme_init_default()
{
	_THEME_HOOKS["username_theme"] = username_theme;
	_THEME_HOOKS["node_submitted_theme"] = node_submitted_theme;
}

void page_theme( map <string, string> item )
{
	string out;
	out = "\
<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\
<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\"\" xml:lang=\"\">\n\
<head>\n\
<base href=\""+string( BASE_URL )+"/\" />\n\
"+stored_head+"\
<link rel=\"stylesheet\" type=\"text/css\" media=\"all\" href=\"etc/binarytiers.css\" />\n\
<title>"+ item["title"] +"</title>\n\
</head>\n\
<body>\n\
<table border=\"0\" cellpadding=\"0\" cellspacing=\"0\" id=\"content\">\n\
	<tr>\n\
	";
if( !item["sidebar-left"].empty() ) out += "<td id=\"sidebar-left\">"+ item["sidebar-left"] +"</td>\n";
out += "\
		<td valign=\"top\">\n\
		<div id=\"main\">\n\
		"+ item["messages"] +"\n\
		"+ item["content"] +"\n\
		</div>\n\
		</td>\n\
	</tr>\n\
</table>\n\
</body>\n\
</html>\n\
";
	print (out);
}

/** 
 * The str_replace() function replace all occurrences of the search string by replace in the main string pointed by subject
 * @param search The string being search for
 * @param replace The replacement string that replaces all found search values
 * @param subject String being searched and replaced on
 * @returns Returns the string with the replaced values
 * @par Example:
 * @code
 * string body = "Date\nTitle\nDescription";
 * out = str_replace("\n","<br>", body);
 * print( out );
 * @endcode @ingroup group14
 */
char *str_replace( const char *search, const char *replace, const char *subject)
{
	char *ret, *sr;
	size_t i, count = 0;
	size_t replace_len = strlen(replace);
	size_t search_len = strlen(search);

	if (replace_len != search_len) {
		for (i = 0; subject[i] != (char) NULL; ) {
			if (memcmp(&subject[i], search, search_len) == 0)
			count++, i += search_len;
			else
			i++;
		}
	} else
		i = strlen(subject);

	ret = (char *) malloc(i + 1 + count * (replace_len - search_len));
	if (ret == NULL)
	return NULL;

	sr = ret;
	while (*subject) {
		if (memcmp(subject, search, search_len) == 0) {
			memcpy(sr, replace, replace_len);
			sr += replace_len;
			subject += search_len;
		} else
			*sr++ = *subject++;
	}
	*sr = (char) NULL;

	return ret;
} 

/**
 * This is an overloaded member function, provided for convenience. It differs from the above function only in what argument(s) it accepts.
 * @ingroup group14
 */
string str_replace(string search, string replace, string subject)
{
	string dest;
	char * ret = str_replace( search.c_str(), replace.c_str(), subject.c_str() );
	dest = string(ret);
	free(ret);
	return dest;
}

/**
 * The htmlencode() function convert special characters to HTML entities.
 * - & (ampersand) becomes &amp;amp;
 * - " (double quote) becomes &amp;quot;
 * - ' (single quote) becomes &amp;#39;
 * - < (less than) becomes &amp;lt;
 * - > (greater than) becomes &amp;gt;
 * .
 * @param str String to be converted
 * @returns The converted string
 * @par Example:
 * @code
 * string rss_item;
 * rss_item += "\
 * <item>\n\
 * <title>"+htmlencode( node["title"] )+"</title>\n\
 * <description>"+htmlencode( node["body"] )+"</description>\n\
 * </item>";
 * @endcode
 * @ingroup encode
 */
string htmlencode( string str )
{
	string out;
	char * res1;
	char * res2;

	res1 = str_replace( "&", "&amp;", str.c_str() );
	res2 = str_replace( "\'", "&#039;", res1 );
	free(res1);
	res1 = str_replace( "\"", "&quot;", res2 );
	free(res2);
	res2 = str_replace( "<", "&lt;", res1 );
	free(res1);
	res1 = str_replace( ">", "&gt;", res2 );
	free(res2);
	
	out = string(res1);
	free(res1);
	
	return out;
}

string replace_wildcards( string match )
{
	string dest, param;
	size_t cut;

	int count = 0;

	while( (cut = match.find_first_of('/')) != match.npos )
	{
		if(cut > 0)
		{
			param = match.substr(0, cut);
			if(param=="%") param = arg( count );
			
			dest += param + "/";
			count ++;
		}
		match = match.substr(cut+1);
	}

	param = match.substr(0, match.length() );
	if(param=="%") param = arg( count );

	dest += param;

	return dest;
}

int match_pattern( string input, string match )
{
	int found = true;
	size_t cut, cut2;
	string param;

	int i_match =0;
	int i_input =0;

	while( (cut = match.find_first_of('/')) != match.npos )
	{
		if(cut > 0)
		{
			if( (cut2 = input.find_first_of('/') ) != match.npos )
			{
				param = match.substr(0, cut);
				
				if(param != "%")
				{
					if( input.substr(0, cut2) != param ) 
					{
						found = false;
						break;
					}
				}
				i_input ++;
			}
		}
		match = match.substr(cut+1);
		input = input.substr(cut2+1);
		i_match ++;
	}
	
	if(i_match != i_input) found = false;

	if(found)
	{
		if( match != "%" )
		{
			if( input != match ) 
				found = false;
		}
		else
		{
			if( input.empty() ) found = false;
			if( ( cut = input.find_first_of('/') ) != string::npos )
			{
				if( cut != input.length()-1 )
					found = false;
			}
		}
	}

	return found;
}

/**
 * The url() function generates an internal URL to the web application. This function takes into account the existance of the @p path plugin
 * @param path The relative path to the page that we want to create the URL for
 * @param absolute A boolean value indicating whether the URL we want is absolute or not
 * @returns A string containing the requested URL
 * @par Example:
 * @code
 * redirect( url( "admin/aggregator" ) );
 * @endcode @ingroup global
 */
string url( string path, bool absolute )
{
	map <string, string> options;
	options["absolute"] = str( (int) absolute );
	return url( path, &options );
}

/**
 * This is an overloaded member function, provided for convenience. It differs from the above function only in what argument(s) it accepts.
 * @param path The relative path to the page that we want to create the URL for
 * @param options An optional pointer to an associative array containing the options:
 * <ul>
 * <li> ["query"] = The URL parameters
 * <li> ["url"] = The relative path for the URL to be generated
 * <li> ["absolute"] = _TRUE to indicate that the URL we want is absolute
 * </ul>
 * @ingroup global
 */
string url( string path, Assoc *options )
{
	string query = "";
	string url = BASE_URL;
	int absolute = false;

	if( options )
	{
		if( isset((*options)["query"]) ) query = (*options)["query"];
		if( isset((*options)["url"]) ) url = (*options)["url"];
		if( isset((*options)["absolute"]) ) 
		{
			url = "";
			absolute = intval( (*options)["absolute"] );
		}
	}
	
	if( clean_url == 0)
	{	
		url += "?q="+path;
		if( isset(query) )
		{
			ltrim( query, '&' );
			url += "&"+query;
		}
	}
	else
	{
		if( !absolute ) url += "/";

		if( model_exists("path") )
		{
			string dst;
			if(DB_TYPE==1)
				dst = redis_str("HGET url_alias:%s %s", language["language"].c_str(), path.c_str() );
			if(DB_TYPE==2)
				dst = db_result(db_querya("SELECT dst FROM url_alias WHERE src='%s' AND language='%s'", path.c_str(), language["language"].c_str() ));
			if( isset(dst) ) path = dst;
		}

		url += path;

		if( isset(query) ) 
		{
			ltrim( query ,'&' );
			url += "?"+query;
		}
	}
	return url;
}

/**
 * The t() function gets the translated value of a string pointed by the parameter @c text. The t() search for the string in the BinaryTiers dictionary and returns its value.
 * @param text The string to be translated
 * @param langcode The language code to translated the string. If this parameter is not specified the function uses the current langcode selected by the user
 * @returns The translated string
 * @par Example:
 * @code
 * print( t("Title") ); // Outputs the word title in the current langcode
 * @endcode
 * @ingroup group13
 */
string t( string text, string langcode )
{
	langcode = isset(langcode) ? langcode : language["language"];

	if( model_exists("locale") )
	{
		return locale_locale( text, langcode );
	}
	else
	{
		return text;
	}
}

/**
 * The arg() function gets the value of the url argument specified by @c num
 * @param num The number of the argument to be retreived)
 * @returns A string with the value of the argument
 * @par Example:
 * @code
 * string nid = arg(1);
 * print( nid ); // Outputs the second parameter
 * @endcode
 * @ingroup global
 */
string arg( int num )
{
	int n = 0;
	size_t cutParam;
	string input = _GET["q"] + "/";
	while( (cutParam = input.find_first_of('/')) != input.npos )
	{
		if(cutParam > 0)
		{
			if(n==num) 
				return input.substr(0, cutParam);
			n++;
		}
		input = input.substr(cutParam+1);
	}
	return string("");
}

string vformat(va_list args, const char *format)
{
	string dest = "", s;
	
	char * escaped;
	const char *str1 = format;

	while( *str1 )
	{
		if(*str1=='%')
		{
			str1++;
			switch(*str1)
			{
			case 'd':
				s = va_arg( args, const char *);
				dest += num(s);
				break;
			case 's':
				s = va_arg( args, const char *);
				escaped = (char *) malloc( s.size() * 2 );
				mysql_real_escape_string(&mysql, escaped, s.c_str(), (int) s.size() );
				dest += escaped;
				free(escaped);
				break;
			default:
				dest += *str1;
			}
		}
		else
		{
			dest += *str1;
		}
		str1++;
	}
	return dest;
}

MYSQL_RES * db_queryad(const char * format, ...)
{
	string query;

	if( DB_TYPE != 2 ) return NULL;

	va_list args;
	va_start(args, format);
	query = vformat(args, format );
	va_end(args);
	
	print (query);

	if ( mysql_query(&mysql, query.c_str()) !=0)
	{	
		if(!disable_errors) print ( string(mysql.net.last_error) + "<br>\n" + query );
		return NULL;
	}

	return mysql_store_result(&mysql);
}

MYSQL_RES * pager_query(string query, string count_query, int limit)
{
	string	page = isset( _GET["page"] ) ? _GET["page"] : "0";

	pager_total_items = intval( db_result( db_querya( count_query.c_str() ) ) );
	pager_page = atoi( page.c_str() );
	pager_total = (int) ceil( (double) pager_total_items / limit );
	pager_page = max(0, min( pager_page , pager_total - 1));

	query += " LIMIT " + str( pager_page * limit ) + "," + str( limit );

	return db_querya( query.c_str() );
}

MYSQL_RES * pager_query(string query, int limit)
{
	map <string, string> item;

	string	low = strtolower( query );
	size_t	cut = low.find("from");
	size_t	cut2 = low.find("group by");

	if(cut2 != string::npos ) cut2 = cut2-cut;

	string	count_query = "SELECT count(*) " + query.substr( cut, cut2 );

	return pager_query(query, count_query, limit);
}

string theme_round(string out)
{
	return "<div class=\"box3\">"+out+"<div class=\"corner3 topLeft3\"></div><div class=\"corner3 topRight3\"></div><div class=\"corner3 bottomLeft3\"></div><div class=\"corner3 bottomRight3\"></div></div>";
}

string format_attributes(map <string, string> &attributes)
{
	string result = "";
	map <string, string>::iterator curr, end;

	for( curr = attributes.begin(), end = attributes.end();  curr != end;  curr++ )
	{
		if(isset( curr->second ) )
			result += " "+curr->first+"=\""+curr->second+"\"";
	}

	return result;
}

/**
 * The implode() function join the array elements pointed by pieces with a glue string. The implode() function may be also used to join associative arrays allowing an additional glue string pointed by keyglue
 * @param pieces Array of string to implode
 * @param glue String used to link between elements of the array or associative array
 * @param key_glue String used to link between keys and values of the associative array
 * @returns Returns a string containing the representation of the array, with the glue string between each element
 * @par Example:
 * @code
 * vector <string> param;
 * param.push_back("email");
 * param.push_back("name");
 * param.push_back("phone");
 * implode( param, "&" ); // Returns email&name&phone
 * @endcode
 * @see explode() inarray()
 * @ingroup group1
 */
string implode(map <string, string> &pieces, string glue, string key_glue)
{
	string result = "";
	map <string, string>::iterator curr, end;

	for( curr = pieces.begin(), end = pieces.end();  curr != end;  curr++ )
	{
		if( curr != pieces.begin() ) result += glue;
		result += curr->first+key_glue+curr->second;
	}

	return result;
}

/**
 * This is an overloaded member function, provided for convenience. It differs from the above function only in what argument(s) it accepts.
 * @ingroup group1
 */
string implode(vector <string> &pieces, string glue)
{
	string result = "";
	
	for( unsigned int i = 0; i < pieces.size(); i++ )
	{
		if( i>0 ) result += glue;
		result += pieces[i];
	}

	return result;
}

/**
 * The explode() function splits a main string into an array or an associative array
 * @param pieces Array or associative array created by splitting the source parameter
 * @param the_string The input string to be split
 * @param separator The boundary character to separate elements into the array
 * @returns None
 * @par Example:
 * @code
 * map <string, string> values;
 * string parameters = "name=John&id=32&phone=5552322&page=2";
 * explode( values, parameters, '&', '=' );
 * explain( values );
 * @endcode
 * @see implode() inarray()
 * @ingroup group1
 */
void explode(vector <string> &pieces, string the_string, const char *separator)
{
	size_t cutParam;
	if( the_string.length()>0 )
	{
		the_string += string(separator);
		while( (cutParam = the_string.find_first_of(separator)) != the_string.npos )
		{
			if(cutParam > 0)
			{
				pieces.push_back( the_string.substr(0, cutParam) );
			}
			else
			{
				pieces.push_back( "" );
			}
			the_string = the_string.substr(cutParam+1);
		}
	}
}

/**
 * This is an overloaded member function, provided for convenience. It differs from the above function only in what argument(s) it accepts.
 * @ingroup group1
 */
void explode(vector <string> &pieces, string the_string, char separator)
{
	size_t cutParam;
	if( the_string.length()>0 )
	{
		the_string += separator;
		while( (cutParam = the_string.find_first_of(separator)) != the_string.npos )
		{
			if(cutParam > 0)
			{
				pieces.push_back( the_string.substr(0, cutParam) );
			}
			else
			{
				pieces.push_back( "" );
			}
			the_string = the_string.substr(cutParam+1);
		}
	}
}

/**
 * This is an overloaded member function, provided for convenience. It differs from the above function only in what argument(s) it accepts.
 * @param pieces Array or associative array created by splitting the source parameter
 * @param the_string The input string to be split
 * @param separator The boundary character to separate elements into the array
 * @param keyseparator The boundary character to separate the keys from values in an associative array 
 * @ingroup group1
 */
void explode(map <string, string> &pieces, string the_string, char separator, char keyseparator)
{
	size_t cutParam, cutKey;

	if( the_string.length()>0 )
	{
		the_string += separator;
		while( (cutParam = the_string.find_first_of(separator)) != the_string.npos )
		{
			if(cutParam > 0)
			{
				string line = the_string.substr(0, cutParam);
				cutKey = line.find_first_of(keyseparator);
				if(cutKey > 0)
				{
					string key = line.substr(0,cutKey);
					string value = line.substr(cutKey+1);
					pieces[key] = value;
				}
			}
			the_string = the_string.substr(cutParam+1);
		}
	}
}

string pager_get_querystring()
{
	string query = "";
	map <string, string>::iterator curr, end;

	for( curr = _GET.begin(), end = _GET.end();  curr != end;  curr++ )
	{
		if( curr->first != "q" && curr->first != "page" )
		{	
			if( isset( curr->second ) )
				query += "&"+curr->first+"="+curr->second;
		}
	}

	for( curr = _POST.begin(), end = _POST.end();  curr != end;  curr++ )
	{
		if( curr->first != "op" )
		{	
			if( isset( curr->second ) )
				query += "&"+curr->first+"="+curr->second;
		}
	}

	return query;
}

string theme_table( vector <string> &header, vector <vector <map <string, string> > > &rows, string table_attributes )
{
	unsigned int			i, j;
	map <string, string>	attributes;
	map <string, string>	cell;
	string					data;
	string					inline_attributes;
	string					out = (table_attributes.length()==0)?"<table>":"<table "+table_attributes+">";
	
	if(header.size()>0)
	{
		out += "<thead><tr>\n";
		for( i = 0; i < header.size(); i++ )
		{
			if(header[i]=="select-all")
				out += "<th class=\"select-all\"></th>";
			else
				out += "<th>"+header[i]+"</th>";
		}
		out += "</tr></thead>\n";
	}

	out += "<tbody>\n";

	for( i = 0; i < rows.size(); i++ )
	{
		attributes.clear();
		
		// Add odd/even class
		string rclass = (i % 2 == 1) ? "even": "odd";
		if (isset(attributes["class"])) {
			attributes["class"] += " " + rclass;
		}
		else {
			attributes["class"] = rclass;
		}

		out += "<tr"+format_attributes(attributes)+">\n";
		
		for( j = 0; j < rows[i].size(); j++ )
		{
			cell = rows[i][j];
			
			data = cell["data"];
			inline_attributes = isset( cell["attributes"] ) ? " "+cell["attributes"] : "";
			cell["data"].clear();
			cell["attributes"].clear();

			out += "<td"+format_attributes(cell)+inline_attributes+">";
			out += data;
			out += "</td>";
		}

		out += "</tr>\n";
	}

	out += "</tbody></table>";
	
	return out;
}

string theme_pager()
{
	string	out;
	string	classe;
	string	link;
	string	query = pager_get_querystring();

	out += "<div class=\"item-list\">\n<ul class=\"pager\">\n";

	if( pager_page > 0 )
	{
		map <string, string> options;
		options["query"] = query+"&page="+ str( pager_page-1 );
		link = url( _GET["q"], &options );
		out += "<li class=\"pager-item\"><a onmouseover=\"this.parentNode.className='pager-item2';\" onmouseout=\"this.parentNode.className='pager-item';\" href=\"" + link + "\"><div>Previous</div></a></li>\n";
	}

	int ini_page = pager_page - 5;
	int end_page = pager_page + 5 + 1;
	if(ini_page < 0) {
		end_page += -ini_page;
		ini_page = 0;
	}
	if(end_page > pager_total) {
		ini_page -= (end_page-pager_total);
		end_page = pager_total;
		if(ini_page<0) ini_page=0;
	}

	for(int i=ini_page; i<end_page; i++)
	{
		classe = "pager-item";

		if(i == pager_page) 
		{
			out += "<li class=\"pager-item2\"><div>" + str( i+1 ) + "</div></li>\n";
		}
		else
		{		
			map <string, string> options;
			options["query"] = query+"&page="+ str( i );
			link = url( _GET["q"], &options );
			out += "<li class=\""+classe+"\">" +  "<a onmouseover=\"this.parentNode.className='pager-item2';\" onmouseout=\"this.parentNode.className='pager-item';\" href=\""+link+"\"><div>" + str( i+1 ) + "</div></a>"  + "</li>\n";
		}
	}
	if( pager_page < pager_total-1 )
	{
		map <string, string> options;
		options["query"] = query+"&page="+ str( pager_page+1 );
		link = url( _GET["q"], &options );
		out += "<li class=\"pager-item\"><a onmouseover=\"this.parentNode.className='pager-item2';\" onmouseout=\"this.parentNode.className='pager-item';\" href=\"" + link + "\"><div>Next</div></a></li>\n";
	}
	
	out += "</ul>\n</div>\n";
	
	return out;
}

string sql_parse(const char * format, ...)
{
	string query;

	va_list args;
	va_start(args, format);
	query = vformat(args, format );
	va_end(args);

	return query;
}

MYSQL_RES * db_query(string format, string p1, string p2, string p3, string p4, string p5, string p6, string p7, string p8, string p9, string p10, string p11, string p12 )
{
	return db_querya(format.c_str(), p1.c_str(), p2.c_str(), p3.c_str(), p4.c_str(), p5.c_str(), p6.c_str(), p7.c_str(), p8.c_str(), p9.c_str(), p10.c_str(), p11.c_str(), p12.c_str() );
}

MYSQL_RES * db_queryd(string format, string p1, string p2, string p3, string p4, string p5, string p6, string p7, string p8, string p9, string p10, string p11, string p12 )
{
	return db_queryad(format.c_str(), p1.c_str(), p2.c_str(), p3.c_str(), p4.c_str(), p5.c_str(), p6.c_str(), p7.c_str(), p8.c_str(), p9.c_str(), p10.c_str(), p11.c_str(), p12.c_str() );
}

MYSQL_RES * db_querya(const char * format, ...)
{
	string query;

	if( DB_TYPE != 2 ) return NULL;

	va_list args;
	va_start(args, format);
	query = vformat(args, format );
	va_end(args);
	
	if ( mysql_query(&mysql, query.c_str()) !=0)
	{	
		if(!disable_errors) 
			set_page_message( string( mysql.net.last_error ) + " => " + query, "error" );
		return NULL;
	}

	return mysql_store_result(&mysql);
}

string db_result( MYSQL_RES *result )
{
	MYSQL_ROW row;
	string	res = "";

	if(result)
	{
		row = mysql_fetch_row(result);
		if(row)
		{				
			if(row[0])
				res = string(row[0]);
		}
		mysql_free_result(result);
	}

	return res;
}

string db_last_insert_id()
{
	//return db_result(db_querya("SELECT LAST_INSERT_ID()"));
	return str( (int) mysql_insert_id(&mysql) );
}

int db_affected_rows()
{
	return (int) mysql_affected_rows( &mysql );
}

int db_num_rows( MYSQL_RES *result )
{	
	return (int) result->row_count;
}

int db_fetch( MYSQL_RES *result, map <string, string> &item)
{
	MYSQL_ROW row;

	if(result)
	{
		row = mysql_fetch_row(result);
		if(row)
		{
			for(int i=0; i< (int) result->field_count; i++)
			{
				if(row[i])
					item[ result->fields[i].name ] = string( row[i] );
				else
					item[ result->fields[i].name ] = "";
			}
			return true;
		}
		mysql_free_result(result);
	}

	return false;
}

void destroy()
{
	removetemp();
}

void http_header( string header )
{
	printf("%s\r\n", header.c_str());
	sent_location = true;
}

void redirect( string path )
{
	printf("Location: %s\r\n", path.c_str());
	sent_location = true;
	main_output = "";
}

void setcookie(string name, string value, string expire, string path, string domain, int secure, int httponly)
{
	string out = "Set-Cookie: "+name+"="+value;

	if( isset( expire ) ) out += "; expires="+gmdate("%a, %d-%b-%Y %H:%M:%S GMT", expire);
	if( isset( domain ) ) out += "; domain="+domain;
	if( isset( path ) ) out += "; path="+path;
	if( secure ) out += "; secure";
	if( httponly ) out += "; httponly";
	
	printf("%s\r\n", out.c_str());
}

void setcookie(string name, string value, int expire, string path, string domain, int secure, int httponly)
{
	setcookie(name, value, str( expire ), path, domain, secure, httponly);
}

void quit( int code )
{
	bool is_gzip = false;
	char *accept_encoding = getenv("HTTP_ACCEPT_ENCODING");
	if( accept_encoding != NULL )
	{
		if( string(accept_encoding).find("gzip") != string::npos )
			is_gzip = true;
	}

	//is_gzip = false;
	printf( "Cache-Control: store, no-cache, must-revalidate\r\n" );
	printf( "X-Powered-By: BinaryTiers/2.4.2\r\n" );
	printf( "Expires: Sun, 19 Nov 1978 05:00:00 GMT\r\n" );
	printf( "Last-Modified: %s\r\n", gmdate("%a, %d %b %Y %H:%M:%S GMT").c_str() );
	printf( "Cache-Control: post-check=0, pre-check=0\r\n" );

	if( !sent_location )
	{
		//main_output = str_replace("\t","",main_output);
		//main_output = str_replace("\n","",main_output);
		//main_output = str_replace("\r","",main_output);

		if( is_gzip )
		{
			CA2GZIP gzip( (char *) main_output.c_str(), (int) main_output.size() );
			printf( "Content-Encoding: gzip\r\n" );
			printf( "Content-Type: text/html; charset=utf-8\r\n\r\n" );
			set_binary( stdout );
			fwrite(gzip.pgzip, 1, gzip.Length, stdout);
		}
		else
		{
			printf( "Content-Type: text/html; charset=utf-8\r\n\r\n" );
			set_binary( stdout );
			fwrite((char *) main_output.c_str(), 1, (int) main_output.size(), stdout);
		}
	}
	else
	{
		printf( "\r\n" );
		set_binary( stdout );
		fwrite((char *) main_output.c_str(), 1, (int) main_output.size(), stdout);
	}
	
	destroy();

	timer_t2 = clock();
	// float diff = ((float)timer_t2 - (float)timer_t1);
	// printf("%f", diff);

	exit( code );
}

void do_multipart()
{
	size_t	cut;
	int i, j, match;

	string key;
	char boundary[100];
	char *buf = (char*) malloc( 4096 + _CONTENT_LENGTH + 2 );
	int nlist = 0;
	int len;
	int *list = (int *) malloc( sizeof(int) * (nlist+1) );

	string filename;
	
	cut = _CONTENT_TYPE.find("boundary=");
	strcpy( boundary, _CONTENT_TYPE.substr( cut+9 ).c_str() );

	i = 0;
	j = 0;

	bool track_progress = false;
	string sizename;
	if( isset( _COOKIE["BTFID"] ) )
	{
		sizename = sizefile( _COOKIE["BTFID"] );
		track_progress = true;
	}

	set_binary( stdin );	
	
	while (0 < (i = (int) fread (buf+j, 1, 4096, stdin)))
	{		
		j += i;
		
		if( track_progress )
		{
			FILE * fp = fopen(sizename.c_str(), "wb");
			if( !fp ) {
				print ( str_replace("%2", file_directory_temp(), str_replace("%1", sizename, "BT-ERROR 503: Can't create file [%1]. Make sure the TEMP_PATH [%2] is valid and has write permissions.") ) );
				quit(0);
			}
			string size = str(j);
			fprintf(fp, size.c_str(), size.length());
			fclose(fp);
		}
	}

	len = j;
	
	//len = (int) fread (buf, 1, _CONTENT_LENGTH, stdin);
	
	i = 0;
	j = 0;
	match = 0;
	
	for(i=0;i<len;i++)
	{
		if( buf[i] == boundary[j] )
		{
			if(match == strlen(boundary))
			{		
				list[nlist] = i + 2;
				nlist ++;
				list = (int *) realloc( list, sizeof(int) * (nlist+1) );
			}

			match ++;

			if(buf[i+1] == boundary[j+1])
				j++;
		}
		else
		{
			j=0;
			match=0;
		}
	}
	
	i = list[nlist-1];
	if( buf[i] != '-' || buf[i+1] != '-' )
	{
		print( "Invalid Attachment" );
	}

	char * pos, *ini, *end;
	int temp_len;
	char * tmp = (char) NULL;
	bool haslf = false;
	bool hasfile;
	
	for(i=0;i<nlist-1;i++)
	{
		j = list[i];

		ini = &buf[j];
		while( *ini != '\n' )
		{
			if(*ini==0x0D) haslf = true;
			ini ++;
		}
		ini ++;

		if( (end = strstr( ini, "\n" )) != NULL)
		{
			temp_len = (int) (end - ini);

			tmp = (char *) malloc( temp_len + 1);
			strncpy(tmp, ini, temp_len);
			tmp[temp_len] = (char) NULL;

			hasfile = false;
			key = "";
						
			if( (pos = strstr(tmp, " name=\"")) != NULL)
			{
				ini = pos + 7;
				pos = strstr( ini, "\"" );
				*pos = (char) NULL;
				key = string ( ini );				
				*pos = '\"';
			}

			if( (pos = strstr(tmp, "filename=\"")) != NULL)
			{
				hasfile = true;
				// has file

				ini = pos+10;
				pos = strstr( ini, "\"" );
				*pos = (char) NULL;
				_FILES[key]["filename"] = string( ini );
				_POST[key] = string( ini );
				// filename

				ini = end+1;
				if( (end = strstr( ini, "\n" )) != NULL)
				{
					temp_len = (int) (end - ini); 

					free(tmp);
					tmp = (char *) malloc( temp_len + 1);
					strncpy(tmp, ini, temp_len);
					tmp[temp_len] = (char) NULL;
					
					if( (pos = strstr( tmp, "Content-Type:" )) != NULL)
					{
						ini = pos+14;
						_FILES[key]["type"] = string( ini );
						// type
					}
				}
			}
			free(tmp);
		
			ini = end+1;
			if( (ini = strstr( ini, "\n" )) != NULL)
			{
				ini++;
				j = list[i+1];
				end = &buf[j];
				end--;

				while( *end != '\n' ) end --;
				if(haslf) end--;

				temp_len = (int) (end - ini);
				if( !hasfile )
				{
					*end = (char) NULL;
					if( isset2(_POST,key) )
					{
						string key2;
						int n;

						key2 = key+"[0]";
						if( !isset2( _POST,key2 ) )
						{
							_POST[key2] = _POST[key];
						}

						n = 1;
						key2 = key+"["+str(n)+"]";
						while( isset2(_POST,key2) )
						{
							n++;
							key2 = key+"["+str(n)+"]";
						}						

						_POST[key2] = string( ini );
						_POST[key] = "#array["+str( n+1 )+"]";
					}
					else
					{
						_POST[key] = string( ini );
					}
				}
				else
				{
					filename = tempfile();

					if(temp_len>0)
					{
						FILE * fp = fopen( filename.c_str(), "wb");
						if( !fp ) {
							print ( str_replace("%2", file_directory_temp(), str_replace("%1", filename, "BT-ERROR 503: Can't create file [%1]. Make sure the TEMP_PATH [%2] is valid and has write permissions.") ) );
							quit(0);
						}
						len = (int) fwrite( ini, 1, temp_len , fp );
						fclose(fp);

						_FILES[key]["size"] = str( len );
						_FILES[key]["tempname"] = filename;
					}
				}
			}
		}

	}
	
	free(buf);
	free(list);
}

void globals_init()
{
	string input, line, key, value;

	if( getenv("HTTP_COOKIE") )
	{
		input = getenv("HTTP_COOKIE");
		if(input.length()>0)
		{	
			input += ";";
			size_t cutParam, cutKey;
			while( (cutParam = input.find_first_of(';')) != input.npos )
			{
				if(cutParam > 0)
				{
					line = input.substr(0, cutParam);
					cutKey = line.find_first_of('=');
					if(cutKey > 0)
					{
						key = line.substr(0,cutKey);
						trim(key);
						value = line.substr(cutKey+1);
						_COOKIE[key] = urldecode(value);
					}
				}
				input = input.substr(cutParam+1);
			}
		}
	}

	if( getenv("QUERY_STRING") )
	{
		input = getenv("QUERY_STRING");
		if(input.length()>0)
		{	
			input += "&";
			size_t cutParam, cutKey;
			while( (cutParam = input.find_first_of('&')) != input.npos )
			{
				if(cutParam > 0)
				{
					line = input.substr(0, cutParam);
					cutKey = line.find_first_of('=');
					if(cutKey > 0)
					{
						key = line.substr(0,cutKey);
						value = line.substr(cutKey+1);
						_GET[key] = urldecode(value);
					}
				}
				input = input.substr(cutParam+1);
			}
		}
	}
	
	if(getenv("CONTENT_LENGTH")!=NULL)
	{
		_CONTENT_LENGTH = atoi( getenv("CONTENT_LENGTH") );
		
		if( isset( _COOKIE["BTMCL"] ) ) max_content_length = intval( _COOKIE["BTMCL"] );

		if( _CONTENT_LENGTH > max_content_length )
		{
			set_page_message( t("The file you are trying to upload is too big. The maximum allowed size is: ")+ format_size( str(max_content_length) ), "error");
		}
		else
		{
			if( getenv("CONTENT_TYPE") != NULL ) 
				_CONTENT_TYPE = string( getenv("CONTENT_TYPE") );
			
			if( _CONTENT_TYPE.find( "multipart/form-data" ) != _CONTENT_TYPE.npos )
			{
				do_multipart();
			}
			else
			{
				while(cin) 
				{
					getline(cin, input);
				}
				
				size_t cutParam, cutKey;
				input += "&";
				while( (cutParam = input.find_first_of('&')) != input.npos )
				{
					if(cutParam > 0)
					{
						line = input.substr(0, cutParam);
						cutKey = line.find_first_of('=');
						if(cutKey > 0)
						{
							key = line.substr(0,cutKey);
							value = line.substr(cutKey+1);
							
							value = urldecode(value);
	//						strip( (char *) value.c_str(), '\r' );
							_POST[key] = value;
						}
					}
					input = input.substr(cutParam+1);
				}
			}

		}
	}
}

string serialize_string( string data )
{
	return "s:"+str((int) data.length())+":\""+data+"\";";
}

string unserialize_string( string data, size_t * ret_pos = NULL )
{
	string type;
	string value;
	size_t cut;
	size_t pos;
	size_t len;

	pos = 0;	
	if( ret_pos != NULL ) pos = *ret_pos;

	type = data.substr( pos, 1 );

	switch( *(type.c_str()) )
	{
		case 's':
			pos +=2;
			cut = data.find_first_of(':', pos);
			if(cut != data.npos)
			{
				len = intval( data.substr( pos, cut-pos ) );
				pos = cut + 2;
				value = data.substr(pos, len);
				pos +=2; // salta ";
			}
			break;
		 case 'i': case 'b':
			pos +=2;
			cut = data.find_first_of(';', pos);
			if(cut != data.npos)
			{
				len = cut-pos;
				value = data.substr( pos, len );
				pos ++; // salta ;
			}
			break;
	}

	if( ret_pos != NULL ) *ret_pos = pos + len;

	return value;
}

void nest( vector <string> &value, map <string, string> *item, string var )
{
	size_t len = value.size();

	if(len == 0) 
		(*item)[var] = "";
	else if(len == 1) {
		(*item)[var] = value[0];
	} else {
		(*item)[var] = "#array["+str(len)+"]";
		for( size_t i=0; i<len; i++) {
			(*item)[var+"["+str(i)+"]"] = value[i];
		}
	}
}

void nestclear( map <string, string> *item, string var )
{
	size_t	cut, len;
	
	if( (cut = (*item)[var].find("#array[")) != string::npos && cut == 0) // is an array
	{
		len = intval( (*item)[var].substr(7) );
		for( int i=0; i<(int)len; i++) {
			(*item).erase( (*item).find(  var+"["+str(i)+"]" ) );
		}
	}
	(*item).erase( (*item).find( var ) );	
}

void unnest( map <string, string> *item, string var, vector <string> &value )
{
	size_t	cut, len;
	
	if( (cut = (*item)[var].find("#array[")) != string::npos && cut == 0) // is an array
	{
		len = intval( (*item)[var].substr(7) );
		for( int i=0; i<(int)len; i++) {
			value.push_back( (*item)[var+"["+str(i)+"]"] );
		}
	}
	else
	{
		if( isset( (*item)[var] ) ) value.push_back( (*item)[var] );
	}
}

void nestcopy( map <string, string> *item, string var, map <string, string> *dest_item, string dest_var )
{
	size_t	cut, len;
	
	if( (cut = (*item)[var].find("#array[")) != string::npos && cut == 0) // is an array
	{
		(*dest_item)[dest_var] = (*item)[var];
		len = intval( (*item)[var].substr(7) );
		for( int i=0; i<(int)len; i++) {
			(*dest_item)[dest_var+"["+str(i)+"]"] = (*item)[var+"["+str(i)+"]"];
		}
	}
	else
	{
		if( isset( (*item)[var] ) ) (*dest_item)[dest_var] = (*item)[var];
	}
}

string serialize_array( vector <pair <string, string> > data )
{
	string out;
	
	out = "s:" + str( (int) data.size() ) + ":{";

	for( size_t i = 0; i< data.size(); i++ )
		out += serialize_string( data[i].first ) + serialize_string( data[i].second );
	
	out+= "}";
	return out;
}

string serialize_array( map <string, string> data )
{
	string out;
	map <string, string>::iterator row, row_end;

	out = "s:" + str( (int) data.size() ) + ":{";

	for( row = data.begin(), row_end = data.end();  row != row_end;  row++ )
		out += serialize_string( row->first ) + serialize_string( row->second );
	
	out+= "}";
	return out;
}

void unserialize_array( string data, vector <pair <string, string> > &result )
{
	string	key, value;
	size_t cut;
	size_t pos = 2;
	size_t i, len;

	if ( data[0] == 's' && data[1] == ':' && isdigit(data[2]) )
	{
		cut = data.find_first_of(':', pos);
		if(cut != data.npos)
		{
			len = intval( data.substr( pos, cut-pos ) );
			pos += (cut-pos) + 2;

			for( i = 0; i< len; i++ )
			{
				key = unserialize_string( data, &pos );
				value = unserialize_string( data, &pos );
				
				result.push_back( make_pair( key, value ) );
			}
		}
	}
	else if ( data[0] == '{' && data[data.length()-1] == '}' )
	{
		// Is an array of this form: {AL:Albania;DZ:Algeria;...}

		vector <string> pieces;
		explode(pieces, data.substr(1, data.length()-2), ';');

		for( i = 0; i< pieces.size(); i++ )
		{
			vector <string> key_value;
			explode(key_value, pieces[i], ':');
			
			if( key_value.size() == 1)
			{
				result.push_back( make_pair( "", key_value[0] ) );
			}
			if( key_value.size() == 2)
			{
				result.push_back( make_pair( key_value[0], key_value[1] ) );
			}
		}		
	}
}

void unserialize_array( string data, map <string, string> &result )
{
	string	key, value;
	size_t cut;
	size_t pos = 2;
	size_t i, len;

	if ( data[0] == 's' && data[1] == ':' && isdigit(data[2]) )
	{
		cut = data.find_first_of(':', pos);
		if(cut != data.npos)
		{
			len = intval( data.substr( pos, cut-pos ) );
			pos += (cut-pos) + 2;

			for( i = 0; i< len; i++ )
			{
				key = unserialize_string( data, &pos );
				value = unserialize_string( data, &pos );

				result[ key ] = value;
			}
		}
	}
	else if ( data[0] == '{' && data[data.length()-1] == '}' )
	{
		// Is an array of this form: AL:Albania;DZ:Algeria;

		vector <string> pieces;
		explode(pieces, data.substr(1, data.length()-2), ';');

		for( i = 0; i< pieces.size(); i++ )
		{
			vector <string> key_value;
			explode(key_value, pieces[i], ':');
			
			if( key_value.size() == 2)
			{
				result[ key_value[0] ] = key_value[1];
			}
		}		
	}
}

int redis_int(const char * format, ...)
{	
	int ret = 0;
	va_list ap;
    va_start(ap,format);
    redisReply * reply = (redisReply *) redisvCommand( redis, format, ap );
	if( reply->type == REDIS_REPLY_INTEGER )
		ret = (int) reply->integer;
	if( reply->type == REDIS_REPLY_STRING )
		ret = atoi( reply->str );
    va_end(ap);
	freeReplyObject(reply);
	return ret;
}

string redis_str(const char * format, ...)
{	
	string ret = "";
	va_list ap;
    va_start(ap,format);
    redisReply * reply = (redisReply *) redisvCommand( redis, format, ap );
	if( reply->type == REDIS_REPLY_INTEGER )
		ret = str( reply->integer );
	if( reply->type == REDIS_REPLY_STRING )
		ret = string( reply->str );
	if( reply->type == REDIS_REPLY_ARRAY )
	{
		if( reply->element[0]->type == REDIS_REPLY_INTEGER )
			ret = str( reply->element[0]->integer );
		if( reply->element[0]->type == REDIS_REPLY_STRING )
			ret = string( reply->element[0]->str );
	}
    va_end(ap);
	freeReplyObject(reply);
	return ret;
}

REDIS_RES * redis_query(const char * format, ...)
{
	REDIS_RES * res = new REDIS_RES;
	
	va_list ap;
	va_start(ap,format);
	res->redis_reply = (redisReply *) redisvCommand( redis, format, ap );
	va_end(ap);
	res->redis_count = 0;
	res->redis_fields = NULL;
	
	return res;
}

void redis_command(const char * format, ...)
{
	va_list ap;
	va_start(ap,format);
	redisReply * reply = (redisReply *) redisvCommand( redis, format, ap );
	va_end(ap);
	freeReplyObject(reply);
}

string redisvFormat(const char *format, va_list ap)
{
    const char *arg, *c = format;
    char *cmd = NULL; /* final command */
    
    /* Build the command string accordingly to protocol */
    string current = "";
    while(*c != '\0') {
        if (*c != '%' || c[1] == '\0') {
            current += c[0];
        } else {
            switch(c[1]) {
            case 's':
                arg = va_arg(ap,char*);
                current += string( arg );
                break;
            case '%':
                current += "%";
                break;
            default:
                /* Try to detect printf format */
                {
					char _dest[16];
                    char _format[16];
                    const char *_p = c+1;
                    size_t _l = 0;
                    va_list _cpy;

                    /* Flags */
                    if (*_p != '\0' && *_p == '#') _p++;
                    if (*_p != '\0' && *_p == '0') _p++;
                    if (*_p != '\0' && *_p == '-') _p++;
                    if (*_p != '\0' && *_p == ' ') _p++;
                    if (*_p != '\0' && *_p == '+') _p++;

                    /* Field width */
                    while (*_p != '\0' && isdigit(*_p)) _p++;

                    /* Precision */
                    if (*_p == '.') {
                        _p++;
                        while (*_p != '\0' && isdigit(*_p)) _p++;
                    }

                    /* Modifiers */
                    if (*_p != '\0') {
                        if (*_p == 'h' || *_p == 'l') {
                            /* Allow a single repetition for these modifiers */
                            if (_p[0] == _p[1]) _p++;
                            _p++;
                        }
                    }

                    /* Conversion specifier */
                    if (*_p != '\0' && strchr("diouxXeEfFgGaA",*_p) != NULL) {
                        _l = (_p+1)-c;
                        if (_l < sizeof(_format)-2) {
                            memcpy(_format,c,_l);
                            _format[_l] = '\0';
                            //va_copy(_cpy,ap);
							_cpy = ap;

                            _l = vsprintf(_dest, _format, _cpy);
							_dest[_l] = '\0';
							current += string( _dest );
                            va_end(_cpy);

                            /* Update current position (note: outer blocks
                             * increment c twice so compensate here) */
                            c = _p-1;
                        }
                    }

                    /* Consume and discard vararg */
                    va_arg(ap,void*);
                }
            }
            c++;
        }
        c++;
    }

    return current;
}

string redis_arg(const char *format, ...)
{
	string repeat;
	va_list ap;
	va_start(ap,format);
	repeat = redisvFormat(format,ap);
	va_end(ap);
	return repeat;
}

REDIS_RES * redis_query_fields(string command, string repeat, string the_fields)
{
	REDIS_RES * res = new REDIS_RES;
	
	vector <string> fields;
	explode(fields, the_fields, ",");
	for(size_t i=0; i<fields.size(); i++)
	{
		trim( fields[i] );
		if(fields[i].substr(0,1)=="#") command += " GET #";
		else command += " "+repeat+fields[i];
	}

	res->redis_fields = (char *) malloc( the_fields.size() + 1 );
	memcpy( res->redis_fields, the_fields.c_str(), the_fields.size() );
	res->redis_fields[ the_fields.size() ] = '\0';

	res->redis_reply = (redisReply *) redisCommand( redis, command.c_str() );
	res->redis_count = 0;

	return res;
}

REDIS_RES * redis_pager_fields(string command, string repeat, string the_fields, int limit)
{
	string	low = strtolower( command );
	size_t	cut = low.find("sort ");
	size_t	cut2 = low.find(" by ");

	if(cut == string::npos ) return NULL;
	
	cut += 5;
	if(cut2 != string::npos ) cut2 = cut2-cut;
	else cut2 = low.length()-cut;

	string	page = isset( _GET["page"] ) ? _GET["page"] : "0";
	string	count_query = "SCARD " + command.substr( cut, cut2 );

	pager_total_items = redis_int( count_query.c_str() );

	pager_page = atoi( page.c_str() );
	pager_total = (int) ceil( (double) pager_total_items / limit );
	pager_page = max(0, min( pager_page , pager_total - 1));

	command += " LIMIT " + str( pager_page * limit ) + " " + str( limit );

	return redis_query_fields(command, repeat, the_fields );
}

void redis_command_fields(string command, string repeat, string the_fields, const char *values, ...)
{
	vector <string> fields;
	vector <string> vals;
	explode(fields, the_fields, ",");

	if( values ) {
		explode(vals, string(values), ",");
	}

	for(size_t i=0; i<fields.size(); i++)
	{
		trim( fields[i] );
		command += " "+repeat+fields[i];
		if( i<vals.size() )
		{
			trim( vals[i] );
			command += " "+vals[i];
		}
	}

	//redisReply * reply = (redisReply *) redisCommand( redis, command.c_str() );


	va_list ap;
	va_start(ap,values);
	redisReply * reply = (redisReply *) redisvCommand( redis, command.c_str(), ap );
	va_end(ap);


	freeReplyObject(reply);
	return;
}

void redis_free( REDIS_RES *res )
{
	freeReplyObject(res->redis_reply);
	res->redis_reply=NULL;
	if(res->redis_fields) free(res->redis_fields);
	res->redis_fields=NULL;
	delete res;
}

bool redis_fetch( REDIS_RES *res, string &item )
{
	if( !res->redis_reply ) return false;

	if( res->redis_reply->type == REDIS_REPLY_ARRAY )
	{
		if( res->redis_count >= (int) res->redis_reply->elements )
		{
			redis_free( res );
			return false;
		}
		else if( res->redis_reply->element[res->redis_count]->type == REDIS_REPLY_STRING )
		{
			item = res->redis_reply->element[res->redis_count]->str;
			res->redis_count++;
		}
		else if( res->redis_reply->element[res->redis_count]->type == REDIS_REPLY_INTEGER )
		{
			item = str( res->redis_reply->element[res->redis_count]->integer );
			res->redis_count++;
		}
		else
		{
			item = "";
			res->redis_count++;
		}
	}
	else if( res->redis_reply->type == REDIS_REPLY_STRING )
	{
		item = res->redis_reply->str;
		redis_free( res );
	}
	else
	{
		redis_free( res );
		return false;
	}
	return true;
}

bool redis_fetch_fields( REDIS_RES *res, map <string, string> &item )
{
	bool ret = false;
	vector <string> fields;
	string key;

	if(!res) return false;

	explode( fields, string( res->redis_fields ), "," );
	for(size_t i=0; i<fields.size(); i++)
	{
		trim( fields[i] );
		if(fields[i].substr(0,1)=="#") key = fields[i].substr(1);
		else key = fields[i];
		ret = redis_fetch( res, item[ key ] );
	}
	return ret;
}

int redis_multi( vector <pair <string, string> > &item, const char * format, ... )
{
	int ret = -1;
	size_t j;
	
	va_list ap;
	va_start(ap,format);
	redisReply * reply = (redisReply *) redisvCommand( redis, format, ap );
	va_end(ap);

	if (reply->type == REDIS_REPLY_ARRAY) {
		ret = (int) reply->elements/2;
		for (j = 0; j < reply->elements; j+=2) {
			item.push_back( make_pair( reply->element[j]->str, reply->element[j+1]->str) );
		}
	}
	freeReplyObject(reply);
	return ret;
}

int redis_multi( map <string, string> &item, const char * format, ... )
{
	int ret = -1;
	size_t j;

	va_list ap;
	va_start(ap,format);
	redisReply * reply = (redisReply *) redisvCommand( redis, format, ap );
	va_end(ap);

	if (reply->type == REDIS_REPLY_ARRAY) {
		ret = (int) reply->elements/2;
		for (j = 0; j < reply->elements; j+=2) {
			item[ reply->element[j]->str ] = reply->element[j+1]->str;
		}
	}
	freeReplyObject(reply);
	return ret;
}

int redis_multi_reversed( vector <pair <string, string> > &item, const char * format, ... )
{
	int ret = -1;
	size_t j;
	
	va_list ap;
	va_start(ap,format);
	redisReply * reply = (redisReply *) redisvCommand( redis, format, ap );
	va_end(ap);

	if (reply->type == REDIS_REPLY_ARRAY) {
		ret = (int) reply->elements/2;
		for (j = 0; j < reply->elements; j+=2) {
			item.push_back( make_pair( reply->element[j+1]->str, reply->element[j]->str) );
		}
	}
	freeReplyObject(reply);
	return ret;
}

int redis_multi_reversed(  map <string, string> &item, const char * format, ... )
{
	int ret = -1;
	size_t j;

	va_list ap;
	va_start(ap,format);
	redisReply * reply = (redisReply *) redisvCommand( redis, format, ap );
	va_end(ap);

	if (reply->type == REDIS_REPLY_ARRAY) {
		ret = (int) reply->elements/2;
		for (j = 0; j < reply->elements; j+=2) {
			item[ reply->element[j+1]->str ] = reply->element[j]->str;
		}
	}
	freeReplyObject(reply);
	return ret;
}

int redis_vector( vector <string> &items, const char * format, ... )
{
	int ret = -1;
	size_t j;

	va_list ap;
	va_start(ap,format);
	redisReply * reply = (redisReply *) redisvCommand( redis, format, ap );
	va_end(ap);

	if (reply->type == REDIS_REPLY_ARRAY) {
		ret = (int) reply->elements;
		for (j = 0; j < reply->elements; j++) {
			items.push_back( reply->element[j]->str );
		}
	}
	freeReplyObject(reply);
	return ret;
}

void variable_set(string name, string value)
{
	conf[name] = serialize_string( value );
	
	if( DB_TYPE == 1 )
	{
		redis_command("HSET variable %s %s", name.c_str(), conf[name].c_str() );
	}
	else
	{	
		db_querya( "DELETE FROM variable WHERE name = '%s'", name.c_str() );
		db_querya( "INSERT INTO variable (name, value) VALUES ('%s', '%s')", name.c_str(), conf[name].c_str() );
	}
}

void variable_get(string name, map <string, string> &result)
{
	return unserialize_array( conf[name], result );
}

void variable_get(string name, vector <pair <string, string> > &result)
{
	return unserialize_array( conf[name], result );
}

string variable_get(string name, string def)
{
	return isset(conf[name]) ? unserialize_string( conf[name] ) : def;
}

bool variables_init()
{
	map <string, string> item;
	MYSQL_RES *result;
		
	if( DB_TYPE == 1 )
	{
		if( redis_multi(conf, "HGETALL variable") )
			return false;
	}
	if( DB_TYPE == 2 )
	{
		disable_errors = true;
		result = db_querya("SELECT * FROM variable");
		disable_errors = false;
		if( result ) 
		{
			while( db_fetch( result, item ) )
			{
				conf[ item["name"] ] = item["value"];
			}
			return false;
		}
	}
	return true;	
}

void build_controllers()
{	
	// Find the _controllers Hooks
	for( map <string, void (*)(void)>::iterator curr = _HOOKS.begin(), end = _HOOKS.end();  curr != end;  curr++ )
	{	
		size_t	cut;
		if( (cut = curr->first.find( "_controllers" )) != string::npos && (cut == curr->first.size() - 12) )
		{
			curr->second();
		}
	}	
}

void build_node_types()
{
	map <string, void (*)(void)>::iterator curr, end;
	size_t	pos;

	// Find the _node_info Hooks
	for( curr = _HOOKS.begin(), end = _HOOKS.end();  curr != end;  curr++ )
	{	
		if( (pos = curr->first.find( "_node_info" )) != string::npos && (pos == curr->first.size() - 10) )
		{
			curr->second();
		}
	}
}

string theme_status_messages()
{
	string output;
	int i;

	if(_ERROR_MESSAGES.size()>0)
	{
		output += "<div class=\"messages error\">\n";
		if(_ERROR_MESSAGES.size()>1) {
			output += " <ul>\n";
			for( i = 0; i < (int) _ERROR_MESSAGES.size(); i++ )	{
				output += "  <li>" + _ERROR_MESSAGES[i] + "</li>\n";
			}
			output += " </ul>\n";
		}
		else
		{
			output += _ERROR_MESSAGES[0];
		}
		output += " </div>\n";
	}

	if(_STATUS_MESSAGES.size()>0)
	{
		output += "<div class=\"messages status\">\n";
		if(_STATUS_MESSAGES.size()>1) {
			output += " <ul>\n";
			for( i = 0; i < (int) _STATUS_MESSAGES.size(); i++ )
			{
				output += "  <li>" + _STATUS_MESSAGES[i] + "</li>\n";
			}
			output += " </ul>\n";
		}
		else output += _STATUS_MESSAGES[0];
		output += " </div>\n";
	}

	return output;		 
}

string hook_help()
{
	string		out;
	map <string, void (*)(void)>::iterator curr3, end3;

	out = help;

	for( curr3 = _HOOKS.begin(), end3 = _HOOKS.end();  curr3 != end3;  curr3++ )
	{
		if( curr3->first.find( "_help" ) == curr3->first.size() - 5 )
		{
			curr3->second(); // Call help function
			out = help;
		}
	}

	return out;
}

string build_tabs(int cur)
{
	list < pair<int, string> > order;
	string out;

	int cur_parent = isset(controllers[cur]["parent tab"]) ? intval( controllers[cur]["parent tab"] ) : intval( controllers[cur]["parent"] );

	//print( "*"+str(cur_parent)+"*" );

	//explain( controllers );

	if( cur_parent >= 0 ) // Is not the root mid
	{
		for(size_t mid=0; mid< controllers.size(); mid++)
		{		
			int parent = isset( controllers[mid]["parent tab"] ) ? intval( controllers[mid]["parent tab"] ) : intval( controllers[mid]["parent"] );

			if( parent == cur_parent)
			{
				//print( "*"+str(parent)+"-"+str(cur_parent)+"*<br>\n" );

				if( (intval( controllers[mid]["type"] ) && MENU_IS_LOCAL_TASK) )
				{
					bool has_access = true;
					if( !controllers[mid]["access callback"].empty() )
					{
						has_access = _ACCESS_CALLBACKS[ controllers[mid]["access callback"] ]( controllers[mid]["access arguments"] );
					}
					else if( !controllers[mid]["access arguments"].empty() )
					{
						has_access = user_access( controllers[mid]["access arguments"] );
					}

					if( has_access ) 
					{
						int pos;

						if(!controllers[mid]["weight"].empty())
							pos = intval( controllers[mid]["weight"] );
						else
							pos = 0;

						string active = "";
						string path = replace_wildcards( controllers[mid]["path"] );
						if( path == _GET["q"] ) active = " class=\"active\"";

						order.push_back( make_pair( pos, "<li"+active+"><a href=\""+url( path )+"\">"+controllers[mid]["title"]+"</a></li>\n" ) );
					}
				}
			}
		}

		if( order.size() > 1 )
		{
			order.sort();
			out = "<ul class=\"tabs primary\">\n";
			for( list < pair<int, string> >::iterator i = order.begin(), end = order.end();  i != end;  i++ )
			{
				out += i->second;
			}
			out += "</ul>\n";
		}
	}
	
	return out;
}

string build_breadcrumb(int mid)
{
	string out;

	mid = intval( controllers[mid]["parent"] );
	while( mid >= 0)
	{
		if( (intval( controllers[mid]["type"] ) && MENU_VISIBLE_IN_BREADCRUMB) )
		{
			string path = controllers[mid]["path"];
			out = "<a href=\""+ url(controllers[mid]["path"]) +"\">" + controllers[mid]["title"] + "</a> » " + out;
		}
		mid = intval( controllers[mid]["parent"] );
	}
	
	out = "<a href=\""+ url("") +"\">Home</a> » " + out;

	return "<div class=\"breadcrumb\">" + out.substr(0, out.length()-3) + "</div>";
}

void process_controller()
{
	map <string, string>::iterator curr, end;
	unsigned int i;
	int found = -1;
	int found_exact = -1;
	bool has_access;
	bool has_theme = false;
	map <string, string> item;

	validate_form = false;
	load_form_only = false;

	if( _GET["q"].length() == 0)
	{
		// Page not found
		item["content"] = "Home Page.";
		item["is_front"] = _TRUE;
		set_page_title( "Home" );
	}
	else
	{
		if( model_exists("path") )
		{
			string src = db_result(db_querya("SELECT src FROM url_alias WHERE dst='%s' AND language='%s'", _GET["q"].c_str(), language["language"].c_str() ));
			if( isset(src) ) _GET["q"] = src;
		}

		for( i = 0; i < controllers.size(); i++ )
		{
			string q = _GET["q"];
			trim(q, "/"); // Fix for directory based urls
			if( controllers[i]["path"].find_first_of('%') != controllers[i]["path"].npos )
			{
				if( match_pattern( _GET["q"] , controllers[i]["path"] ) )
				{
					found = i;			
				}
			}
			else if( q == controllers[i]["path"] )
			{
				found = i;
				found_exact = i;
			}
		}
		if( found_exact >=0 ) found = found_exact;

		if( found >=0 )
		{
			has_access = true;
			if( !controllers[found]["access callback"].empty() )
			{
				has_access = _ACCESS_CALLBACKS[ controllers[found]["access callback"] ]( controllers[found]["access arguments"] );
			}
			else if( !controllers[found]["access arguments"].empty() )
			{
				has_access = user_access( controllers[found]["access arguments"] );
			}

			if( has_access )
			{
				set_page_title( controllers[found]["title"] );

				_ACTIVE_CONTROLLER = found;
				if(getenv("CONTENT_LENGTH")!=NULL)
				{
					if(_SUBMITS[ controllers[found]["callback"] ])
					{
						load_form_only = true;
						_CALLBACKS[ controllers[found]["callback"] ]();
						load_form_only = false;

						if( form_validate( cur_form ) )
						{
							if(_VALIDATES[ controllers[found]["callback"] ])
							{								
								if( _VALIDATES[ controllers[found]["callback"] ]() )
								{
									item["content"] = _SUBMITS[ controllers[found]["callback"] ]();
								}
							}
						}
					}
				}
				
				if( !isset( item["content"] ) )
					item["content"] = _CALLBACKS[ controllers[found]["callback"] ]();

				item["messages"] = theme_status_messages();
			}
			else
			{
				// Don't have permission
				set_page_title( "Access denied" );
				item["content"] = "You are not authorized to access this page.";
				item["content"] = theme( "denied", item );
			}

			item["breadcrumb"] = build_breadcrumb( found );
			item["tabs"] = build_tabs( found );
		}
		else
		{
			// Page not found
			set_page_title( "Page not found" );
			item["content"] = "The requested page could not be found.";
			item["content"] = theme( "error", item );
		}
	}
		
	item["sidebar-left"] = "<div class=\"block block-user\" id=\"block-user-1\">\n<h2 class=\"title\">"+user["name"]+"</h2><div class=\"content\">" + theme_menu_tree() + "</div></div>";
	
	if( item["is_front"] == _TRUE )
	{
		item["head_title"] = variable_get("site_name", "BinaryTiers");
		if( isset( variable_get("site_slogan", "") ) ) item["head_title"] += " | " + variable_get("site_slogan", "");
	}
	else
	{		
		item["head_title"] = get_page_title() + " | " + variable_get("site_name", "BinaryTiers");
	}
	
	item["title"] = get_page_title();
	get_page_extra( item );
	item["help"] = hook_help();
			
	if( !item["content"].empty() )
	{
		has_theme = false;
		string func_name;

		if( isset( user["theme"] ) )
			func_name = user["theme"]+"_page_theme";
		else
			func_name = variable_get("theme_default", "admin")+"_page_theme";
		
		if( _THEME_HOOKS[func_name] )
		{
			print( _THEME_HOOKS[func_name]( item ) );
			has_theme = true;
		}
		else
		{
			page_theme( item );
		}
	}
}

string theme( string hook, map <string, string> &item )
{
	string func_name;

	if( isset( user["theme"] ) )
		func_name = user["theme"]+"_"+hook+"_theme";
	else
		func_name = variable_get("theme_default", "admin")+"_"+hook+"_theme";
	
	if( _THEME_HOOKS[func_name] )
	{
		return _THEME_HOOKS[func_name]( item );
	}
	else
	{
		func_name = hook+"_theme";
		if( _THEME_HOOKS[func_name] )
		{
			return _THEME_HOOKS[func_name]( item );
		}
	}

	return item["content"];
}

string session_id()
{
	map <string, string>::iterator curr, end;

	for( curr = _COOKIE.begin(), end = _COOKIE.end();  curr != end;  curr++ )
	{
		if( curr->first.find( "SESS" ) != curr->first.npos )
		{
			return curr->second;
		}
	}

	return "";
}

void sess_read( string key )
{
	MYSQL_RES *result;

	if(key.empty())
	{
		if( DB_TYPE == 2 )
		{
			result = db_querya("SELECT u.* FROM users u WHERE u.uid = 0");
			db_fetch( result, user );
		}
		user["language"] = "";
	}
	else
	{
		if( DB_TYPE == 1 )
		{
			int uid = redis_int("HGET sessions:%s uid", key.c_str() );
			user["uid"] = str( uid );
			redis_multi(user, "HGETALL users:%d", uid );
			//Update Access Time
			redis_command("HSET users:%d access %d", uid, time() );
		}

		if( DB_TYPE == 2 )
		{
			// retrieve data for a "user" object
			result = db_querya("SELECT sid FROM sessions WHERE sid = '%s'", key.c_str());

			if (!db_num_rows( result )) {
				result = db_querya("SELECT u.* FROM users u WHERE u.uid = 0");
				db_fetch( result, user );
			}
			else {
				result = db_querya("SELECT u.*, s.* FROM users u INNER JOIN sessions s ON u.uid = s.uid WHERE s.sid = '%s'", key.c_str());
				db_fetch( result, user );

				db_querya("UPDATE users SET access=UNIX_TIMESTAMP() WHERE uid=%d AND UNIX_TIMESTAMP()-access>60", user["uid"].c_str() );
			}
		}
	}

	map <string, string> role;

	if( user["uid"]=="0" ) 
		user["roles"] = ANONYMOUS_RID;
	else 
		user["roles"] = AUTHENTICATED_RID;

	if( DB_TYPE == 1 )
	{
		string role;
		REDIS_RES *rr = redis_query( "LRANGE users:%d:roles 0 -1", intval(user["uid"]) );
		while( redis_fetch( rr, role ) ) {
			user["roles"] += "," + role;
	    }
	}

	if( DB_TYPE == 2 )
	{
		result = db_querya("SELECT * FROM users_roles WHERE uid = %d", user["uid"].c_str());
		while ( db_fetch( result, role ) ) {
			user["roles"] += "," + role["rid"];
	    }
	}
}

/**
 * The model_exists() function test if a model is enabled by the administrator.
 * @param name Name of the model we want to test.
 * @returns true if the model is enabled, otherwise the returned value is false.
 * @ingroup models
 */
bool model_exists(string name)
{
	if( _PLUGINS_STRINGS[ "model_"+name ]["on"] == _TRUE ) 
		return true;
	else
		return false;
}

/**
 * Checks whether a string is valid UTF-8.
 */
bool valid_utf8( string text ) 
{
	if (text.length() == 0) 
		return true;
	
	return preg_match((char *) "/^./us", (char *) text.c_str());
}

string check_plain( string text ) 
{
	return htmlencode( text );
}

string check_url( string url )
{
	return url;
}

bool valid_email_address(string mail)
{
	mail = strtolower( mail );
	//char * pattern = "[a-z0-9!#$%&'*+/=?^_`{|}~-]+(?:\\.[a-z0-9!#$%&'*+/=?^_`{|}~-]+)*@(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\\.)+(?:[a-z]{2}|com|org|net|gov|mil|biz|cat|info|mobi|name|aero|jobs|museum)\\b";
	//char * pattern = "^[-!#$%&\'*+/0-9=?A-Z^_a-z{|}~](\\.?[-!#$%&\'*+/0-9=?A-Z^_a-z{|}~])*@[a-zA-Z](-?[a-zA-Z0-9])*(\\.[a-zA-Z](-?[a-zA-Z0-9])*)+$";
	const char pattern[] = "^(?!(?:(?:\\x22?\\x5C[\\x00-\\x7E]\\x22?)|(?:\\x22?[^\\x5C\\x22]\\x22?)){255,})(?!(?:(?:\\x22?\\x5C[\\x00-\\x7E]\\x22?)|(?:\\x22?[^\\x5C\\x22]\\x22?)){65,}@)(?:(?:[\\x21\\x23-\\x27\\x2A\\x2B\\x2D\\x2F-\\x39\\x3D\\x3F\\x5E-\\x7E]+)|(?:\\x22(?:[\\x01-\\x08\\x0B\\x0C\\x0E-\\x1F\\x21\\x23-\\x5B\\x5D-\\x7F]|(?:\\x5C[\\x00-\\x7F]))*\\x22))(?:\\.(?:(?:[\\x21\\x23-\\x27\\x2A\\x2B\\x2D\\x2F-\\x39\\x3D\\x3F\\x5E-\\x7E]+)|(?:\\x22(?:[\\x01-\\x08\\x0B\\x0C\\x0E-\\x1F\\x21\\x23-\\x5B\\x5D-\\x7F]|(?:\\x5C[\\x00-\\x7F]))*\\x22)))*@(?:(?:(?!.*[^.]{64,})(?:(?:(?:xn--)?[a-z0-9]+(?:-[a-z0-9]+)*\\.){1,126}){1,}(?:(?:[a-z][a-z0-9]*)|(?:(?:xn--)[a-z0-9]+))(?:-[a-z0-9]+)*)|(?:\\[(?:(?:IPv6:(?:(?:[a-f0-9]{1,4}(?::[a-f0-9]{1,4}){7})|(?:(?!(?:.*[a-f0-9][:\\]]){7,})(?:[a-f0-9]{1,4}(?::[a-f0-9]{1,4}){0,5})?::(?:[a-f0-9]{1,4}(?::[a-f0-9]{1,4}){0,5})?)))|(?:(?:IPv6:(?:(?:[a-f0-9]{1,4}(?::[a-f0-9]{1,4}){5}:)|(?:(?!(?:.*[a-f0-9]:){5,})(?:[a-f0-9]{1,4}(?::[a-f0-9]{1,4}){0,3})?::(?:[a-f0-9]{1,4}(?::[a-f0-9]{1,4}){0,3}:)?)))?(?:(?:25[0-5])|(?:2[0-4][0-9])|(?:1[0-9]{2})|(?:[1-9]?[0-9]))(?:\\.(?:(?:25[0-5])|(?:2[0-4][0-9])|(?:1[0-9]{2})|(?:[1-9]?[0-9]))){3}))\\]))$";
	
	return preg_match( (char*) pattern, (char *) mail.c_str() );
}

bool ereg(char *pattern, char *string)
{	
	int        rc;
    size_t     nmatch = 0;
	    
    if ((rc = regcomp(&preg, pattern, REG_EXTENDED)) != 0) {
		//printf("regcomp() failed, returning nonzero (%d)\n", rc);
		return false;
    }
    if ((rc = regexec(&preg, string, nmatch, NULL, 0)) != 0) {
		//printf("failed to ERE match '%s' with '%s',returning %d.\n", string, pattern, rc);
		return false;
	}
	regfree( &preg );
	
	return true;
}

#define OVECCOUNT 30    /* should be a multiple of 3 */

bool preg_match(char *pattern, char *subject, void *matches)
{
	pcre *re;
	const char *error;
	int erroffset;
	int ovector[OVECCOUNT];
	int subject_length;
	int rc, i;
	
	subject_length = (int)strlen(subject);

	vector <string> *match = NULL;
	
	re = pcre_compile( pattern, 0, &error, &erroffset, NULL);
	if (re == NULL)	{
		return false;
	}

	rc = pcre_exec( re, NULL, subject, subject_length, 0, 0, ovector, OVECCOUNT);
	if (rc < 0)	{
		switch(rc)
		{
			case PCRE_ERROR_NOMATCH: 
				//print("No match\n");
				break;
			default: 
				//print("Match error "+str(rc)+"\n"); 
				break;
		}
		pcre_free(re);		
		return false;
	}

	/* If we got this far the match succeded */
	if (rc == 0) {
		rc = OVECCOUNT/3;
	}

	if( matches ) {
		match = (vector <string> *) matches;
	
		for (i = 0; i < rc; i++) {
			char *substring_start = subject + ovector[2*i];
			match->push_back( substring_start );
		}
	}

	pcre_free(re);
	return true;
}

bool preg_match_all(char *pattern, char *subject, void *matches, int flag)
{
	pcre *re;
	const char *error;
	unsigned char *name_table;
	int erroffset;
	int namecount;
	int name_entry_size;
	int ovector[OVECCOUNT];
	int subject_length;
	int rc, i;
	int find_all = 1;
	
	subject_length = (int)strlen(subject);

	vector <string> *match = NULL;
	vector < pair <int, string> > *match2 = NULL;
	
	re = pcre_compile( pattern, 0, &error, &erroffset, NULL);
	if (re == NULL)	{
		return false;
	}

	rc = pcre_exec( re, NULL, subject, subject_length, 0, 0, ovector, OVECCOUNT);
	if (rc < 0)	{
		switch(rc)
		{
			case PCRE_ERROR_NOMATCH: 
				break;
			default: 
				break;
		}
		pcre_free(re);		
		return false;
	}

	if (rc == 0) {
		rc = OVECCOUNT/3;
	}
	
	if( matches )
	{
		if( flag & PREG_OFFSET_CAPTURE )
			match2 = (vector < pair <int, string> > *) matches;
		else
			match = (vector <string> *) matches;
	
		for (i = 0; i < rc; i++)
		{
			char *substring_start = subject + ovector[2*i];
			int substring_length = ovector[2*i+1] - ovector[2*i];
	
			char * dest = (char *) malloc( substring_length + 1 );
			strncpy( dest, substring_start, substring_length );
			dest[substring_length] = (char) NULL;
			
			if( flag & PREG_OFFSET_CAPTURE )
				match2->push_back( make_pair(ovector[2*i], dest) );
			else
				match->push_back( dest );
			free( dest );
		}
	}
	
	pcre_fullinfo( re, NULL, PCRE_INFO_NAMECOUNT, &namecount);
	if (namecount > 0) 
	{  
		unsigned char *tabptr;
		pcre_fullinfo( re, NULL, PCRE_INFO_NAMETABLE, &name_table);
		pcre_fullinfo( re, NULL, PCRE_INFO_NAMEENTRYSIZE, &name_entry_size);
		tabptr = name_table;
		for (i = 0; i < namecount; i++) {
			tabptr += name_entry_size;
		}
	}
	
	if (!find_all) {
		pcre_free(re);
		return 0;
	}

	for (;;) 
	{
		int options = 0;
		int start_offset = ovector[1];
		if (ovector[0] == ovector[1])
		{
			if (ovector[0] == subject_length) break;
			options = PCRE_NOTEMPTY | PCRE_ANCHORED;
		}

		rc = pcre_exec( re, NULL, subject, subject_length, start_offset, options, ovector, OVECCOUNT);
		if (rc == PCRE_ERROR_NOMATCH) 
		{
			if (options == 0) break;
			ovector[1] = start_offset + 1;
			continue; /* Go round the loop again */
		}

		if (rc < 0) {
			pcre_free(re);
			return true;
		}
		if (rc == 0) {
			rc = OVECCOUNT/3;
		}

		for (i = 0; i < rc; i++)
		{
			char *substring_start = subject + ovector[2*i];
			int substring_length = ovector[2*i+1] - ovector[2*i];

			char * dest = (char *) malloc( substring_length + 1 );
			strncpy( dest, substring_start, substring_length );
			dest[substring_length] = (char) NULL;
			
			if( flag & PREG_OFFSET_CAPTURE )
				match2->push_back( make_pair(ovector[2*i], dest) );
			else
				match->push_back( dest );

			free( dest );
		}

		if (namecount > 0) 
		{
			unsigned char *tabptr = name_table;
			for (i = 0; i < namecount; i++) {		  
				tabptr += name_entry_size;
			}
		}
	}

	pcre_free(re);
	return 0;
}

char lookup_base64[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

void Base64_encode(const char *pSrc, int nSrcLen, char *pszOutBuf) 
{
	while (nSrcLen >= 3) {
		*pszOutBuf++ = lookup_base64[ (*pSrc >> 2) & 0x3F];
		*pszOutBuf++ = lookup_base64[((*pSrc << 4) & 0x30) | (((*(pSrc + 1)) >> 4) & 0x0F)];
		*pSrc++;
		*pszOutBuf++ = lookup_base64[((*pSrc << 2) & 0x3C) | (((*(pSrc + 1)) >> 6) & 0x03)];
		*pSrc++;
		*pszOutBuf++ = lookup_base64[ (*pSrc     ) & 0x3F];
		*pSrc++;
		nSrcLen -= 3;
	}
	if (nSrcLen) 
	{
		*pszOutBuf++ = lookup_base64[(*pSrc >> 2) & 0x3F];
		if (--nSrcLen) {
			*pszOutBuf++ = lookup_base64[((*pSrc << 4) & 0x30) | (((*(pSrc + 1)) >> 4) & 0x0F)];
			*pSrc++;
			if (--nSrcLen) {
				*pszOutBuf++ = lookup_base64[((*pSrc << 2) & 0x3C) | (((*(pSrc + 1)) >> 6) & 0x03)];
				*pSrc++;
				*pszOutBuf++ = lookup_base64[(*pSrc) & 0x3F];
			}
			else {
				*pszOutBuf++ = lookup_base64[(*pSrc << 2) & 0x3C];
				*pszOutBuf++ = '=';
			}
		}
		else {
			*pszOutBuf++ = lookup_base64[(*pSrc << 4) & 0x30];
			*pszOutBuf++ = '=';
			*pszOutBuf++ = '=';
		}
	}
	*pszOutBuf = (char) NULL;
	return;  
}

int Base64_decode(const char *src, int len, char *dst) 
{
	char lookup_inv[256];
	int dst_size = (len / 4) * 3;

	memset(lookup_inv, 0, 256);
	for (int i = 0; i < 64; ++i) {
		lookup_inv[(unsigned char) lookup_base64[i]] = i;
	}

	len /= 4;
	while (len--) {
		unsigned int value = lookup_inv[(unsigned char) *src++]; value <<= 6;
		value |= lookup_inv[(unsigned char) *src++]; value <<= 6;
		value |= lookup_inv[(unsigned char) *src++]; value <<= 6;
		value |= lookup_inv[(unsigned char) *src++];

		dst[2] = (unsigned char) value; value >>= 8;
		dst[1] = (unsigned char) value; value >>= 8;
		dst[0] = (unsigned char) value;
		dst += 3;
	}

	if (*(--src) == '=') --dst_size;
	if (*(--src) == '=') --dst_size;
	*dst = (char) NULL;

	return dst_size;
}

string base64_encode( string source )
{
	char * dest = (char *) malloc( (source.length() / 3) * 4 + 5 );

	Base64_encode( source.c_str(), (int) source.length(), dest );
	string	result = string( dest );
	free( dest );

	return result;
}

string base64_decode( string source )
{
	char * dest = (char *) malloc( (source.length() / 4) * 3 + 5 );

	Base64_decode( source.c_str(), (int) source.length(), dest );
	string	result = string( dest );
	free( dest );

	return result;
}

int fsockopen(const char * host, int port)
{
	int socket_fd;
	struct sockaddr_in name;
	struct hostent* hostinfo;
	/* Create the socket.  */
	socket_fd = socket (PF_INET, SOCK_STREAM, 0);
	/* Store the server’s name in the socket address.  */
	name.sin_family = AF_INET;
	/* Convert from strings to numbers.  */
	hostinfo = gethostbyname ( host );
	if (hostinfo == NULL)
		return 0;
	else
		name.sin_addr = *((struct in_addr *) hostinfo->h_addr);
	/* Web servers use port 80.  */
	name.sin_port = htons ( port );
	memset(&(name.sin_zero), '\0', 8);

	/* Connect to the Web server  */
	if (connect (socket_fd, (const sockaddr *) &name, sizeof (struct sockaddr_in)) == -1) 
	{
		print ("BT-ERROR: connect");
		return 0;
	}

	return socket_fd; 
}

int parse_url(string uri, map <string, string> &result)
{
	size_t pos, pos_path;
	
	pos = uri.find_first_of(':');
	if( pos != string::npos ) // has scheme
	{	
		result["scheme"] = uri.substr(0, pos);
		uri = uri.substr( pos + 1);
		pos = uri.find_first_not_of('/');
		if( pos != string::npos )
		{
			uri = uri.substr( pos );
			pos_path = uri.find_first_of('/');
			if( pos_path == string::npos ) pos_path = uri.length();

			pos = uri.find_first_of('@');
			if( pos != string::npos && pos < pos_path ) // has user
			{
				result["user_pass"] = uri.substr(0, pos);
				uri = uri.substr( pos + 1);
			}
			pos = uri.find_first_of(':');
			if( pos != string::npos && pos < pos_path) // has port
			{
				result["host"] = uri.substr(0, pos);
				uri = uri.substr( pos + 1);
				pos = uri.find_first_of('/');
				if( pos == string::npos ) pos = uri.length();

				result["port"] = uri.substr(0, pos);
			}
			else
			{
				pos = uri.find_first_of('/');
				if( pos == string::npos ) pos = uri.length();

				result["host"] = uri.substr(0, pos);
			}

			uri = uri.substr(pos);
			pos = uri.find_first_of('?');
			if( pos != string::npos ) // has query
			{
				result["path"] = uri.substr(0, pos);
				uri = uri.substr( pos + 1);
				pos = uri.find_first_of('#');
				if( pos != string::npos ) // has fragment
				{
					result["query"] = uri.substr(0, pos);
					result["fragment"] = uri.substr(pos+1);
				}
				else
				{
					result["query"] = uri;
				}
			}
			else
			{
				result["path"] = uri;
			}
		}
	}

	return 1;
}

// Establish a connection using an SSL layer
SSLConnection *sslConnect (const char * host, int port)
{
	SSLConnection *c;

	c = (SSLConnection *) malloc (sizeof (SSLConnection));
	c->sslHandle = NULL;
	c->sslContext = NULL;

	c->socket = fsockopen ( host, port );

	if (c->socket)
	{
		// Register the error strings for libcrypto & libssl
		SSL_load_error_strings ();
		// Register the available ciphers and digests
		SSL_library_init ();

		// New context saying we are a client, and using SSL 2 or 3
		c->sslContext = SSL_CTX_new (SSLv23_client_method ());
		if (c->sslContext == NULL)
		ERR_print_errors_fp (stderr);

		// Create an SSL struct for the connection
		c->sslHandle = SSL_new (c->sslContext);
		if (c->sslHandle == NULL)
		ERR_print_errors_fp (stderr);

		// Connect the SSL struct to our connection
		if (!SSL_set_fd (c->sslHandle, c->socket))
		ERR_print_errors_fp (stderr);

		// Initiate SSL handshake
		if (SSL_connect (c->sslHandle) != 1)
		ERR_print_errors_fp (stderr);
	}
	else
	{
		print ("BT-ERROR: Connect failed");
	}

	return c;
}

// Disconnect & free connection struct
void sslDisconnect (SSLConnection *c)
{
	if (c->socket)
		close (c->socket);

	if (c->sslHandle)
	{
		SSL_shutdown (c->sslHandle);
		SSL_free (c->sslHandle);
	}
	if (c->sslContext)
		SSL_CTX_free (c->sslContext);

	free (c);
}

// Read all available text from the connection
string sslRead (SSLConnection *c)
{
	string result;
	const int readSize = 2048;
	int received, count = 0;
	char *buffer = (char *) malloc( readSize + 1);

	if (c)
	{
		while (1)
		{
			received = SSL_read(c->sslHandle, buffer, readSize);
			buffer[received] = '\0';

			if (received > 0)
			{
				result += string( buffer );
			}

			if (received == 0)
				break;
			count++;
		}
	}

	free( buffer );

	return result;
}

// Write text to the connection
void sslWrite (SSLConnection *c, string text)
{
	if (c)
		SSL_write (c->sslHandle, text.c_str(), (int) text.length() );
}

void http_request( map <string, string> &result, string url, map <string, string> headers, string method, string data )
{
	char	chunk[1025];
	map <string, string> uri;
	
	FILE * tmp = NULL;
	string local_path;
	string request;
	string path;
	string host;
	string port;

	string response = "";
	int i;

	parse_url(url, uri);

	int fp;
	SSLConnection *fp_ssl = NULL;

	if( uri["scheme"]=="http" )
	{	
		port = isset(uri["port"]) ? uri["port"] : "80";
		host = uri["host"] + (port != "80" ? ":" + port : "");
		fp = fsockopen( uri["host"].c_str(), intval(port) );
	}
	else if( uri["scheme"]=="https" )
	{
		port = isset(uri["port"]) ? uri["port"] : "443";
		host = uri["host"] + (port != "443" ? ":" + port : "");
		fp_ssl = sslConnect( uri["host"].c_str(), intval(port) );
	}
	else
	{
		return;
	}

	if(!fp && !fp_ssl) {
		return;
	}

	path = isset(uri["path"]) ? uri["path"] : "/";

	if (isset(uri["query"])) {
		path = path + "?" + uri["query"];
	}

	request = method + " " + path + " HTTP/1.0\r\n";
	request += "Host: " + host + "\r\n";
	request += "User-Agent: Uselabs (+http://uselabs.com/)\r\n";
	if( !headers.empty() ) 
	{		
		//request += "Content-Type: application/x-www-form-urlencoded;\r\n";
		for( map <string, string>::iterator curr = headers.begin(), end = headers.end();  curr != end;  curr++ )
		{
			if( curr->first == "$localpath" )
			{
				local_path = curr->second;
				tmp = fopen( tempfile().c_str(), "w+b" );
			}
			else
			{
				if( curr != headers.begin() ) request += "\r\n";
				request += curr->first+": "+curr->second;
			}
		}
		request += "\r\n";
	}
	if( data.length()>0 ) request += "Content-Length: " + str(data.length())+ "\r\n";
	if( !uri["user_pass"].empty() )
	{
	    request += "Authorization: Basic " + base64_encode( uri["user_pass"] ) + "\r\n";
	}
	request += "\r\n";
	request += data;
	
	if( fp_ssl )
	{
		sslWrite (fp_ssl, request );
		response = sslRead (fp_ssl);
		sslDisconnect (fp_ssl);
	}
	else
	{
		send( fp, request.c_str(), (int) request.length(), 0 ); 
	
		while ( (i = recv( fp, chunk, 1024, 0 )) != 0 ) {
			if( tmp ) fwrite(chunk, 1, i, tmp);
			chunk[i] = (char) NULL;
			response += string( chunk );
		}
		chunk[i] = (char) NULL;
		response += string( chunk );
		close( fp );
	}
	
	string split;

	size_t pos = response.find("\r\n\r\n");

	if( pos != string::npos ) 
	{
		split = response.substr(0, pos);
		result["data"] = response.substr(pos+4);

		pos = split.find_first_of(' ');
		result["protocol"] = split.substr(0, pos);
		split = split.substr(pos+1);

		pos = split.find_first_of(' ');
		result["response_code"] = split.substr(0, pos);
		split = split.substr(pos+1);

		pos = split.find_first_of("\r\n");
		result["response_message"] = split.substr(0, pos);
		split = split.substr(pos);
		pos = split.find_first_not_of("\r\n");
		split = split.substr(pos);
		
		vector <string> pieces;
		explode(pieces, split, "\r\n");

		for(i=0;i<(int) pieces.size();i++)
		{
			string key, value;

			pos = pieces[i].find_first_of(':');
			if( pos != string::npos )
			{
				key = pieces[i].substr(0, pos);
				value = pieces[i].substr(pos+1);
				ltrim( value );
				result[ key ] = value;
			}
		}
	}

	if( tmp )
	{
		int len = intval( result["Content-Length"] );
		int size = ftell( tmp );
		fseek( tmp, size-len, SEEK_SET );

		FILE *out = fopen( local_path.c_str(), "wb" );
		int i = 0;
		while( i< len )
		{
			int c = fgetc( tmp );
			fputc( c, out );
			i++;
		}
		fclose( out );
		fclose( tmp );
	}

	switch ( intval(result["response_code"]) )
	{
		case 200: // OK
		case 304: // Not modified
			break;
		case 301: // Moved permanently
		case 302: // Moved temporarily
		case 307: // Moved temporarily
			http_request(result, result["Location"], headers, method, data);
			break;
	}

	return;
}

string get_browser_language()
{
	string lang = "en";
	if( getenv("HTTP_ACCEPT_LANGUAGE") )
	{
		lang = strtolower( getenv("HTTP_ACCEPT_LANGUAGE") );
		lang = lang.substr(0, 2);
		if( lang == "ca" ) lang = "es";
	}
	return lang;
}

/**
 * The bt_begin() function initializes BinaryTiers Core by stablishing a connection to the MySQL database and by filling _COOKIE, _GET and _POST arrays.
 * @param argc The integer that contains the count of arguments that follow in argv.
 * @param argv The array of null-terminated strings representing command-line parameters sent by main function.
 * @returns None
 * @ingroup group12
 */
void bt_begin(int argc, char *argv[])
{
	sent_location = 0;
	main_output = "";
	help = "";
	_GET.clear();
	_POST.clear();
	_COOKIE.clear();
	_MODELS.clear();
	_THEMES.clear();
	_CALLBACKS.clear();
	_SUBMITS.clear();
	_VALIDATES.clear();
	_ACCESS_CALLBACKS.clear();
	_PLUGINS_STRINGS.clear();
	_HOOKS.clear();
	_THEME_HOOKS.clear();
	schema.clear();	
	controllers.clear();
	_ELEMENTS.clear();
	_FILES.clear();
	_TEMP_FILES.clear();
	_ERROR_MESSAGES.clear();
	_STATUS_MESSAGES.clear();
	_TEMP_FILES_COUNT = 0;
	_ACTIVE_CONTROLLER = -1;
	_CONTENT_LENGTH = 0;
	_CONTENT_TYPE = "";
	htmlout = false;
	load_form_only = false;
	validate_form = false;

	if( !BASE_URL )
	{
		strcpy(get_host_data,"http://");
		strcat(get_host_data,_SERVER("HTTP_HOST").c_str());
		BASE_URL = get_host_data;
	}

	if(argc>1)
	{
		_GET["q"] = string( argv[1] );
	}

	if( DB_TYPE == 1 )
	{
		struct timeval timeout = { 1, 500000 }; // 1.5 seconds
		redis = redisConnectWithTimeout((char*) DB_HOST, 6379, timeout);
		if (redis->err) {
			print ("BT-ERROR 502: Redis Connection error: "+ string(redis->errstr) );
			quit(0);
		}
		redis_command("SELECT %d", DB_INDEX);
	}
	if( DB_TYPE == 2 )
	{
		if ( mysql_init(&mysql) == NULL)
		{
			print ("BT-ERROR 501: Can't initialize MySQL library.");
			quit(0);
		}

		if ( mysql_real_connect(&mysql, DB_HOST, DB_USER, DB_PASSW, DB_NAME, 0, NULL, 0) == NULL )
		{
			print ("BT-ERROR 502: MySQL connection error.");
			quit(0);
		}

		//mysql_set_character_set(&mysql, "utf8");
		
		mysql_query(&mysql, "SET NAMES 'utf8'");
	}

	timer_t1 = clock();

	bool need_install = variables_init();
	
	globals_init();

	theme_init_default();

	if( arg(0) == "install" ) 
	{
		user["uid"] = "0";
		perm[0][""] = false;
	}
	else
	{
		sess_read( session_id() );
	}

	if( need_install )
	{
		if( arg(0) != "install" )
		{
			redirect( url("install") );
			quit(0);
		}
	}

	// debug 
	/*
	if( argc > 1 )
	{
		user["uid"] = "1";
		user["theme"] = "admin";
	}*/

	vector <string> enabled_models;
	vector <string> enabled_themes;

	explode(enabled_models, variable_get("bt_models","system,node,user,"), ',');
	explode(enabled_themes, variable_get("bt_themes","admin,"), ',');
	
	language["language"] = isset(user["language"]) ? user["language"] : ( isset( _COOKIE["BTLANG"] ) ? _COOKIE["BTLANG"] : get_browser_language() );

	for(int i=0; i<(int) enabled_models.size(); i++)
	{
		_PLUGINS_STRINGS["model_"+enabled_models[i]]["on"] = "1";
	}

	for(int i=0; i<(int) enabled_themes.size(); i++)
	{
		_PLUGINS_STRINGS["theme_"+enabled_themes[i]]["on"] = "1";
	}
}

/**
 * The bt_end() function processes the and display the requested page.
 * @returns None
 * @ingroup group12
 */
void bt_end()
{
	build_node_types();
	build_controllers();
	build_menu();
	process_controller();

	if( DB_TYPE == 1 )
	{
		redisFree(redis);
	}
	if( DB_TYPE == 2 )
	{
		mysql_close(&mysql);
	}
	quit();
}

string strip_tags( string text )
{
	string	s;
	vector < string >	tags;

	tags = preg_split( (char *) "(<.*?>)", text, -1, PREG_SPLIT_NO_EMPTY);

	for( size_t i=0; i< tags.size(); i++)
	{
		if( tags[i].find_first_of("<>") == string::npos )
		{
			s += tags[i];
		}
	}
	return s;
}

vector < string > preg_split(char *pattern, string text, int limit, int flags )
{
	vector < string > parts;
	vector < pair <int, string> >	matches;

	preg_match_all(pattern, (char *) text.c_str(), &matches, PREG_OFFSET_CAPTURE);

	size_t ini = 0, end = 0;
	for( size_t i=0; i< matches.size(); i++ )
	{
		end = matches[i].first;
		if( flags & PREG_SPLIT_NO_EMPTY ) {
			if( end-ini > 0 ) parts.push_back( text.substr( ini, end-ini ) );
		}
		else {
			parts.push_back( text.substr( ini, end-ini ) );
		}
		ini = end + matches[i].second.length();
	}

	end = text.length();
	if( flags & PREG_SPLIT_NO_EMPTY ) {
		if( end-ini > 0 ) parts.push_back( text.substr( ini, end-ini ) );
	}
	else {
		parts.push_back( text.substr( ini, end-ini ) );
	}

	return parts;
}

string filter_xss( string text, string allowed_tags )
{
	string	out;
	
	vector < pair <int, string> >	matches;
	vector <string> allowed;

	explode( allowed, allowed_tags, ' ' );

	preg_match_all( (char *) "(<.*?>)", (char *) text.c_str(), &matches, PREG_OFFSET_CAPTURE);

	size_t ini = 0;
	bool disable = false;

	if( matches.size() > 0 )
	{
		for( size_t i=0; i< matches.size(); i++ )
		{
			size_t pos_cur, pos_next;
			if( i < matches.size() - 1)
			{
				pos_cur = matches[i].first;
				pos_next = matches[i+1].first;
			}
			else
			{
				pos_cur = matches[i].first;
				pos_next = text.length();
			}

			if( pos_cur != pos_next )
			{
				if( matches[i].second.find( "<script" ) != string::npos ) disable = true;
				if( matches[i].second.find( "/script>" ) != string::npos ) disable = false;
				
				for( size_t j=0; j<allowed.size(); j++ )
				{
					string open = allowed[j].substr(0, allowed[j].size() - 1);
					string close = "/"+allowed[j].substr(1);
					if( matches[i].second.find( open ) != string::npos || matches[i].second.find( close ) != string::npos )
					{
						out += matches[i].second;
					}
				}
			
				if( !disable )
				{
					ini = pos_cur + matches[i].second.length();
					out += text.substr( ini, pos_next-ini );
				}
			}
		}
	}
	else
	{
		out = text;
	}

	return out;
}

string xml_start( map <string, string> &attributes, const char *wname, const char **atts)
{
	int i = 0;
#if BT_SIZEOFWCHAR == 2
	size_t size_name = 256;
	size_t len;
	char * name = (char *) malloc( size_name + 1 );
	const wchar_t ** a = (const wchar_t **) atts;
	while( a[i] )
	{
		string key, value;
		len = wcslen( (const wchar_t *) a[i] );
		if( len > size_name ) {
			size_name = len;
			free( name );
			name = (char*) malloc( size_name + 1 );
		}
		wcstombs( name, (const wchar_t *) a[i], size_name);
		strtoupper( name );
		key = string( name );
		i++;
		
		len = wcslen( (const wchar_t *) a[i] );
		if( len > size_name ) {
			size_name = len;
			free( name );
			name = (char*) malloc( size_name + 1 );
		}
		wcstombs( name, (const wchar_t *) a[i], size_name);
		value = string( name );
		i ++;
		attributes[ key ] = value;
	}
	len = wcslen( (const wchar_t *) wname );
	if( len > size_name ) {
		size_name = len;
		free( name );
		name = (char*) malloc( size_name + 1 );
	}
	wcstombs( name, (const wchar_t *) wname, size_name);
#else
	int size_name = 0;
	char * name = (char *) wname;
	while( atts[i] )
	{
		string key, value;
		strtoupper( (char *) atts[i] );
		key = string( atts[i] );
		i++;
		value = string( atts[i] );
		i ++;
		attributes[ key ] = value;
	}
#endif

	string ret = string(name);
	if( size_name > 0 ) free( name );

	return ret;
}

string xml_end(const char *wname)
{
	char name[64];

#if BT_SIZEOFWCHAR == 2
	wcstombs( name, (const wchar_t *) wname, 127);
#else
	strcpy( name, wname );
#endif

	return string(name);
}

string xml_data( const XML_Char *s, int len )
{
	char * data = (char *) malloc( len + 4 );

#if BT_SIZEOFWCHAR == 2
	wcstombs( data, (const wchar_t *) s, len);
#else
	strncpy( data, s, len );
#endif

	data[len] = (char) NULL;

	string ret = string( data );
	free( data );
	return ret;
}

/**
 * @defgroup group12 Initialization
 * The initialization of BinaryTiers is very straigh forward, everything takes place in the main() function of the program. It consists of a call to bt_begin(), followed by the installation of the Plugins and Themes with _MODEL() and _THEME(), and ends with a call to bt_end().
 * @par Example:
 * @code
 * void main(int argc, char *argv[])
 * {
 * 	bt_begin( argc, argv );
 * 
 * 	_MODEL( system );
 * 	_MODEL( node );
 * 	_MODEL( user );
 * 	_MODEL( menu );
 * 
 * 	_THEME( admin );
 * 	_THEME( site );
 * 
 * 	bt_end();
 * }
 * @endcode
 */

/**
 * @defgroup models Model Functions
 * Models are the programming interface to extend BinaryTiers functionallity. Models might be enabled or disabled by user administrators.\n
 * Each model must define a @p _hooks() function where it will define the model description with _MAP_DESC(), the version with _MAP_VER(), the user permissions with _MAP_PERM() and all the hooks implemented by the model with _MAP_HOOK().
 * @par Example:
 * @code
 * void taxonomy_hooks()
 * {
 * 	_MAP_DESC( taxonomy, "Enables the categorization of content." );
 * 	_MAP_VER ( taxonomy, "1.0.1" );
 * 	_MAP_PERM( taxonomy, "administer taxonomy" );
 * 	_MAP_HOOK( taxonomy, form_alter );
 * 	_MAP_HOOK( taxonomy, insert );
 * 	_MAP_HOOK( taxonomy, update );
 * 	_MAP_HOOK( taxonomy, load );
 * 	_MAP_HOOK( taxonomy, schema );
 * 	_MAP_HOOK( taxonomy, controllers );
 * 	_MAP_HOOK( taxonomy, help );
 * }
 * @endcode
 */

/**
 * @defgroup themes Theme Functions
 * Themes are the programming interface to extend BinaryTiers look and feed. Themes might be enabled or disabled by user administrators.\n
 * Each theme must define a @p _hooks() function where it will define the hooks implemented by the theme with _MAP_THEME_HOOK().
 * @par Example:
 * @code
 * void admin_hooks()
 * {
 * 	_MAP_THEME_HOOK( admin, page_theme );
 * 	_MAP_THEME_HOOK( admin, node_theme );
 * 	_MAP_THEME_HOOK( admin, aggregator_feed_source_theme );
 * 	_MAP_THEME_HOOK( admin, aggregator_item_theme );
 * 	_MAP_THEME_HOOK( admin, aggregator_wrapper_theme );
 * }
 * @endcode
 */

/**
 * @defgroup global Global Functions
 * BinaryTiers has the following Global Functions
 */

/**
 * @defgroup group1 Array Functions
 * Functions related to Arrays
 */

/**
 * @defgroup group7 Page Functions
 * Functions related to Page Functions
 */

/**
 * @defgroup group13 Variable Handling & Conversion
 * Functions related to Variable Handling & Conversion
 */

/**
 * @defgroup group14 String Functions
 * Functions related to Strings
 */

/**
 * @defgroup data_type Data Types
 * BinaryTiers inherits all data types defined in C and C++. The most relevant data type in BinaryTiers is the standard C++ type std:string, commonly used as simply string. Since is used as the exchange data type for most of the functions. The following data types are also defined in BinaryTiers.
 */

/**
 * @defgroup encode Encoding
 * BinaryTiers has the following Encoding Functions
 */

/**
 * @defgroup time Time & Date
 * BinaryTiers has the following Time & Date Functions
 */

/**
 * @mainpage BinaryTiers Manual
 * BinaryTiers is an open source, high-performance C++ web application framework that implements the Model-View-Controller (MVC) architecture pattern. BinaryTiers is designed to be fast, robust, secure and easy to scale.\n\n
 * BinaryTiers is designed keeping processing time in mind, saving energy and computing resources. BinaryTiers web applications range from personal blogs to large social networking websites.\n\n
 * BinaryTiers framework is organized in a set of highly optimized C++ functions that compile your web application in a single and portable executable, allowing the creation of high performance cross-platform machine compiled web applications.\n\n
 * This manual is divided in the following sections:\n
 * <ul>
 * <li>@subpage page1</li>
 * <li>@subpage page2</li>
 * <li>@subpage page3</li>
 * <li>@subpage page7</li>
 * </ul>
 */


/************

 * @section sec1 Getting Started
 * <ol>
 * <li>Make sure that you have the following programs and libraries installed:
 * <ul>
 * <li>g++</li> 
 * <li>mysql client library</li>
 * <li>expat</li>
 * <li>pcre</li>
 * </ul>
 * </li>
 * <li>
 * Get the source code and build the core:
 * @code
 * wget http://www.uselabs.com/download/bt.tar.gz
 * tar -xvzf bt.tar.gz
 * cd bt/lib
 * make
 * @endcode
 * </li>
 * <li>
 * Create an empty Database: You must create an empty database for BinaryTiers, make sure the database user has the rights to use the BinaryTiers database.
 * @code
 * mysqladmin -u MY_DATA_BASE_USER -p create MY_DATA_BASE_NAME
 * @endcode
 * </li>
 * <li>
 * Configure the Database settings and Build BinaryTiers for the first time:
 * @code
 * cd ../src
 * vi settings.h
 * make
 * @endcode
 * </li>
 * <li>
 * Configure the Web Server:
 * <ul><li>If you are using Apache you'll have to edit the file httpd.conf and restart Apache</li></ul>
 * </li>
 * <li>
 * Configure the CMS: 
 * <ul><li>Point your browser to the base URL of your web site</li>
 * <li>Setup your admin username and password</li></ul>
 * </li>
 * </ol>
 * See the section @ref page1 for detailed information on how to install BinaryTiers on different platforms

 

/***********/

/** 
 * @page page1 Installation
 * This section discusses how to download, compile and install BinaryTiers for your platform. 
 * @section sec10 Requirements
 * BinaryTiers has few dependencies, and we'll try to keep this number as low as possible.
 * @section sec10-1 Installing Requirements on Linux/Unix
 * To use BinaryTiers in Unix-like systems you will need the following packages:\n
 * <ul>
 * <li>g++</li> 
 * <li>redis</li>
 * <li>mysql-devel</li>
 * <li>expat-devel</li>
 * <li>pcre-devel</li>
 * </ul>
 * You can install everything at the command prompt:
 * @subsection sec5-1-1 Red Hat Based / Fedora
 * @code
 * yum -y install gcc gcc-c++
 * yum install mysql-devel
 * yum install pcre-devel
 * yum install expat-devel
 * yum install fcgi-devel
 * yum install gd-devel
 * @endcode
 * @subsection sec5-1-2 Debian Based / Ubuntu
 * @code 
 * sudo apt-get update
 * sudo apt-get install g++
 * sudo apt-get install libmysqlclient15-dev
 * sudo apt-get install libgd2-noxpm-dev
 * sudo apt-get install libpcre3-dev libexpat1-dev libfcgi-dev libssl-dev
 * @endcode 
 * @section sec10-2 Installing Requirements on Windows
 * To use BinaryTiers in Windows you will need the following:\n
 * <ul>
 * <li>visual c++</li>
 * <li>mysql windows client (libmysql.lib)</li>
 * <li>expat for windows (xmlparse.lib)</li>
 * <li>pcre for windows (pcrecppd.lib, pcred.lib, librxspencer.lib)</li>
 * </ul>
 * If your copy of BinaryTiers don't have these libraries, you can obtain the libraries from here:
 * <ul>
 * <li>Redis from: http://redis.io</li>
 * <li>MySQL Server from: http://dev.mysql.com/downloads/mysql/</li>
 * <li>Pcre for Windows developer files: http://gnuwin32.sourceforge.net/packages/pcre.htm</li>
 * <li>Expat for Windows: http://expat.sourceforge.net/</li>
 * </ul>
 * @note Additionaly you may need other packages such as: openssl, gd, fcgi, etc. that you can install at any time.
 * @section sec12 Getting the Source Code
 * 
 * @code
 * wget http://www.binarytiers.org/files/binarytiers-2.4.2.tar.gz
 * tar -xvzf binarytiers-2.4.2.tar.gz
 * @endcode
 * @section sec13 Building the Core
 * The first build of the core has to be done at the command prompt; successive builds might be done using the CMS:
 * @code
 * cd binarytiers/lib
 * make
 * @endcode
 * @note On Windows you'll have to build the project btcore.vcproj
 * @section sec14 Create the Database
 * You must create an empty database for BinaryTiers, make sure the database user has the rights to use the BinaryTiers database.
 * @code
 * mysql -u MY_DATA_BASE_USER -p
 * create database MY_DATA_BASE_NAME
 * @endcode
 * On the above code change MY_DATA_BASE_USER to your MySQL username and MY_DATA_BASE_NAME to the name of the database.
 * @section sec15 Configure Settings and First Build
 * @code
 * cd ../src
 * vi settings.h
 * @endcode
 * Edit the settings.h file, and set your Database configuration:
 * @code
 * char * DB_HOST         = "localhost";
 * char * DB_USER         = "MY_DATA_BASE_USER";
 * char * DB_PASSW        = "MY_DATA_BASE_PASSWORD";
 * char * DB_NAME         = "MY_DATA_BASE_NAME";
 * int    DB_TYPE         = 1; // 1:Redis, 2:Mysql
 * int    DB_INDEX        = 0; // Redis Database
 * char * SENDMAIL_HOST   = "localhost";
 * char * SENDMAIL_FROM   = "admin@localhost";
 * char * TEMP_PATH       = "\tmp\";
 * char * BASE_URL        = "http://example.com";
 * int max_content_length = 200000000;
 * int cookie_lifetime    = 2000000;
 * int clean_url          = 1;
 * @endcode
 * Build BinaryTiers for the first time. The first build has to be done at the command prompt; successive builds might be done using the CMS:
 * @code
 * make
 * @endcode
 * @note On Windows you'll have to build the project bt.vcproj
 * @section sec16 Configure the Web Server
 * If you are using Apache, make sure to specify the path of BinaryTiers in the file : /etc/httpd/conf/httpd.conf
 * @code
 * <VirtualHost *:80>
 *    ServerName   example.com
 *    ServerAlias  example.com
 *    DocumentRoot "/MY_BINARYTIERS_PATH/"
 *    DirectoryIndex bt.bin
 *    <Directory />
 *        Options Indexes MultiViews FollowSymLinks
 *        AllowOverride All
 *        Order allow,deny
 *        Allow from all
 *    </Directory>
 *    ScriptAlias /cgi-bin/ "/MY_BINARYTIERS_PATH/"
 * </VirtualHost>
 * @endcode
 * @section sec17 Configure the CMS
 * <ul><li>Point your browser to the base URL of your web site</li>
 * <li>Setup your admin username and password</li></ul>
 */

/** 
 * @page page2 Architecture Pattern
 * BinaryTiers implements the architecture pattern Model-View-Controller (MVC)
 * @section sec2-1 Models
 * Models encapsulate the business logic and the data objects of your web application, in BinaryTiers models are normally backed by a database. Models can be easily extended and they permit an independent development, testing and maintenance.
 * @section sec2-2 Views
 * The View is composed of “themes” that are responsible for providing appropriate representations of your application interface. Themes are CPP files with embedded HTML.
 * @section sec2-3 Controllers
 * The Controllers are responsible for handling incoming HTTP requests and send the response. This response can be processed by the View, or sent directly to the output in order to generate XML, JSON, etc.\n
 * In BinaryTiers controllers can provide a direct response or a pre-processed response that is commonly used in form validations.
 */

/** 
 * @page page3 Getting Stared
 * Follow the guidelines to start developing your web application. We'll keep updating this section, but for now you may find the following resources a good starting point:
 * <ul>
 * <li>@ref group12</li>
 * <li>@ref models</li>
 * <li>@ref themes</li>
 * </ul>
 */

/** 
 * @page page7 Why C++
 * After allot of research of trying to find what is the best language for Web App development. We realized that all the interpreted languages were either created for fun of the language designers or to solve problems that originally had nothing to do with modern Web Application Development, instead they have evolved after the years. And developers in parallel have created web application frameworks that run into those languages to provide the real features that they need.\n\n
 * For example PHP started as a set of C libraries to build dynamic web pages and has evolved to a general-purpose interpreted language in which developers have created blog publishing applications and content management systems. Like WordPress or Drupal.\n\n
 * On September 2009 we begin the development of BinaryTiers, taking everything we have been learning about Web Application Architecture and Content Management Systems during all these years and recreates a true environtment for machine compiled web applications.\n\n
 */
