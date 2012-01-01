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
#include "translation.h"
#include "locale.h"

void translation_hooks()
{
	_MAP_DESC( translation, "Allows content to be translated into different languages." );
	_MAP_VER ( translation, "1.3.1" );
	_MAP_HOOK( translation, form_alter );
	_MAP_HOOK( translation, schema );
	_MAP_HOOK( translation, update );
	_MAP_HOOK( translation, insert );
}

void translation_schema()
{
	
}

void translation_save()
{
	map <string, string> node = cur_node;

	if(DB_TYPE==1)
	{
		redis_command("HSET node:%d language %s", intval(node["nid"]), node["language"].c_str() );
	}
	if(DB_TYPE==2)
	{
		db_querya("UPDATE node SET language='%s' WHERE nid=%d", node["language"].c_str(), node["nid"].c_str() );
	}
}

void translation_insert()
{
	translation_save();
}

void translation_update()
{
	translation_save();
}

void translation_form_alter()
{
	if( cur_form_id == "add-page" ) {

		if( cur_form["language"]["#allow"] == _FALSE ) return;

		map <string, string> node;

		node = cur_form["#node"];

		vector <pair <string, string> > options;
		vector <map <string, string> > * languages;
		languages = language_list();		

		options.push_back( make_pair("", "Language Neutral") );

		for(size_t i=0; i< languages->size(); i++)
		{
			options.push_back( make_pair((*languages)[i]["language"], (*languages)[i]["name"]) );
		}

		cur_form["language"]["#type"] = "select";
		cur_form["language"]["#title"] = "Language";
		cur_form["language"]["#options"] = serialize_array( options );
		cur_form["language"]["#weight"] = "1";
		cur_form["language"]["#value"] = node["language"];
	}
}
