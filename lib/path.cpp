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
#include "path.h"

void path_hooks()
{
	_MAP_DESC( path, "Allows users to rename URLs." );
	_MAP_VER ( path, "1.3.1" );
	_MAP_HOOK( path, schema );
	_MAP_HOOK( path, update );
	_MAP_HOOK( path, insert );
	_MAP_HOOK( path, load );
	_MAP_HOOK( path, form_alter );
}

void path_schema()
{
	map <string, map<string, string> > url_alias;

	url_alias["#spec"]["description"] = "A list of URL aliases for WSE paths; a user may visit either the source or destination path.";
	url_alias["#spec"]["name"] = "url_alias";
	url_alias["pid"]["description"] = "A unique path alias identifier.";
	url_alias["pid"]["type"] = "serial";
	url_alias["pid"]["unsigned"] = _TRUE;
	url_alias["pid"]["not null"] = _TRUE;
	url_alias["src"]["description"] = "The WSE path this alias is for; e.g. node/12.";
	url_alias["src"]["type"] = "varchar";
	url_alias["src"]["length"] = "255";
	url_alias["src"]["not null"] = _TRUE;
	url_alias["src"]["default"] = "";
	url_alias["dst"]["description"] = "The alias for this path; e.g. title-of-the-story.";
	url_alias["dst"]["type"] = "varchar";
	url_alias["dst"]["length"] = "255";
	url_alias["dst"]["not null"] = _TRUE;
	url_alias["dst"]["default"] = "";
	url_alias["language"]["description"] = "The language this alias is for; if blank, the alias will be used for unknown languages. Each WSE path can have an alias for each supported language.";
	url_alias["language"]["type"] = "varchar";
	url_alias["language"]["length"] = "12";
	url_alias["language"]["not null"] = _TRUE;
	url_alias["language"]["default"] = "";
	url_alias["#unique keys"]["dst_language"] = "dst,language";
	url_alias["#primary key"]["pid"] = "pid";
    url_alias["#indexes"]["src_language"] = "src,language";
    
	schema.push_back( url_alias );
}

void path_make_autopath( map <string, string> &node )
{
	if( node["type"] == "page" && isset( node["path"] ) )
	{
		string source = "node/"+node["nid"];
		string dest = node["path"];
		
		if(DB_TYPE==1)
		{
			redis_command("HSET url_alias:%s %s %s", node["language"].c_str(), source.c_str(), dest.c_str() );
		}
		if(DB_TYPE==2)
		{
			db_querya("DELETE FROM url_alias WHERE src='%s' AND language='%s'", source.c_str(), node["language"].c_str() );
			db_querya("INSERT INTO url_alias (src, dst, language) VALUES ('%s', '%s', '%s')", source.c_str(), dest.c_str(), node["language"].c_str() );
		}
	}
}

void path_load()
{
	string source;

	if( cur_node["type"] == "story" )
		source = "story/"+cur_node["nid"];
	else
		source = "node/"+cur_node["nid"];
	
	if(DB_TYPE==1) {
		cur_node["path"] = redis_str("HGET url_alias:%s %s", cur_node["language"].c_str(), source.c_str() );
	}
	if(DB_TYPE==2) {
		cur_node["path"] = db_result( db_querya("SELECT dst FROM url_alias WHERE src='%s' AND language='%s'", source.c_str(), cur_node["language"].c_str() ) );
	}
}

void path_update()
{
	map <string, string> node = cur_node;

	path_make_autopath( node );
}

void path_insert()
{
	map <string, string> node = cur_node;

	path_make_autopath( node );
}

void path_form_alter()
{
	if( cur_form_id == "add-page" ) {
		map <string, string> node;
		
		node = cur_form["#node"];

		cur_form["path_settings"]["#type"] = "fieldset";
		cur_form["path_settings"]["#title"] = "URL path settings";
		cur_form["path_settings"]["#collapsible"] = _TRUE;
		cur_form["path_settings"]["#collapsed"] = _TRUE;
		cur_form["path_settings"]["#weight"] = "21";

		cur_form["path"]["#type"] = "textfield";
		cur_form["path"]["#value"] = node["path"];
		cur_form["path"]["#size"] = "120";
		cur_form["path"]["#description"] = "Optionally specify an alternative URL by which this story or page can be accessed. Leave blank to let Binary Tiers generate the URL for you.";
		cur_form["path"]["#fieldset"] = "path_settings";
	}
}
