/* 
** Copyright (C) 2011 Uselabs and/or its subsidiary(-ies).
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
#include "locale.h"
#include "node.h"

vector <map <string, string> > languages_t;
map <string, map <string, string> > locales_t;
string pager_get_querystring();

void locale_hooks()
{
	_MAP_DESC( locale, "Adds language handling functionality and enables the translation of the user interface to languages other than English." );
	_MAP_VER ( locale, "1.3.1" );
	_MAP_PERM( locale, "administer languages,administer translate,can upload csv" );
	_MAP_HOOK( locale, schema );
	_MAP_HOOK( locale, controllers );
}

void locale_schema()
{
	map <string, map<string, string> > languages;
	languages["#spec"]["name"] = "languages";
	languages["#spec"]["description"] = "List of all available languages in the system.";
	languages["language"]["type"] = "varchar";
	languages["language"]["length"] = "12";
	languages["language"]["not null"] = _TRUE;
	languages["language"]["default"] = "";
	languages["language"]["description"] = "The language code, e.g. 'de' or 'en-us'";
	languages["name"]["type"] = "varchar";
	languages["name"]["length"] = "64";
	languages["name"]["not null"] = _TRUE;
	languages["name"]["default"] = "";
	languages["name"]["description"] = "Language name in English";
	languages["native"]["type"] = "varchar";
	languages["native"]["length"] = "64";
	languages["native"]["not null"] = _TRUE;
	languages["native"]["default"] = "";
	languages["native"]["description"] = "Native language name";
	languages["direction"]["type"] = "int";
	languages["direction"]["not null"] = _TRUE;
	languages["direction"]["default"] = "0";
	languages["direction"]["description"] = "Direction of language (Left-to-Right = 0, Right-to-Left = 1)";
	languages["enabled"]["type"] = "int";
	languages["enabled"]["not null"] = _TRUE;
	languages["enabled"]["default"] = "0";
	languages["enabled"]["description"] = "Enabled flag (1 = Enabled, 0 = Disabled)";
	languages["domain"]["type"] = "varchar";
	languages["domain"]["length"] = "128";
	languages["domain"]["not null"] = _TRUE;
	languages["domain"]["default"] = "";
	languages["domain"]["description"] = "Domain to use for this language";
	languages["prefix"]["type"] = "varchar";
	languages["prefix"]["length"] = "128";
	languages["prefix"]["not null"] = _TRUE;
	languages["prefix"]["default"] = "";
	languages["prefix"]["description"] = "Path prefix to use for this language";
	languages["#primary key"]["language"] = "language";
	languages["#indexes"]["list"] = "name";
	schema.push_back( languages );

	map <string, map<string, string> > locales_source;
	locales_source["#spec"]["name"] = "locales_source";
	locales_source["#spec"]["description"] = "List of English source strings.";
	locales_source["lid"]["type"] = "serial";
	locales_source["lid"]["not null"] = _TRUE;
	locales_source["lid"]["description"] = "Unique identifier of this string.";
	locales_source["location"]["type"] = "varchar";
	locales_source["location"]["length"] = "255";
	locales_source["location"]["not null"] = _TRUE;
	locales_source["location"]["default"] = "";
	locales_source["location"]["description"] = "WSE path in case of online discovered translations or file path in case of imported strings.";
	locales_source["textgroup"]["type"] = "varchar";
	locales_source["textgroup"]["length"] = "255";
	locales_source["textgroup"]["not null"] = _TRUE;
	locales_source["textgroup"]["default"] = "default";
	locales_source["textgroup"]["description"] = "A model defined group of translations, see hook_locale().";
	locales_source["source"]["type"] = "text";
	locales_source["source"]["mysql_type"] = "blob";
	locales_source["source"]["not null"] = _TRUE;
	locales_source["source"]["description"] = "The original string in English.";
	locales_source["version"]["type"] = "varchar";
	locales_source["version"]["length"] = "20";
	locales_source["version"]["not null"] = _TRUE;
	locales_source["version"]["default"] = "none";
	locales_source["version"]["description"] = "Version of BinaryTiers, where the string was last used (for locales optimization).";
	locales_source["#primary key"]["lid"] = "lid";
	locales_source["#indexes"]["source"] = "source(30)";
	schema.push_back( locales_source );

	map <string, map<string, string> > locales_target;
	locales_target["#spec"]["name"] = "locales_target";
	locales_target["#spec"]["description"] = "Stores translated versions of strings.";
	locales_target["lid"]["type"] = "int";
	locales_target["lid"]["not null"] = _TRUE;
	locales_target["lid"]["default"] = "0";
	locales_target["lid"]["description"] = "Source string ID. References {locales_source}.lid.";
	locales_target["translation"]["type"] = "text";
	locales_target["translation"]["mysql_type"] = "blob";
	locales_target["translation"]["not null"] = _TRUE;
	locales_target["translation"]["description"] = "Translation string value in this language.";
	locales_target["language"]["type"] = "varchar";
	locales_target["language"]["length"] = "12";
	locales_target["language"]["not null"] = _TRUE;
	locales_target["language"]["default"] = "";
	locales_target["language"]["description"] = "Language code. References {languages}.language.";
	locales_target["plid"]["type"] = "int";
	locales_target["plid"]["not null"] = _TRUE; // This should be NULL for no referenced string, not zero.
	locales_target["plid"]["default"] = "0";
	locales_target["plid"]["description"] = "Parent lid (lid of the previous string in the plural chain) in case of plural strings. References {locales_source}.lid.";
	locales_target["plural"]["type"] = "int";
	locales_target["plural"]["not null"] = _TRUE;
	locales_target["plural"]["default"] = "0";
	locales_target["plural"]["description"] = "Plural index number in case of plural strings.";
	locales_target["#primary key"]["language"] = "language";
	locales_target["#primary key"]["lid"] = "lid";
	locales_target["#primary key"]["plural"] = "plural";
	locales_target["#indexes"]["lid"] = "lid";
	locales_target["#indexes"]["plid"] = "plid";
	locales_target["#indexes"]["plural"] = "plural";
	schema.push_back( locales_target );
}

void locale_controllers()
{
	map <string, string> item;

	item["path"] = "admin/language";
	item["title"] = "Languages";
	item["callback"] = _MAP_FORM( locale_admin_language );
	item["access arguments"] = "administer languages";
	item["parent tab"] = "admin/language";
	poke( controllers, item );

	item["path"] = "admin/language/add";
	item["title"] = "Add language";
	item["weight"] = "2";
	item["callback"] = _MAP_FORM( locale_add_language );
	item["type"] = str( MENU_LOCAL_TASK );
	item["access arguments"] = "administer languages";
	item["parent tab"] = "admin/language";
	poke( controllers, item );

	item["path"] = "admin/language/edit/%";
	item["title"] = "Edit language";
	item["callback"] = _MAP_FORM( locale_edit_language );
	item["type"] = str( MENU_LOCAL_TASK );
	item["access arguments"] = "administer languages";
	item["parent"] = "admin/language";
	poke( controllers, item );

	item["path"] = "admin/language";
	item["title"] = "List";
	item["weight"] = "1";
	item["type"] = str( MENU_LOCAL_TASK );
	item["callback"] = _MAP_FORM( locale_admin_language );
	item["access arguments"] = "administer languages";
	item["parent tab"] = "admin/language";
	poke( controllers, item );

	item["path"] = "admin/translate";
	item["title"] = "Translate";
	item["callback"] = _MAP( locale_admin_translate );
	item["access arguments"] = "administer translate";
	item["parent tab"] = "admin/translate";
	poke( controllers, item );

	item["path"] = "admin/translate/search";
	item["title"] = "Search";
	item["weight"] = "2";
	item["callback"] = _MAP( locale_translate_search );
	item["type"] = str( MENU_LOCAL_TASK );
	item["access arguments"] = "administer translate";
	item["parent tab"] = "admin/translate";
	poke( controllers, item );

	item["path"] = "admin/translate";
	item["title"] = "Overview";
	item["weight"] = "1";
	item["type"] = str( MENU_LOCAL_TASK );
	item["callback"] = _MAP( locale_admin_translate );
	item["access arguments"] = "administer translate";
	item["parent tab"] = "admin/translate";
	poke( controllers, item );

	item["path"] = "admin/translate/edit/%";
	item["title"] = "Edit String";
	item["callback"] = _MAP_FORM( locale_translate_edit );
	item["access arguments"] = "administer translate";
	item["type"] = str( MENU_LOCAL_TASK );
	item["parent"] = "admin/translate";
	poke( controllers, item );

	item["path"] = "admin/translate/delete/%";
	item["title"] = "Edit String";
	item["callback"] = _MAP( locale_translate_delete );
	item["access arguments"] = "administer translate";
	item["type"] = str( MENU_LOCAL_TASK );
	poke( controllers, item );

	item["path"] = "locale/change/%";
	item["title"] = "Edit String";
	item["callback"] = _MAP( locale_change );
	item["type"] = str( MENU_LOCAL_TASK );
	poke( controllers, item );

	item["path"] = "admin/translate/csv";
	item["title"] = "Profile Groups";
	item["callback"] = _MAP( locale_download );
	item["access arguments"] = "administer translate";
	item["type"] = str( MENU_LOCAL_TASK );	
	item["parent"] = "admin";
	poke( controllers, item );
}

string locale_admin_language()
{
	map <string, map<string,string> > form;
	vector <map <string, string> > items;
	
	string out = "Admin";

	set_page_title("Languages");

	form["languages"]["#type"] = "table";
	form["languages"]["#header"] = "Enabled,Code,English name,Native name,Direction,Default,Weight,Operations";
	form["languages"]["#weight"] = "2";

	string language_default = variable_get("language_default");

	if(DB_TYPE==1)
	{
		map <string, string> item;
		REDIS_RES *rr = redis_query_fields("SORT languages BY nosort", "GET languages:*->", "#language,name,native,direction,domain,prefix,enabled");
		while( redis_fetch_fields( rr, item ) )
			items.push_back( item );
	}
	if(DB_TYPE==2)
	{
		map <string, string> item;
		MYSQL_RES *result = db_querya("SELECT * FROM languages");
		while( db_fetch( result, item) )
			items.push_back( item );
	}
	
	for(size_t i = 0; i< items.size(); i++)
	{    
		form["a"+str(i)]["#type"] = "checkbox";
		form["a"+str(i)]["#table"] = "languages";
		form["a"+str(i)]["#checked"] = items[i]["enabled"];
		form["a"+str(i)]["#value"] = "1";

		form["b"+str(i)]["#type"] = "markup";
		form["b"+str(i)]["#table"] = "languages";
		form["b"+str(i)]["#value"] = items[i]["language"];

		form["c"+str(i)]["#type"] = "markup";
		form["c"+str(i)]["#table"] = "languages";
		form["c"+str(i)]["#value"] = "<b>" + items[i]["name"] + "</b>";

		form["d"+str(i)]["#type"] = "markup";
		form["d"+str(i)]["#table"] = "languages";
		form["d"+str(i)]["#value"] = items[i]["native"];

		form["e"+str(i)]["#type"] = "markup";
		form["e"+str(i)]["#table"] = "languages";
		form["e"+str(i)]["#value"] = (items[i]["direction"] == LANGUAGE_RTL) ? "Right to left" : "Left to right";

		form["f"+str(i)]["#type"] = "radio";
		form["f"+str(i)]["#name"] = "default_language";
		form["f"+str(i)]["#table"] = "languages";
		form["f"+str(i)]["#value"] = items[i]["language"];
		form["f"+str(i)]["#checked"] = (language_default == items[i]["language"]) ? _TRUE : _FALSE;

		form["g"+str(i)]["#type"] = "markup";
		form["g"+str(i)]["#table"] = "languages";
		form["g"+str(i)]["#value"] = items[i]["weight"];

		form["h"+str(i)]["#type"] = "markup";
		form["h"+str(i)]["#table"] = "languages";
		form["h"+str(i)]["#value"] = "<a href=\""+url("admin/language/edit/"+items[i]["language"])+"\">edit</a>";

		form["languages"]["#row"+str(i)] = "a"+str(i)+",b"+str(i)+",c"+str(i)+",d"+str(i)+",e"+str(i)+",f"+str(i)+",g"+str(i)+",h"+str(i);
	}

	form["submit"]["#type"] = "submit";
	form["submit"]["#value"] = "Save configuration";
	form["submit"]["#weight"] = "13";

	return get_form( "locale_admin_language", form );
}

bool locale_admin_language_validate()
{
	return true;
}

string locale_admin_language_submit()
{
	int i = 0;

	if(DB_TYPE==1)
	{
		string langcode;
		REDIS_RES *result = redis_query("SMEMBERS languages");
		while( redis_fetch( result, langcode ) )
		{
			redis_command("HSET languages:%s enabled %d", langcode.c_str(), intval( _POST["a"+str(i)] ) );
			i++;
		}
	}
	if(DB_TYPE==2)
	{
		map <string, string> item;
		MYSQL_RES *result = db_querya("SELECT * FROM languages");
		while( db_fetch( result, item) )
		{
			db_querya("UPDATE languages SET enabled = %d WHERE language = '%s'", num( _POST["a"+str(i)] ).c_str(), item["language"].c_str() );
			i++;
		}
	}
	
	variable_set("language_default", _POST["default_language"] );

	redirect( url("admin/language") );
	return "";
}

void locale_form( map <string, map<string,string> > &form )
{
	form["langcode"]["#type"] = "textfield";
	form["langcode"]["#title"] = "Language code";
	form["langcode"]["#size"] = "12";
	form["langcode"]["#maxlength"] = "60";
	form["langcode"]["#required"] = _TRUE;
	form["langcode"]["#description"] = "<a href=\"http://www.ietf.org/rfc/rfc4646.txt\">RFC 4646</a> compliant language identifier. Language codes typically use a country code, and optionally, a script or regional variant name. <em>Examples: \"en\", \"en-us\" and \"zh-hant\".</em>";
	form["langcode"]["#weight"] = "1";
	
	form["name"]["#type"] = "textfield";
	form["name"]["#title"] = "Language name in English";
	form["name"]["#maxlength"] = "64";
	form["name"]["#required"] = _TRUE;
	form["name"]["#description"] = "Name of the language in English. Will be available for translation in all languages.";
	form["name"]["#weight"] = "2";

	form["native"]["#type"] = "textfield";
	form["native"]["#title"] = "Native language name";
	form["native"]["#maxlength"] = "64";
	form["native"]["required"] = _TRUE;
	form["native"]["#description"] = "Name of the language in the language being added.";
	form["native"]["#weight"] = "3";
	
	form["prefix"]["#type"] = "textfield";
	form["prefix"]["#title"] = "Path prefix";
	form["prefix"]["#maxlength"] = "64";
	form["prefix"]["#description"] = "Language code or other custom string for pattern matching within the path. With language negotiation set to <em>Path prefix only</em> or <em>Path prefix with language fallback</em>, this site is presented in this language when the Path prefix value matches an element in the path. For the default language, this value may be left blank. <strong>Modifying this value will break existing URLs and should be used with caution in a production environment.</strong> <em>Example: Specifying \"deutsch\" as the path prefix for German results in URLs in the form \"www.example.com/deutsch/node\".</em>";
	form["prefix"]["#weight"] = "4";

	form["domain"]["#type"] = "textfield";
	form["domain"]["#title"] = "Language domain";
	form["domain"]["#maxlength"] = "128";
	form["domain"]["#description"] = "Language-specific URL, with protocol. With language negotiation set to <em>Domain name only</em>, the site is presented in this language when the URL accessing the site references this domain. For the default language, this value may be left blank. <strong>This value must include a protocol as part of the string.</strong> <em>Example: Specifying \"http://example.de\" or \"http://de.example.com\" as language domains for German results in URLs in the forms \"http://example.de/node\" and \"http://de.example.com/node\", respectively.</em>";
	form["domain"]["#weight"] = "5";

	form["direction"]["#type"] = "radios";
	form["direction"]["#title"] = "Direction";
	form["direction"]["#required"] = _TRUE;
	form["direction"]["#value"] = "0";
	form["direction"]["#description"] = "Direction that text in this language is presented.";
	form["direction"]["#options"] = "Left to right,Right to left";
	form["direction"]["#weight"] = "6";
}

string locale_edit_language()
{
	map <string, map<string,string> > form;
	map <string, string> item;

	string langcode = arg(3);

	locale_form( form );

	if(DB_TYPE==1)
	{
		redis_multi( item, "HGETALL languages:%s", langcode.c_str() );
	}
	if(DB_TYPE==2)
	{
		db_fetch( db_querya("SELECT * FROM languages WHERE language = '%s'", langcode.c_str() ) , item );
	}

	form["langcode"]["#value"] = langcode;
	form["langcode"]["#required"] = _FALSE;
	form["langcode"]["#disabled"] = _TRUE;
	form["name"]["#value"] = item["name"];
	form["native"]["#value"] = item["native"];
	form["prefix"]["#value"] = item["prefix"];
	form["domain"]["#value"] = item["domain"];
	form["direction"]["#value"] = item["direction"];
	
	form["submit"]["#type"] = "submit";
	form["submit"]["#value"] = "Save";
	form["submit"]["#weight"] = "13";

	form["delete"]["#type"] = "submit";
	form["delete"]["#value"] = "Delete";
	form["delete"]["#weight"] = "14";

	return get_form( "locale_add_language", form );
}

bool locale_edit_language_validate()
{
	return true;
}

string locale_edit_language_submit()
{
	string langcode = arg(3);

	if(_POST["op"]=="Delete")
	{
		if(DB_TYPE==1)
		{
			redis_command("SREM languages %s", langcode.c_str() );
			redis_command("DEL languages:%s", langcode.c_str() );
		}
		if(DB_TYPE==2)
		{
			db_querya("DELETE FROM languages WHERE language = '%s'", langcode.c_str() );
		}
	}

	if(_POST["op"]=="Save")
	{
		if(DB_TYPE==1)
		{
			redis_command_fields( redis_arg("HMSET languages:%s", langcode.c_str() ), "", "name,native,direction,domain,prefix,enabled", "%s,%s,%d,%s,%s,%d",
				_POST["name"].c_str(),
				_POST["native"].c_str(),
				intval(_POST["direction"]),
				_POST["domain"].c_str(),
				_POST["prefix"].c_str() );
		}		

		if(DB_TYPE==2)
		{
			db_querya("UPDATE languages SET name='%s', native='%s', direction=%d, domain='%s', prefix='%s' WHERE language='%s'", 
				_POST["name"].c_str(),
				_POST["native"].c_str(),
				_POST["direction"].c_str(),
				_POST["domain"].c_str(),
				_POST["prefix"].c_str(),
				langcode.c_str() );
		}
	}

	redirect( url("admin/language") );
	return "";
}

string locale_add_language()
{
	map <string, map<string,string> > form;

	locale_form( form );
	
	form["submit"]["#type"] = "submit";
	form["submit"]["#value"] = "Add language";
	form["submit"]["#weight"] = "13";

	return get_form( "locale_add_language", form );
}

bool locale_add_language_validate()
{
	return true;
}

string locale_add_language_submit()
{
	string langcode = strtolower( _POST["langcode"] );
	if(DB_TYPE==1)
	{
		redis_command("SADD languages %s", langcode.c_str() );
		redis_command_fields( redis_arg("HMSET languages:%s", langcode.c_str() ), "", "name,native,direction,domain,prefix,enabled", "%s,%s,%d,%s,%s,%d",
			_POST["name"].c_str(),
			_POST["native"].c_str(),
			intval(_POST["direction"]),
			_POST["domain"].c_str(),
			_POST["prefix"].c_str(),
			true );
	}
	if(DB_TYPE==2)
	{
		db_querya("INSERT INTO languages (language, name, native, direction, domain, prefix, enabled) VALUES ('%s', '%s', '%s', %d, '%s', '%s', %d)",
			langcode.c_str(),
			_POST["name"].c_str(),
			_POST["native"].c_str(),
			_POST["direction"].c_str(),
			_POST["domain"].c_str(),
			_POST["prefix"].c_str(),
			_TRUE );
	}

	redirect( url("admin/language") );

	return "";
}

string locale_locale( string source, string langcode )
{
	map <string, string> data;

	if(DB_TYPE==1)
	{
		if (!isset2( locales_t, langcode) )
		{
			REDIS_RES *result = redis_query_fields("SORT locales BY nosort", "GET locales:*->", "#lid,source,"+ langcode );
			while( redis_fetch_fields( result, data ) ) {
				locales_t[ langcode ][ data["source"] ] = !isset( data[ langcode ] ) ? _TRUE : data[ langcode ];	
			}
		}
		if( !isset(locales_t[langcode][source] ) ) {
			int lid = redis_int("INCR locales:ids");
			redis_command("SADD locales %d", lid );
			redis_command("HMSET locales:%d source %s location %s textgroup %s", lid, source.c_str(), _GET["q"].c_str(), "default" );
			locales_t[langcode][source] = _TRUE;
		}
	}
	if(DB_TYPE==2)
	{
		map <string, string> translation;
		MYSQL_RES *result;

		if (!isset2( locales_t, langcode) )
		{
			result = db_querya("SELECT s.source, t.translation, t.language FROM locales_source s LEFT JOIN locales_target t ON s.lid = t.lid AND t.language = '%s' WHERE s.textgroup = 'default' AND LENGTH(s.source) < 120", langcode.c_str() );
			while ( db_fetch( result, data ) )
			{
				locales_t[ langcode ][ data["source"] ] = !isset( data["translation"] ) ? _TRUE : data["translation"];
			}
		}

		if( !db_fetch( db_querya("SELECT s.lid, t.translation, s.version FROM locales_source s LEFT JOIN locales_target t ON s.lid = t.lid AND t.language = '%s' WHERE s.source = '%s' AND s.textgroup = 'default'", langcode.c_str(), source.c_str() ), translation ) )
		{
			db_querya("INSERT INTO locales_source (location, source, textgroup) VALUES ('%s', '%s', 'default')", _GET["q"].c_str(), source.c_str() );
			locales_t[langcode][source] = _TRUE;
		}
	}

	return (locales_t[langcode][source] == _TRUE ) ? source : locales_t[langcode][source];
}

void locale_upload()
{
	map <string, string> item;
	char separator = ';';

	vector <string> pieces;
	string cvs = file_get_contents( _FILES["csv"]["tempname"] );

	size_t ini = cvs.find("Code");
	cvs = cvs.substr(ini);
	explode( pieces, cvs, '\n');

	vector <string> line;
	vector <string> header;
	explode( header, pieces[0], separator);
	for(size_t i=0; i<header.size(); i++) {
		trim( header[i] );
	}

	if(DB_TYPE==1)
	{
		map <string, string> lang;
		REDIS_RES *result = redis_query_fields("SORT languages BY nosort", "GET languages:*->", "#language,name");
		while( redis_fetch_fields( result, item ) ) {
			lang[ item["name"] ] = item["language"];
		}

		for(size_t i=1; i<pieces.size(); i++)
		{
			explode( line, pieces[i], separator);

			for(size_t j=3; j<line.size(); j++)
			{				
				trim( line[j] );
				
				if( isset( line[j] ) )
				{
					redis_command("HSET locales:%d %s %s", intval(line[0]), lang[ header[j] ].c_str(), line[j].c_str() );
					redis_command("SADD locales:count:%s %d", lang[ header[j] ].c_str(), intval(line[0]) );
				}
				else
				{
					redis_command("HDEL locales:%d %s", intval(line[0]), lang[ header[j] ].c_str() );
					redis_command("SREM locales:count:%s %d", lang[ header[j] ].c_str(), intval(line[0]) );
				}
			}

			line.clear();
		}

	}

	if(DB_TYPE==2)
	{
		map <string, string> lang;
		MYSQL_RES *result = db_querya("SELECT * FROM languages");
		while ( db_fetch( result, item ) ) {
			lang[ item["name"] ] = item["language"];
		}
				
		for(size_t i=1; i<pieces.size(); i++)
		{
			explode( line, pieces[i], separator);

			for(size_t j=3; j<line.size(); j++)
			{				
				trim( line[j] );
				
				string lid = db_result( db_querya("SELECT lid FROM locales_target WHERE lid=%d AND language='%s'", line[0].c_str(), lang[ header[j] ].c_str() ));
				if( isset( line[j] ) )
				{
					if( isset( lid ) ) {
						//db_querya("UPDATE locales_target SET translation='%s' WHERE lid=%d AND language='%s'", 
							//line[j].c_str(), line[0].c_str(), lang[ header[j] ].c_str() );
					}
					else {
						db_querya("INSERT INTO locales_target (lid, translation, language) VALUES (%d, '%s', '%s')", 
							line[0].c_str(), line[j].c_str(), lang[ header[j] ].c_str() );
					}
				}
				else
				{
					if( isset( lid ) ) {
						//db_querya("DELETE FROM locales_target WHERE lid=%d AND language='%s'", line[0].c_str(), lang[ header[j] ].c_str() );
					}
				}
			}

			line.clear();
		}
	}
}

string locale_admin_translate()
{
	string out;
	map <string, string> item;

	char separator = ';';

	if( isset( _FILES["csv"]["filename"] ) ) {
		locale_upload();
	}

	if(DB_TYPE==1)
	{
		string total = redis_str("SCARD locales");
		REDIS_RES *result = redis_query_fields("SORT languages BY nosort", "GET languages:*->", "#language,name,native,direction,domain,prefix,enabled");
		out += "<div style=\"margin:10px\"><table>";
		while( redis_fetch_fields( result, item ) )
		{
			item["count"] = redis_str("SCARD locales:count:%s", item["language"].c_str() );
			out += "<tr><td><b>"+item["name"]+"</b></td><td>"+item["count"]+" phrases translated of "+total+"</td></tr>";
		}
		out += "</table></div>";
	}
	if(DB_TYPE==2)
	{
		MYSQL_RES *result;
		string total = db_result( db_querya("SELECT count(*) as count FROM locales_source") );
		out += "<div style=\"margin:10px\"><table>";
		result = db_querya("SELECT count(*) as count, l.name FROM locales_target t LEFT JOIN languages l ON l.language=t.language GROUP BY l.language" );
		while ( db_fetch( result, item ) )
		{
			out += "<tr><td><b>"+item["name"]+"</b></td><td>"+item["count"]+" phrases translated of "+total+"</td></tr>";
		}
		out += "</table></div>";
	}

	out += "<div style=\"margin:10px\"><a href=\""+url("admin/translate/csv")+"\">Download CSV Database</a></div>";

	if( user_access( "can upload csv" ) )
	{
	out += "<form method=\"post\" enctype=\"multipart/form-data\">\n\
			<div style=\"margin:10px\">\n\
			<b>Upload CSV Database</b><br>\n\
			Select File :<input type=\"file\" name=\"csv\" /><br><br>\n\
				<input type=\"submit\" id=\"upload\" name=\"upload\" value=\"Upload\" />\n\
			</div>\n\
			</form>\n";
	}

	return out;
}

string locale_download()
{
	string out;
	
	string separator = ";";
	string line = "\n";
	
	out += "Code";
	out += separator+"Location";
	out += separator+"Source";

	vector <string> lang;

	if(DB_TYPE==1)
	{
		map <string, string> item;
		redis_vector( lang, "SMEMBERS languages" );
		for(size_t i=0; i<lang.size(); i++) {
			out += separator+redis_str("HGET languages:%s name", lang[i].c_str());
		}
		REDIS_RES * result = redis_query_fields("SORT locales", "GET locales:*->", "#lid,source,location,"+ implode(lang, ",") );
		while( redis_fetch_fields( result, item ) ) {
			out += line+item["lid"];
			out += separator+item["location"];
			out += separator+item["source"];

			for(size_t i=0; i<lang.size(); i++)	{
				out += separator+item[ lang[i] ];
			}

			item.clear();
		}
	}
	if(DB_TYPE==2)
	{	
		map <string, string> item, item2;
		MYSQL_RES *result, *result2;
		result = db_querya( "SELECT * FROM languages" );
		while ( db_fetch( result, item ) )
		{
			lang.push_back( item["language"] );
			out += separator+item["name"];
		}
		
		result = db_querya( "SELECT * FROM locales_source ORDER BY lid" );
		while ( db_fetch( result, item ) )
		{
			result2 = db_querya( "SELECT * FROM locales_target WHERE lid=%d", item["lid"].c_str() );
			while ( db_fetch( result2, item2 ) ) {
				item[ item2["language"] ] = item2["translation"];
			}

			out += line+item["lid"];
			out += separator+item["location"];
			out += separator+item["source"];

			for(size_t i=0; i<lang.size(); i++)	{
				out += separator+item[ lang[i] ];
			}

			item.clear();
		}
	}

	http_header("Content-Disposition:attachment;filename=translation.csv");

	string bom;
	unsigned char c;
	c = 0xEF;
	bom += c;
	c = 0xBB;
	bom += c;
	c = 0xBF;
	bom += c;	
	out = bom + out;

	print (out);
	return "";
}

string locale_change()
{
	string language = arg(2);
	string dest = _GET["dst"];
	setcookie("BTLANG", language, str( time()+cookie_lifetime ), "/", getenv("HTTP_HOST") ? string(getenv("HTTP_HOST")) : "");
	redirect( url( dest ) );

	if(DB_TYPE==1)
		redis_command("HSET users:%d language %s", intval(user["uid"]), language.c_str() );
	if(DB_TYPE==2)
		db_querya("UPDATE users SET language='%s' WHERE uid=%d", language.c_str(), user["uid"].c_str() );
	
	return "";
}

string _locale_translate_seek( string word ) 
{
	string output = "";
	vector <map <string,string> > arr;

	if(DB_TYPE==1)
	{
		REDIS_RES *result;
		map <string, string> item;
		vector <string> languages;
		redis_vector( languages, "SMEMBERS languages" );
	
		if( !redis_int("EXISTS translate_seek:%s", word.c_str()) )
		{
			result = redis_query_fields("SORT locales BY nosort", "GET locales:*->", "#lid,source,"+ implode(languages, ",") );
			while( redis_fetch_fields( result, item ) ) {
				if( item["source"].find( word ) != string::npos )
					redis_command("SADD translate_seek:%s %s", word.c_str(), item["lid"].c_str() );
				for( size_t i=0; i<languages.size(); i++ ) {
					if( item[ languages[i] ].find( word ) != string::npos )
						redis_command("SADD translate_seek:%s %s", word.c_str(), item["lid"].c_str() );
				}
			}
			redis_command("EXPIRE translate_seek:%s 100", word.c_str());
		}
		
		result = redis_pager_fields(redis_arg("SORT translate_seek:%s", word.c_str()), "GET locales:*->", "#lid,source,location,textgroup,"+ implode(languages, ","), 150);
		while( redis_fetch_fields( result, item ) ) {
			for( size_t i=0; i<languages.size(); i++ ) {
				if(isset( item[ languages[i] ] ) )
					item["languages"] += languages[i]+",";
			}
			arr.push_back( item );
			item.clear();
		}
	}

	if(DB_TYPE==2)
	{
		MYSQL_RES *result;
		map <string, string> item;
		string sql, sqlcount;
	  
		sql = "SELECT s.lid, s.source, s.location, s.textgroup, GROUP_CONCAT(t.language) AS languages FROM locales_source s LEFT JOIN locales_target t ON s.lid = t.lid WHERE (s.source LIKE '%%%%%s%%%%' OR t.translation LIKE '%%%%%s%%%%') GROUP BY s.lid";
		sql = sql_parse( sql.c_str(), word.c_str(), word.c_str() );

		sqlcount = "SELECT COUNT(DISTINCT(s.lid)) FROM locales_source s LEFT JOIN locales_target t ON s.lid = t.lid WHERE (s.source LIKE '%%%%%s%%%%' OR t.translation LIKE '%%%%%s%%%%')";
		sqlcount = sql_parse( sqlcount.c_str(), word.c_str(), word.c_str() );

		result = pager_query( sql, sqlcount, 150 );
		while ( db_fetch( result, item ) ) {
			arr.push_back( item );
		}
	}

	vector <vector <map <string, string> > > rows;
	vector <map <string, string> > row;
	map <string, string> cell;
	vector <string> header;

	header.push_back("Text group");
	header.push_back("String");
	header.push_back("Languages");
	header.push_back("Operations");

	map <string, string> options;
	options["query"] = "page="+_GET["page"]+pager_get_querystring();
	
	for( size_t i=0; i<arr.size(); i++ )
	{	
		trim( arr[i]["languages"], ',');

		cell["data"] = arr[i]["textgroup"];
		poke(row, cell);
		cell["data"] = arr[i]["source"] + "<br><small>" + arr[i]["location"] + "</small>";
		poke(row, cell);
		cell["data"] = arr[i]["languages"];
		poke(row, cell);
		cell["data"] = "<a href=\""+url("admin/translate/edit/"+arr[i]["lid"], &options )+"\">edit</a> <a href=\""+url("admin/translate/delete/"+arr[i]["lid"], &options )+"\">delete</a>";
		poke(row, cell);
		
		poke(rows, row);
	}
    
    if ( rows.size() ) 
	{
		output += theme_table( header, rows );
		output += theme_pager();
    }
    else 
	{
		output += "No strings found for your search.";
    }

	return output;
}

string locale_translate_search()
{
	string out, word;
	map <string, map<string,string> > form;

	if( isset( _POST["op"] ) )
	{
		word = _POST["string"];
		_GET["string"].clear();
		out = _locale_translate_seek( word );		
	}
	else
	{
		word = _GET["string"];
		out = _locale_translate_seek( word );
	}
	
	form["string"]["#type"] = "textfield";
    form["string"]["#title"] = "String contains";
	form["string"]["#value"] = word;
    form["string"]["#description"] = "Leave blank to show all strings. The search is case sensitive.";
	
	form["submit"]["#type"] = "submit";
	form["submit"]["#value"] = "Search";
	form["submit"]["#weight"] = "13";

	return out + get_form( "locale_translate_search", form );
}

string locale_translate_delete()
{
	string lid = arg(3);

	if(DB_TYPE==1)
	{
		string item;
		REDIS_RES *result = redis_query("SMEMBERS languages");
		while( redis_fetch( result, item ) ) { // Remove locale from all counters
			redis_command("SREM locales:count:%s %d", item.c_str(), intval(lid) );
		}
		result = redis_query("KEYS translate_seek:*");
		while( redis_fetch( result, item ) ) { // Remove locale from all volatile filters
			redis_command("SREM %s %d", item.c_str(), intval(lid) );
		}
		redis_command("SREM locales %d", intval(lid) );
		redis_command("DEL locales:%d", intval(lid) );
	}
	if(DB_TYPE==2)
	{
		db_querya("DELETE FROM locales_source WHERE lid = %d", lid.c_str() );
		db_querya("DELETE FROM locales_target WHERE lid = %d", lid.c_str() );
	}

	map <string, string> options;
	options["query"] = "page="+_GET["page"]+pager_get_querystring();

	redirect( url("admin/translate/search", &options) );
	return "";
}

string locale_translate_edit()
{
	map <string, map<string,string> > form;
	map <string, string> translation;
	map <string, string> language;
	map <string, string> source;
	
	bool found = false;
	string lid = arg(3);

	if(DB_TYPE==1) {
		if( redis_int("SISMEMBER locales %d", intval(lid)) ) {
			found = true;
		}
	}
	if(DB_TYPE==2) {
		if( db_fetch(db_querya("SELECT source, textgroup, location FROM locales_source WHERE lid = %d", lid.c_str() ), source ) ) {
			found = true;
		}
	}

	if( !found ) {
		map <string, string> options;
		options["query"] = "page="+_GET["page"]+pager_get_querystring();
		set_page_message("String not found.", "error");
		redirect( url("admin/translate/search", &options));
		return "";
	}

	if(DB_TYPE==1) 
	{
		REDIS_RES *result = redis_query_fields("SORT languages BY languages:*->name ALPHA", "GET languages:*->", "#language,name");
		while( redis_fetch_fields( result, language ) ) {
			string langcode = language["language"];
			form[langcode]["#type"] = "textarea";
			form[langcode]["#title"] = language["name"];
			form[langcode]["#rows"] = "1";
			form[langcode]["#value"] = redis_str("HGET locales:%d %s", intval(lid), langcode.c_str());
		}
		source["source"] = redis_str("HGET locales:%d source", intval(lid));
	}

	if(DB_TYPE==2)
	{
		MYSQL_RES *result = db_querya("SELECT * FROM languages ORDER BY name ASC");
		while ( db_fetch( result, language ) )
		{
			string langcode = language["language"];
			form[langcode]["#type"] = "textarea";
			form[langcode]["#title"] = language["name"];
			form[langcode]["#rows"] = "1";
		}

		result = db_querya("SELECT DISTINCT translation, language FROM locales_target WHERE lid = %d", lid.c_str() );
		while ( db_fetch( result, translation ) ) {
			form[translation["language"]]["#value"] = translation["translation"];
		}
	}

	form["source"]["#type"] = "markup";
	form["source"]["#value"] = "<div class=\"form-item\"><label>Original text: </label>"+source["source"]+"</div>";
	form["source"]["#weight"] = "-1";

	form["submit"]["#type"] = "submit";
	form["submit"]["#value"] = "Save translations";
	form["submit"]["#weight"] = "13";

	return get_form("locale_translate_edit", form );
}

bool locale_translate_edit_validate()
{
	return true;
}

vector <map <string, string> > * language_list()
{
	if(languages_t.size() == 0)
	{
		map <string, string> language;
		
		if(DB_TYPE==1)
		{
			string langcode;
			REDIS_RES *result = redis_query("SMEMBERS languages");
			while( redis_fetch( result, langcode ) ) {
				redis_multi( language, "HGETALL languages:%s", langcode.c_str() );
				language["language"] = langcode;
				languages_t.push_back( language );
			}
		}
		if(DB_TYPE==2)
		{
			MYSQL_RES *result;
			result = db_querya("SELECT * FROM languages ORDER BY weight ASC, name ASC");
			while ( db_fetch( result, language ) ) {
				languages_t.push_back( language );
			}
		}
	}

	return (&languages_t);
}

string locale_get_name( string langcode )
{
	vector <map <string, string> > *languages;

	languages = language_list();

	for(size_t i=0; i<languages->size(); i++)
	{
		if( (*languages)[i]["language"] == langcode ) return (*languages)[i]["name"];
	}

	return "";
}

string locale_translate_edit_submit()
{
	vector <map <string, string> > *languages;
	string lid = arg(3);

	languages = language_list();

	if(DB_TYPE==1)
	{
		for(size_t i=0; i<languages->size(); i++)
		{
			string key = (*languages)[i]["language"];
			string value = _POST[key];
			redis_command("HSET locales:%d %s %s", intval(lid), key.c_str(), value.c_str() );
			if( isset( value ) ) 
				redis_command("SADD locales:count:%s %d", key.c_str(), intval(lid) );
			else 
				redis_command("SREM locales:count:%s %d", key.c_str(), intval(lid) );
		}
	}
	if(DB_TYPE==2)
	{
		for(size_t i=0; i<languages->size(); i++)
		{
			string key = (*languages)[i]["language"];
			string value = _POST[key];
			string translation = db_result(db_querya("SELECT translation FROM locales_target WHERE lid = %d AND language = '%s'", lid.c_str(), key.c_str() ));
			if( isset( value ) )
			{
				// Only update or insert if we have a value to use.
				if( isset( translation ) )
				{
					db_querya("UPDATE locales_target SET translation = '%s' WHERE lid = %d AND language = '%s'", value.c_str(), lid.c_str(), key.c_str() );
				}
				else 
				{
					db_querya("INSERT INTO locales_target (lid, translation, language) VALUES (%d, '%s', '%s')", lid.c_str(), value.c_str(), key.c_str() );
				}
			}
			else if ( isset(translation) )
			{
				// Empty translation entered: remove existing entry from database.
				db_querya("DELETE FROM locales_target WHERE lid = %d AND language = '%s'", lid.c_str(), key.c_str() );
			}
		}
	}

	_POST.clear();
	map <string, string> options;
	options["query"] = "page="+_GET["page"]+pager_get_querystring();

	redirect( url("admin/translate/search", &options) );

	return "";
}
