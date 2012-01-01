/*	Copyright (C) 2009 Use Labs, LLC
    Authors: Sonia G. Garcia, Ivan Daunis

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include <cstdarg>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <iostream>

#include "xmlparse/xmlparse.h"

#if ! defined(BT_SIZEOFWCHAR)
#if __SIZEOF_WCHAR_T__ == 4 || __WCHAR_MAX__ > 0x10000
	#define BT_SIZEOFWCHAR 4
#else
	#define BT_SIZEOFWCHAR 2
#endif
#endif

#ifdef _MSC_VER
	#include <winsock.h> 
	#include <direct.h>
#endif
#ifndef _MSC_VER
	#include <unistd.h>
	#include <sys/types.h>
	#include <dirent.h>
#endif

#include <mysql.h>
#include "settings.h"

using namespace std;

#define	PREG_SPLIT_NO_EMPTY			1
#define	PREG_SPLIT_DELIM_CAPTURE	2
#define	PREG_SPLIT_OFFSET_CAPTURE	4

#define	PREG_OFFSET_CAPTURE			8

#define _CRT_SECURE_NO_WARNINGS
#define floatval(x) atof(x.c_str())
#define intval(x) atoi(x.c_str())
#define isset(x) !x.empty()
#define isset2(x, y) (x.find( y ) != x.end())
#define _TRUE	"1"
#define _FALSE	"0"
#define _PLUGIN(a) _PLUGINS.push_back(#a); a ## _hooks();
#define _THEME(a) _THEMES.push_back(#a); a ## _hooks();
#define _MAP_FORM(x) #x;_CALLBACKS[#x] = x;_SUBMITS[#x] = x##_##submit;_VALIDATES[#x] = x##_##validate;
#define _MAP(x) #x;_CALLBACKS[#x] = x;_SUBMITS[#x] = NULL;_VALIDATES[#x] = NULL;
#define _MAP_ACCESS(x) #x;_ACCESS_CALLBACKS[#x] = x;
#define _QUOTEME(x) #x
#define _MAP_HOOK(x, y) if(_PLUGINS_STRINGS[_QUOTEME(module##_##x)]["on"]=="1" || string(_QUOTEME(y))=="schema" || string(_QUOTEME(y))=="install") _HOOKS[_QUOTEME(x##_##y)] = x##_##y;
#define _MAP_THEME_HOOK(x, y) if(_PLUGINS_STRINGS[_QUOTEME(theme##_##x)]["on"]=="1") _THEME_HOOKS[_QUOTEME(x##_##y)] = x##_##y;
#define _MAP_DESC(x,y) _PLUGINS_STRINGS[#x]["desc"] = y;
#define _MAP_VER(x,y) _PLUGINS_STRINGS[#x]["ver"] = y;
#define _MAP_PERM(x,y) _PLUGINS_STRINGS[#x]["perm"] = y;
#define _POST(x) _POST[x].c_str()
#define _ARG(x) arg(x).c_str()
#define	PUSH(s, d) d.push_back(s); s.clear();

#define MENU_IS_ROOT					0x0001
#define MENU_VISIBLE_IN_TREE			0x0002
#define MENU_VISIBLE_IN_BREADCRUMB		0x0004
#define MENU_VISIBLE_IF_HAS_CHILDREN	0x0008
#define MENU_MODIFIABLE_BY_ADMIN		0x0010
#define MENU_MODIFIED_BY_ADMIN			0x0020
#define MENU_CREATED_BY_ADMIN			0x0040
#define MENU_IS_LOCAL_TASK				0x0080
#define MENU_EXPANDED					0x0100
#define MENU_LINKS_TO_PARENT			0x0200

/**
 * Normal menu items show up in the menu tree and can be moved/hidden by
 * the administrator. Use this for most menu items. It is the default value if
 * no menu item type is specified.
 */
#define MENU_NORMAL_ITEM  (MENU_VISIBLE_IN_TREE | MENU_VISIBLE_IN_BREADCRUMB | MENU_MODIFIABLE_BY_ADMIN)

/**
 * Item groupings are used for pages like "node/add" that simply list
 * subpages to visit. They are distinguished from other pages in that they will
 * disappear from the menu if no subpages exist.
 */
#define MENU_ITEM_GROUPING	(MENU_VISIBLE_IF_HAS_CHILDREN | MENU_VISIBLE_IN_BREADCRUMB | MENU_MODIFIABLE_BY_ADMIN)

/**
 * Callbacks simply register a path so that the correct function is fired
 * when the URL is accessed. They are not shown in the menu.
 */
#define MENU_CALLBACK	MENU_VISIBLE_IN_BREADCRUMB

/**
 * Dynamic menu items change frequently, and so should not be stored in the
 * database for administrative customization.
 */
#define MENU_DYNAMIC_ITEM	(MENU_VISIBLE_IN_TREE | MENU_VISIBLE_IN_BREADCRUMB)

/**
 * Modules may "suggest" menu items that the administrator may enable. They act
 * just as callbacks do until enabled, at which time they act like normal items.
 */
#define MENU_SUGGESTED_ITEM		(MENU_MODIFIABLE_BY_ADMIN | MENU_VISIBLE_IN_BREADCRUMB)

/**
 * Local tasks are rendered as tabs by default. Use this for menu items that
 * describe actions to be performed on their parent item. An example is the path
 * "node/52/edit", which performs the "edit" task on "node/52".
 */
#define MENU_LOCAL_TASK		MENU_IS_LOCAL_TASK

/**
 * Every set of local tasks should provide one "default" task, that links to the
 * same path as its parent when clicked.
 */
#define MENU_DEFAULT_LOCAL_TASK		(MENU_IS_LOCAL_TASK | MENU_LINKS_TO_PARENT)

/**
 * Custom items are those defined by the administrator. Reserved for internal
 * use; do not return from hook_menu() implementations.
 */
#define MENU_CUSTOM_ITEM	(MENU_VISIBLE_IN_TREE | MENU_VISIBLE_IN_BREADCRUMB | MENU_CREATED_BY_ADMIN | MENU_MODIFIABLE_BY_ADMIN)

/**
 * Custom menus are those defined by the administrator. Reserved for internal
 * use; do not return from hook_menu() implementations.
 */
#define MENU_CUSTOM_MENU	(MENU_IS_ROOT | MENU_VISIBLE_IN_TREE | MENU_CREATED_BY_ADMIN | MENU_MODIFIABLE_BY_ADMIN)

#define	FILTER_FORMAT_DEFAULT	"0"
#define	FILTER_HTML_STRIP		"1"
#define	FILTER_HTML_ESCAPE		"2"

#define DRUPAL_AUTHENTICATED_RID	"2"
#define DRUPAL_ANONYMOUS_RID		"1"

extern vector <map <string, map<string, string> > > schema;
extern map <string, map<string,string> > _ELEMENTS;
extern vector <map <string, string> > _PATHS;
extern vector <string> _PLUGINS;
extern vector <string> _THEMES;
extern map <string, map<string,string> > _PLUGINS_STRINGS;
extern map <string, void (*)(void)> _HOOKS;
extern map <string, string (*)( map<string,string> ) > _THEME_HOOKS;
extern map <string, string (*)(void)> _CALLBACKS;
extern map <string, bool (*)(string)> _ACCESS_CALLBACKS;
extern map <string, string (*)(void)> _SUBMITS;
extern map <string, bool (*)(void)> _VALIDATES;
extern map <string, string> _POST;
extern map <string, string> _GET;
extern map <string, string> _COOKIE;
extern map <string, map<string,string> > _FILES;
extern map <string, map<string,string> > node_types;
extern map <string, string> user;
extern map <string, map<string,string> > cur_form;
extern list < pair<int, string> > cur_order;
extern vector <map <string,string> > cur_files;
extern string cur_form_id;
extern map <string, string> cur_node;
extern map <int, map<string,bool> > perm;

extern map <string, string> language;
extern string stored_head;
extern string help;

extern int	pager_total_items;
extern int	pager_page;
extern int	pager_total;

extern string	main_output;

extern bool load_form_only;
extern bool disable_errors;

extern int _PATHS_ACTIVE;

#ifndef __BTMain__
#define __BTMain__

#include "database.h"

bool module_exists(string name);
bool inarray( map <string, string> &item, string find );
string format_date( string timestamp );
string format_interval(int timestamp, int granularity = 2);
string format_size( string str_size );
string format_plural(int count, string singular, string plural);
MYSQL_RES * db_query(const char * format, ...);
MYSQL_RES * db_queryd(const char * format, ...);
string db_last_insert_id();
int db_num_rows( MYSQL_RES *result );
int db_fetch_object( MYSQL_RES *result, map <string, string> &item);
string db_result( MYSQL_RES *result );
MYSQL_RES * pager_query(string query, int limit);
string sql_parse(const char * format, ...);

void profile_load_profile(map <string, string> &account);

string htmlspecialchars( string str );
string urldecode(const std::string & encoded);
string urlencode(const std::string & decoded);
bool match_right(string haystack, string needle);
string rawurlencode(const std::string & decoded);
string getcwd();
int dirread( int handle, string &filename );
int diropen( string dir );
void dirclose( int handle );
int time();
void setcookie(string name, string value, string expire="", string path="", string domain="", int secure=0, int httponly=0);
string date( string format, string time = "" );
string gmdate( string format, string timestamp = "" );
string md5( string data );
string chr( int n );
string chr( size_t n );
string chr( double n );
string str_replace(string search, string replace, string subject);
string str_repeat( string str, int multiplier );
string addslashes( string str );
string arg( int num );
string t( string str, string langcode = "" );
string variable_get(string name, string def="");
void variable_get(string name, vector <pair <string, string> > &result);
void variable_get(string name, map <string, string> &result);
void drupal_set_header( string header );
void drupal_set_html_head( string data );
void drupal_goto( string path );
void drupal_add_js(string file, bool nocache = false);
string drupal_get_title();
void drupal_get_extra( map <string, string> &item );
void drupal_set_title(string title);
void drupal_set_extra(string prefix, map <string, string> extra);
string implode(map <string, string> &pieces, string glue);
string implode(vector <string> &pieces, string glue);
void explode(vector <string> &pieces, string the_string, char separator=',');
void explode(map <string, string> &pieces, string the_string, char separator=',');
string url( string path, map <string, string> *options = NULL );
string url_php( string path, string query = "" );
void print( string str );
void print_r( map <string, map<string,string> > &items );
void print_r( vector <map <string, string> > &items );
void print_r( vector <string> &items );
void print_r( map <string, string> &item );
void drupal_set_message(string message, string type = "status");
void drupal_http_request( map <string, string> &result, string url, map <string, string> headers, string method="GET", string data="" );
int sendmail( string from, string to, string subject, string message );
string theme( string hook, map <string, string> &item );
string theme_pager();
string theme_table( vector <string> &header, vector <vector <map <string, string> > > &rows, string table_attributes="" );
bool valid_email_address(string mail);
bool ereg(char *pattern, char *string);
bool preg_match(char *pattern, char *string, void *matches = NULL);

string get_form(string form_name, map<string, map<string,string> > &form, string action = string(""));
bool form_validate( map<string, map<string,string> > &form );
void form_set_error(string field, string message);

bool user_access(string arguments);
bool user_is_authenticated(string arguments="");

void btarray_to_vector( map <string, string> *item, string var, vector <string> &value );
void vector_to_btarray( map <string, string> *item, string var, vector <string> &value );
void btarray_to_btarray( map <string, string> *item, string var, map <string, string> *dest_item, string dest_var );
void clear_btarray( map <string, string> *item, string var );
string serialize_array( map <string, string> data );
string serialize_array( vector <pair <string, string> > data );
void unserialize_array( string data, map <string, string> &result );
void unserialize_array( string data, vector <pair <string, string> > &result );
bool is_string( string str );
int  is_dir( string path );
string strtotime( string date );
char *strtoupper(char *s1);
char *strtolower(char *s1);
string strtolower(string str);
void quit( int code = 0 );

void trim(string& str, const char *chr=" \t\n\r\x0B");
void trim(string& str, char chr);
void ltrim(string& str, char chr=' ');
void debug( string str );
bool file_exists( string filename );
string file_get_contents( string filename );
int filesize ( string filename );
int db_affected_rows();
string check_plain( string text );
string check_url( string url );

void variable_set(string name, string value);
void variable_get(string name, map <string, string> &result);

string file_directory_path();
string file_directory_temp();

string filter_xss( string str, vector < string > allowed_tags );
vector < string > preg_split(char *pattern, string text, int limit = -1, int flags = 0 );
string strip_tags( string text );

#endif
