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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/timeb.h>
#include <time.h>
#include <math.h>

#include <cstdarg>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <iostream>

#include "xmlparse/xmlparse.h"

using namespace std;

#if ! defined(BT_SIZEOFWCHAR)
#if __SIZEOF_WCHAR_T__ == 4 || __WCHAR_MAX__ > 0x10000
	#define BT_SIZEOFWCHAR 4
#else
	#define BT_SIZEOFWCHAR 2
#endif
#endif

#ifdef _MSC_VER
	//#include <winsock.h> 
	#include <winsock2.h>
	#include <direct.h>
	#include <hash_map>
	using namespace stdext;
	#define sleep(x) ::Sleep( x * 1000);
#endif
#ifndef _MSC_VER
	#include <unistd.h>
	#include <sys/types.h>
	#include <dirent.h>
	#include <ext/hash_map>
	using namespace __gnu_cxx;
#endif

#include <mysql.h>
#include <hiredis.h>

#ifndef _REDIS_RES
#define _REDIS_RES
typedef struct {
    int redis_count;
    redisReply *redis_reply;
	char *redis_fields;
} REDIS_RES;
#endif

/** Begin Setting Variables */ 
extern char * DB_HOST;
extern char * DB_USER;
extern char * DB_PASSW;
extern char * DB_NAME;
extern int DB_TYPE;
extern int DB_INDEX;

extern char * SENDMAIL_HOST;
extern char * SENDMAIL_FROM;
extern char * TEMP_PATH;
extern char * BASE_URL;
extern char * BASE_URL_PHP;

extern int	max_content_length; // The value can also be specified in the cookie: _COOKIE["BTMCL"]
extern int	cookie_lifetime;
extern int	clean_url;
/** End Setting Variables */ 


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

#define	PREG_SPLIT_NO_EMPTY			1
#define	PREG_SPLIT_DELIM_CAPTURE	2
#define	PREG_SPLIT_OFFSET_CAPTURE	4

#define	PREG_OFFSET_CAPTURE			8

/**
 * An @ref Array is a list of strings. To have an Array of any type of data use ArrayOf().\n
 * An @ref Array is essentially a C++ vector container of strings which means that their elements can be accessed using offsets on regular pointers to elements and also using iterators. C++ vectors are generally the most efficient in time for accessing elements, add elements and remove elements from the end of the sequence.
 * <table>
 * <tr><th>Iterators</th><th></th></tr>
 * <tr><td>begin</td><td>Return iterator to beginning</td></tr>
 * <tr><td>end</td><td>Return iterator to end</td></tr>
 * <tr><td>rbegin</td><td>Return reverse iterator to reverse beginning</td></tr>
 * <tr><td>rend</td><td>Return reverse iterator to reverse end</td></tr>
 * <tr><th>Capacity</th><th></th></tr>
 * <tr><td>size</td><td>Return size</td></tr>
 * <tr><td>max_size</td><td>Return maximum size</td></tr>
 * <tr><td>resize</td><td>Change size</td></tr>
 * <tr><td>capacity</td><td>Return size of allocated storage capacity</td></tr>
 * <tr><td>empty</td><td>Test whether vector is empty</td></tr>
 * <tr><td>reserve</td><td>Request a change in capacity</td></tr>
 * <tr><th>Iterators</th><th></th></tr>
 * <tr><td>operator[]</td><td>Access element</td></tr>
 * <tr><td>at</td><td>Access element</td></tr>
 * <tr><td>front</td><td>Access first element</td></tr>
 * <tr><td>back</td><td>Access last element</td></tr>
 * <tr><th>Modifiers</th><th></th></tr>
 * <tr><td>assign</td><td>Assign vector content</td></tr>
 * <tr><td>push_back</td><td>Add element at the end</td></tr>
 * <tr><td>pop_back</td><td>Delete last element</td></tr>
 * <tr><td>insert</td><td>Insert elements</td></tr>
 * <tr><td>erase</td><td>Erase elements</td></tr>
 * <tr><td>swap</td><td>Swap content</td></tr>
 * <tr><td>clear</td><td>Clear content</td></tr>
 * </table>
 * @par Example:
 * @code
 * Array list;
 * for( size_t i=0; i<8; i++ )
 * {
 *		list.push_back( str( (int) pow(2, (double)i) ) );
 * }
 * explain( list );
 * @endcode
 * @hideinitializer @ingroup data_type
 */
#define Array		vector <string>

/**
 * An ArrayOf() is a list of elements of any type of data, the data type is specified by the parameter datatype.\n
 * An ArrayOf() is essentially a C++ vector container of strings which means that their elements can be accessed using offsets on regular pointers to elements and also using iterators. C++ vectors are generally the most efficient in time for accessing elements, add elements and remove elements from the end of the sequence.
 * <table>
 * <tr><th>Iterators</th><th></th></tr>
 * <tr><td>begin</td><td>Return iterator to beginning</td></tr>
 * <tr><td>end</td><td>Return iterator to end</td></tr>
 * <tr><td>rbegin</td><td>Return reverse iterator to reverse beginning</td></tr>
 * <tr><td>rend</td><td>Return reverse iterator to reverse end</td></tr>
 * <tr><th>Capacity</th><th></th></tr>
 * <tr><td>size</td><td>Return size</td></tr>
 * <tr><td>max_size</td><td>Return maximum size</td></tr>
 * <tr><td>resize</td><td>Change size</td></tr>
 * <tr><td>capacity</td><td>Return size of allocated storage capacity</td></tr>
 * <tr><td>empty</td><td>Test whether vector is empty</td></tr>
 * <tr><td>reserve</td><td>Request a change in capacity</td></tr>
 * <tr><th>Iterators</th><th></th></tr>
 * <tr><td>operator[]</td><td>Access element</td></tr>
 * <tr><td>at</td><td>Access element</td></tr>
 * <tr><td>front</td><td>Access first element</td></tr>
 * <tr><td>back</td><td>Access last element</td></tr>
 * <tr><th>Modifiers</th><th></th></tr>
 * <tr><td>assign</td><td>Assign vector content</td></tr>
 * <tr><td>push_back</td><td>Add element at the end</td></tr>
 * <tr><td>pop_back</td><td>Delete last element</td></tr>
 * <tr><td>insert</td><td>Insert elements</td></tr>
 * <tr><td>erase</td><td>Erase elements</td></tr>
 * <tr><td>swap</td><td>Swap content</td></tr>
 * <tr><td>clear</td><td>Clear content</td></tr>
 * </table>
 * @param datatype Type of data for the elements in the array to be declared.
 * @par Example:
 * @code
 * ArrayOf(int) list;
 * for( size_t i=0; i<8; i++ )
 * {
 *      list.push_back( (int) pow(2, (double)i) );
 * }
 * @endcode
 * @hideinitializer @ingroup data_type
 */
#define ArrayOf(datatype)	vector <datatype>

/**
 * An @ref Assoc is an associative array of strings in which keys and values are strings. To have the values of any type of data use AssocOf().\n
 * An @ref Assoc is essentially a C++ hash_map container of string keys and string values. Looking up an element in a hash_map by its key is efficient, so hash_map is useful for "dictionaries" where the order of elements is irrelevant
 * <table>
 * <tr><th>Iterators</th><th></th></tr>
 * <tr><td>begin</td><td>Return iterator to beginning</td></tr>
 * <tr><td>end</td><td>Return iterator to end</td></tr>
 * <tr><td>rbegin</td><td>Return reverse iterator to reverse beginning</td></tr>
 * <tr><td>rend</td><td>Return reverse iterator to reverse end</td></tr>
 * <tr><th>Capacity</th><th></th></tr>
 * <tr><td>empty</td><td>Test whether container is empty</td></tr>
 * <tr><td>size</td><td>Return container size</td></tr>
 * <tr><td>max_size</td><td>Return maximum size</td></tr>
 * <tr><th>Access</th><th></th></tr>
 * <tr><td>operator[]</td><td>Access element</td></tr>
 * <tr><th>Modifiers</th><th></th></tr>
 * <tr><td>insert</td><td>Insert element</td></tr>
 * <tr><td>erase</td><td>Erase element</td></tr>
 * <tr><td>swap</td><td>Swap content</td></tr>
 * <tr><td>clear</td><td>Clear content</td></tr>
 * <tr><th>Observers</th><th></th></tr>
 * <tr><td>key_comp</td><td>Return key comparison object</td></tr>
 * <tr><td>value_comp</td><td>Return value comparison object</td></tr>
 * <tr><th>Operations</th><th></th></tr>
 * <tr><td>find</td><td>Get iterator to element</td></tr>
 * <tr><td>count</td><td>Count elements with a specific key</td></tr>
 * <tr><td>lower_bound</td><td>Return iterator to lower bound</td></tr>
 * <tr><td>upper_bound</td><td>Return iterator to upper bound</td></tr>
 * <tr><td>equal_range</td><td>Get range of equal elements</td></tr>
 * </table>
 * @par Example:
 * @code
 * Assoc item;
 * item.clear();
 * item["path"] = "node/%";
 * item["title"] = "View Content";
 * item["access arguments"] = "access content";
 * item["callback"] = _MAP( node_page_view );
 * item["type"] = str( MENU_LOCAL_TASK );
 * @endcode
 * @hideinitializer @ingroup data_type
 */
#define Assoc		map <string, string>

/**
 * An AssocOf() is an associative array in which keys are strings and values of any type of data specified by the parameter datatype.\n
 * An AssocOf() is essentially a C++ hash_map container of string keys and string values. Looking up an element in a hash_map by its key is efficient, so hash_map is useful for "dictionaries" where the order of elements is irrelevant
 * <table>
 * <tr><th>Iterators</th><th></th></tr>
 * <tr><td>begin</td><td>Return iterator to beginning</td></tr>
 * <tr><td>end</td><td>Return iterator to end</td></tr>
 * <tr><td>rbegin</td><td>Return reverse iterator to reverse beginning</td></tr>
 * <tr><td>rend</td><td>Return reverse iterator to reverse end</td></tr>
 * <tr><th>Capacity</th><th></th></tr>
 * <tr><td>empty</td><td>Test whether container is empty</td></tr>
 * <tr><td>size</td><td>Return container size</td></tr>
 * <tr><td>max_size</td><td>Return maximum size</td></tr>
 * <tr><th>Access</th><th></th></tr>
 * <tr><td>operator[]</td><td>Access element</td></tr>
 * <tr><th>Modifiers</th><th></th></tr>
 * <tr><td>insert</td><td>Insert element</td></tr>
 * <tr><td>erase</td><td>Erase element</td></tr>
 * <tr><td>swap</td><td>Swap content</td></tr>
 * <tr><td>clear</td><td>Clear content</td></tr>
 * <tr><th>Observers</th><th></th></tr>
 * <tr><td>key_comp</td><td>Return key comparison object</td></tr>
 * <tr><td>value_comp</td><td>Return value comparison object</td></tr>
 * <tr><th>Operations</th><th></th></tr>
 * <tr><td>find</td><td>Get iterator to element</td></tr>
 * <tr><td>count</td><td>Count elements with a specific key</td></tr>
 * <tr><td>lower_bound</td><td>Return iterator to lower bound</td></tr>
 * <tr><td>upper_bound</td><td>Return iterator to upper bound</td></tr>
 * <tr><td>equal_range</td><td>Get range of equal elements</td></tr>
 * </table>
 * @param datatype Type of data for the values in the associative array to be declared.
 * @par Example:
 * @code
 * AssocOf( Assoc ) form;
 * form["title"]["#type"] = "textfield";
 * form["title"]["#title"] = "Title";
 * form["title"]["#size"] = "60";
 * form["title"]["#weight"] = "-1";
 * form["title"]["#required"] = _TRUE;
 * form["title"]["#value"] = form["#node"]["title"];
 * @endcode
 * @hideinitializer @ingroup data_type
 */
#define AssocOf(datatype)	map <string, datatype>

#define floatval(number) atof(number.c_str()) /**< The floatval() function converts a string to a floating number. @param number The string to be converted. @returns A double precision value. @hideinitializer @ingroup group13*/
#define intval(number) atoi(number.c_str()) /**< The intval() function converts a string to a number. @param number The string to be converted. @returns An integer. @hideinitializer @ingroup group13*/
#define isset(object) !object.empty() /**< Test whether the string, vector or container is empty. @hideinitializer @ingroup group13*/
#define isset2(object, value) (object.find( value ) != object.end()) /**< Test whether a value is found within an associative array. @hideinitializer @ingroup group13*/
#define _TRUE	"1" /**< Constant value equal to the string "1" @hideinitializer @ingroup data_type*/
#define _FALSE	"0" /**< Constant value equal to the string "0" @hideinitializer @ingroup data_type*/
#define _MODEL(model_name) _MODELS.push_back(#model_name); model_name ## _hooks(); /**< The _MODEL() function adds a model into the models stack and executes the hooks function for the specified model. @param model_name The name of the model to be added. @returns None. @hideinitializer @ingroup group12*/
#define _THEME(theme_name) _THEMES.push_back(#theme_name); theme_name ## _hooks(); /**< The _THEME() function adds a theme into the themes stack and executes the hooks function for the specified theme. @param theme_name The name of the theme to be added. @returns None. @hideinitializer @ingroup group12*/

/** The _MAP_FORM() function defines a form callback function for a menu item. The function must be defined in the attribute ["callback"] of a menu item.
 * @param callback_function Name of the form callback function that will be called when the menu item is accessed
 * @par Example:
 * @code
 * item["callback"] = _MAP_FORM( locale_admin_language );
 * @endcode @hideinitializer @ingroup models
 */
#define _MAP_FORM(callback_function) #callback_function;_CALLBACKS[#callback_function] = callback_function;_SUBMITS[#callback_function] = callback_function##_##submit;_VALIDATES[#callback_function] = callback_function##_##validate; /**< An explanation @hideinitializer @ingroup models*/

/** The _MAP() function defines a page callback function for a menu item. The function must be defined in the attribute ["callback"] of a menu item.
 * @param callback_function Name of the page callback function that will be called when the menu item is accessed
 * @par Example:
 * @code
 * item["callback"] = _MAP( locale_admin_translate );
 * @endcode @hideinitializer @ingroup models
 */
#define _MAP(callback_function) #callback_function;_CALLBACKS[#callback_function] = callback_function;_SUBMITS[#callback_function] = NULL;_VALIDATES[#callback_function] = NULL;

/** The _MAP_ACCESS() function defines a custom access function for a menu item. The custom function must be defined in the attribute ["access callback"] of a menu item. If the ["access callback"] attribute is not specified BinaryTiers will call the default access function user_access()
 * @param callback_function Name of the callback function that will be called before accessing the menu option
 * @par Example:
 * @code
 * item["access callback"] = _MAP_ACCESS( user_is_not_authenticated );
 * @endcode @hideinitializer @ingroup models
 */
#define _MAP_ACCESS(callback_function) #callback_function;_ACCESS_CALLBACKS[#callback_function] = callback_function;
#define _QUOTEME(x) #x
 
/**
 * The _MAP_HOOK() function declares each hook implemented by a plugin.
 * @param model_name Name of the model
 * @param hook_function Name of the hook function to be implemented
 * @par Model Hook Functions:
 * \n<table>
 * <tr><th>Hook Function</th><th>Usage</th></tr>
 * <tr><td>form_alter</td><td>To add or modify an existing form</td></tr>
 * <tr><td>elements</td><td>To change or add new form elements and behabiour</td></tr>
 * <tr><td>node_info</td><td>To define new node types</td></tr>
 * <tr><td>controllers</td><td>To add the controllers that will respond to page calls</td></tr>
 * <tr><td>help</td><td>To display help information</td></tr>
 * <tr><td>insert</td><td>Called when a new node is created</td></tr>
 * <tr><td>update</td><td>Called when a node is updated</td></tr>
 * <tr><td>delete</td><td>Called when a node is deleted</td></tr>
 * <tr><td>load</td><td>Called when a node is loaded</td></tr>
 * <tr><td>schema</td><td>To specify the MySQL tables that will be created during the installation or activation of a model. Only needed in MySQL environments</td></tr>
 * <tr><td>install</td><td>Function called during the installation process</td></tr>
 * </table>  
 * @par Example:
 * @code
 * _MAP_HOOK( taxonomy, insert );
 * @endcode @hideinitializer @ingroup models
 */
#define _MAP_HOOK(model_name, hook_function) if(_PLUGINS_STRINGS[_QUOTEME(model##_##model_name)]["on"]=="1" || string(_QUOTEME(hook_function))=="schema" || string(_QUOTEME(hook_function))=="install") _HOOKS[_QUOTEME(model_name##_##hook_function)] = model_name##_##hook_function;

/**
 * The _MAP_DESC() function sets the description of a model.
 * @param model_name Name of the model
 * @param description String containing the model description
 * @par Example:
 * @code
 * _MAP_DESC( taxonomy, "Enables the categorization of content." );
 * @endcode @hideinitializer @ingroup models
 */
#define _MAP_DESC(model_name,description) _PLUGINS_STRINGS[#model_name]["desc"] = description;

/**
 * The _MAP_VER() function sets the version of a model.
 * @param model_name Name of the model
 * @param version String containing the model version
 * @par Example:
 * @code
 * _MAP_VER( taxonomy, "1.0.1" );
 * @endcode @hideinitializer @ingroup models
 */
#define _MAP_VER(model_name,version) _PLUGINS_STRINGS[#model_name]["ver"] = version;

/**
 * The _MAP_PERM() function sets the user permissions of the model.
 * @param model_name Name of the model
 * @param permissions String containing the list of user permissions separated by comma, that are accessible when the model is enabled
 * @par Example:
 * @code
 * _MAP_PERM( node, "administer content types, administer nodes, access content, view revisions, revert revisions, delete revisions" );
 * @endcode @hideinitializer @ingroup models
 */
#define _MAP_PERM(model_name,permissions) _PLUGINS_STRINGS[#model_name]["perm"] = permissions;

/**
 * The _MAP_THEME_HOOK() function declares each hook implemented by a theme.
 * @param theme_name Name of the theme
 * @param hook_function Name of the hook function to be implemented
 * @par Theme Hook Functions:
 * \n<table>
 * <tr><th>Hook Function</th><th>Usage</th></tr>
 * <tr><td>page_theme</td><td>Called on every page request</td></tr>
 * <tr><td>node_theme</td><td>Called on every node_view() request</td></tr>
 * <tr><td>error_theme</td><td>Called when a page is not found</td></tr>
 * <tr><td>denied_theme</td><td>Called when the user doesn't have permissions to view the page</td></tr>
 * </table>  
 * @par Example:
 * @code
 * _MAP_THEME_HOOK( admin, page_theme );
 * @endcode @hideinitializer @ingroup themes
 */
#define _MAP_THEME_HOOK(theme_name, hook_function) if(_PLUGINS_STRINGS[_QUOTEME(theme##_##theme_name)]["on"]=="1") _THEME_HOOKS[_QUOTEME(theme_name##_##hook_function)] = theme_name##_##hook_function;

/**
 * The _SERVER() function is a helper function to access the Environment variables.\n
 * The Environment variables are a series of values that the web server and the browser send to BinaryTiers on every page request.\n
 * This is the list of the standard Apache Environmental Variables, other servers and models may create other variables:
 * <table>
 * <tr><th>Variable</th><th>Value</th></tr>
 * <tr><td>DOCUMENT_ROOT</td><td>The root directory of your server</td></tr>
 * <tr><td>HTTP_COOKIE</td><td>The visitor's cookie, if one is set</td></tr>
 * <tr><td>HTTP_HOST</td><td>The hostname of the page being attempted</td></tr>
 * <tr><td>HTTP_REFERER</td><td>The URL of the page that called your program</td></tr>
 * <tr><td>HTTP_USER_AGENT</td><td>The browser type of the visitor</td></tr>
 * <tr><td>HTTPS</td><td>"on" if the program is being called through a secure server</td></tr>
 * <tr><td>PATH</td><td>The system path your server is running under</td></tr>
 * <tr><td>QUERY_STRING</td><td>The query string (see GET, below)</td></tr>
 * <tr><td>REMOTE_ADDR</td><td>The IP address of the visitor</td></tr>
 * <tr><td>REMOTE_HOST</td><td>The hostname of the visitor (if your server has reverse-name-lookups on; otherwise this is the IP address again)</td></tr>
 * <tr><td>REMOTE_PORT</td><td>The port the visitor is connected to on the web server</td></tr>
 * <tr><td>REMOTE_USER</td><td>The visitor's username (for .htaccess-protected pages)</td></tr>
 * <tr><td>REQUEST_METHOD</td><td>GET or POST</td></tr>
 * <tr><td>REQUEST_URI</td><td>The interpreted pathname of the requested document or CGI (relative to the document root)</td></tr>
 * <tr><td>SCRIPT_FILENAME</td><td>The full pathname of the current CGI</td></tr>
 * <tr><td>SCRIPT_NAME</td><td>The interpreted pathname of the current CGI (relative to the document root)</td></tr>
 * <tr><td>SERVER_ADMIN</td><td>The email address for your server's webmaster</td></tr>
 * <tr><td>SERVER_NAME</td><td>Your server's fully qualified domain name (e.g. www.cgi101.com)</td></tr>
 * <tr><td>SERVER_PORT</td><td>The port number your server is listening on</td></tr>
 * <tr><td>SERVER_SOFTWARE</td><td>The server software you're using (e.g. Apache 1.3)</td></tr>
 * </table>
 * @par Example:
 * @code
 * print( "Your IP address is: "+_SERVER("REMOTE_ADDR") );
 * @endcode
 * @hideinitializer @ingroup global
 */
#define _SERVER(variable) (getenv(variable) ? string(getenv(variable)) : string(""))

/** 
 * The poke() function pushes back an item into a vector and clears the item's content. 
 * @param object The source vector where we want to adds the element at the end.
 * @param item The element that we want to add and clear its contents for reuse.
 * @hideinitializer @ingroup group13
 */
#define	poke(object, item) object.push_back(item); item.clear();

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
 * Models may "suggest" menu items that the administrator may enable. They act
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
 * use; do not return from hook_controllers() implementations.
 */
#define MENU_CUSTOM_ITEM	(MENU_VISIBLE_IN_TREE | MENU_VISIBLE_IN_BREADCRUMB | MENU_CREATED_BY_ADMIN | MENU_MODIFIABLE_BY_ADMIN)

/**
 * Custom menus are those defined by the administrator. Reserved for internal
 * use; do not return from hook_controllers() implementations.
 */
#define MENU_CUSTOM_MENU	(MENU_IS_ROOT | MENU_VISIBLE_IN_TREE | MENU_CREATED_BY_ADMIN | MENU_MODIFIABLE_BY_ADMIN)

#define	FILTER_FORMAT_DEFAULT	"0"
#define	FILTER_HTML_STRIP		"1"
#define	FILTER_HTML_ESCAPE		"2"

#define AUTHENTICATED_RID	"2"
#define ANONYMOUS_RID		"1"

extern ArrayOf( AssocOf ( Assoc ) ) schema;
extern AssocOf( Assoc ) _ELEMENTS;
extern ArrayOf( Assoc ) controllers;
extern Array _MODELS;
extern Array _THEMES;
extern AssocOf( Assoc ) _PLUGINS_STRINGS;
extern AssocOf( void (*)(void) ) _HOOKS;
extern AssocOf( string (*)( Assoc ) ) _THEME_HOOKS;
extern AssocOf( string (*)(void) ) _CALLBACKS;
extern AssocOf( bool (*)(string) )_ACCESS_CALLBACKS;
extern AssocOf( string (*)(void) ) _SUBMITS;
extern AssocOf( bool (*)(void) ) _VALIDATES;
extern Assoc _POST;
extern Assoc _GET;
extern Assoc _COOKIE;
extern AssocOf( Assoc ) _FILES;
extern AssocOf( Assoc ) node_types;
extern Assoc user;
extern AssocOf( Assoc ) cur_form;
extern list < pair<int, string> > cur_order;
extern ArrayOf( Assoc ) cur_files;
extern string cur_form_id;
extern Assoc cur_node;
extern map <int, map<string,bool> > perm;

extern Assoc language;
extern string stored_head;
extern string help;

extern int	pager_total_items;
extern int	pager_page;
extern int	pager_total;

extern string	main_output;

extern bool load_form_only;
extern bool disable_errors;

extern int _ACTIVE_CONTROLLER;

#ifndef __BTMain__
#define __BTMain__

#include "database.h"
#include "form.h"
#include "user.h"

bool model_exists( string name );
bool inarray( vector <string> &item, string find );
bool inarray( map <string, string> &item, string find );
string format_date( string timestamp );
string format_interval( int timestamp, int granularity = 2 );
string format_size( string str_size );
string format_plural( int count, string singular, string plural );

string redis_arg(const char *format, ...);
int redis_multi( vector <pair <string, string> > &item, const char * format, ... );
int redis_multi( map <string, string> &item, const char * format, ... );
int redis_multi_reversed( vector <pair <string, string> > &item, const char * format, ... );
int redis_multi_reversed( map <string, string> &item, const char * format, ... );
int redis_vector( vector <string> &items, const char * format, ... );
int redis_int(const char * format, ...);
string redis_str(const char * format, ...);
REDIS_RES * redis_query(const char * format, ... );
REDIS_RES * redis_query_fields(string command, string repeat, string the_fields);
REDIS_RES * redis_pager_fields(string command, string repeat, string the_fields, int limit);
void redis_command(const char * format, ... );
void redis_command_fields(string command, string repeat, string the_fields, const char *values = NULL, ...);
bool redis_fetch(REDIS_RES *res, string &item );
bool redis_fetch_fields( REDIS_RES *res, map <string, string> &item );
void redis_free( REDIS_RES *res );

MYSQL_RES * db_query( string format, string p1="", string p2="", string p3="", string p4="", string p5="", string p6="", string p7="", string p8="", string p9="", string p10="", string p11="", string p12="" );
MYSQL_RES * db_queryd( string format, string p1="", string p2="", string p3="", string p4="", string p5="", string p6="", string p7="", string p8="", string p9="", string p10="", string p11="", string p12="" );
MYSQL_RES * db_querya( const char * format, ... );
MYSQL_RES * db_queryad( const char * format, ... );
string db_last_insert_id();
int db_num_rows( MYSQL_RES *result );
int db_fetch( MYSQL_RES *result, map <string, string> &item );
string db_result( MYSQL_RES *result );
MYSQL_RES * pager_query( string query, int limit );
MYSQL_RES * pager_query(string query, string count_query, int limit);
string sql_parse( const char * format, ... );

void http_header( string header );
void redirect( string path );
void add_html_head( string html_code );
void add_js( string file, bool nocache = false );
void add_css( string file, bool nocache = false );
string get_page_title();
void get_page_extra( map <string, string> &item );
void set_page_title( string title );
void set_page_extra( string prefix, map <string, string> extra );
void set_page_message( string message, string type = "status" );
int  parse_url( string uri, map <string, string> &result );
void http_request( map <string, string> &result, string url, map <string, string> headers, string method="GET", string data="" );
void setcookie( string name, string value, string expire="", string path="", string domain="", int secure=0, int httponly=0 );
void setcookie( string name, string value, int expire=0, string path="", string domain="", int secure=0, int httponly=0 );

string base64_encode( string source );
string base64_decode( string source );

string htmlencode( string str );
string urldecode( string encoded );
string urlencode( string decoded );
string rawurlencode( string decoded );

bool is_dir( string path );
string getcwd();
int dirread( int handle, string &filename );
int diropen( string dir );
void dirclose( int handle );

int time();
string microtime();
string date( string format, string time = "" );
string gmdate( string format, string time = "", int timezone = 0 );
string md5( string data );
string sha1( string data );
size_t crc32( string data );
size_t crc32( unsigned char *data, size_t len );
string qpencode(string str);
string num( string s );
string str( size_t n );
string str( int n );
string str( double n );
string addslashes( string str );
string jsonencode( string out ) ;
bool match_right( string haystack, string needle );
string str_replace( string search, string replace, string subject );
string str_repeat( string str, int multiplier );
bool is_numeric( string str );
string strtotime( string date );

string strtoupper( string str );
char *strtoupper( char *str );
string strtolower( string str );
char *strtolower( char *str );

void trim( string& str, const char *chr=" \t\n\r\x0B" );
void trim( string& str, char chr );
void trim( vector <string> &pieces, const char *chr=" \t\n\r\x0B" );
void trim( map <string, string> &item, const char *chr=" \t\n\r\x0B" );
void ltrim( string& str, const char *chr=" \t\n\r\x0B" );
void ltrim( string& str, char chr );
void rtrim( string& str, const char *chr=" \t\n\r\x0B" );
void rtrim( string& str, char chr );

string implode( map <string, string> &pieces, string glue, string keyglue="=");
string implode( vector <string> &pieces, string glue);
void explode( vector <string> &pieces, string source, char separator=',');
void explode( map <string, string> &pieces, string source, char separator=',', char keyseparator='=');
string arg( int num );
string t( string str, string langcode = "" );
string url( string path, map <string, string> *options = NULL );
string url( string path, bool absolute );
void print( string str );
void explain( map <string, map<string,string> > &items );
void explain( vector <map <string, string> > &items );
void explain( vector <string> &items );
void explain( map <string, string> &item );
void explain( vector <pair <string, string> > items );
void jsondecode( map <string, string> &pieces, string the_string );
string variable_get( string name, string def="" );
void variable_get( string name, vector <pair <string, string> > &result );
void variable_get( string name, map <string, string> &result );
void variable_set( string name, string value );
void variable_get( string name, map <string, string> &result );

int sendmail( string from, string to, string subject, string message );
string theme( string hook, map <string, string> &item );
string theme_pager();
string theme_table( vector <string> &header, vector <vector <map <string, string> > > &rows, string table_attributes="" );
bool valid_email_address( string mail );
bool ereg( char *pattern, char *string );
bool preg_match( char *pattern, char *string, void *matches = NULL );

void unnest( map <string, string> *source_array, string source_key, vector <string> &vector );
void nest( vector <string> &source_vector, map <string, string> *dest_array, string dest_key );
void nestcopy( map <string, string> *source_array, string source_key, map <string, string> *dest_array, string dest_key );
void nestclear( map <string, string> *source_array, string source_key );

string serialize_array( map <string, string> data );
string serialize_array( vector <pair <string, string> > data );
void unserialize_array( string data, map <string, string> &result );
void unserialize_array( string data, vector <pair <string, string> > &result );

void quit( int code = 0 );
void debug( string str );
bool file_exists( string filename );
string file_get_contents( string filename );
string tempfile( const char * tempdir = NULL, const char * prefix = "tmp" );
int filesize ( string filename );
int filecopy ( string filename, string dest );
int db_affected_rows();
string check_plain( string text );
string check_url( string url );
string session_id();

string file_directory_path();
string file_directory_temp();
bool file_set_status( map <string, string> &file, int status );
bool file_save_upload( map <string, string> &file, string source, string validators, string dest, int replace = FILE_EXISTS_RENAME );

string filter_xss( string str, string allowed_tags );
vector < string > preg_split( char *pattern, string text, int limit = -1, int flags = 0 );
string strip_tags( string text );

string xml_start( map <string, string> &attributes, const char *wname, const char **atts );
string xml_data( const XML_Char *s, int len );
string xml_end( const char *wname );

string capwords( string s );
char *capwords( char *s1 );

void bt_begin( int argc, char *argv[] );
void bt_end();

#endif
