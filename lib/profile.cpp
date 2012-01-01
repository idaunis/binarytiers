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
#include "profile.h"

map <string, string> profile_field;

void profile_hooks()
{
	_MAP_DESC( profile, "Supports configurable user profiles." );
	_MAP_VER ( profile, "1.3.1" );
	_MAP_HOOK( profile, schema );
	_MAP_HOOK( profile, controllers );
}

void profile_schema()
{
	map <string, map<string, string> > profile_fields;
	profile_fields["#spec"]["description"] = "Stores profile field information.";
	profile_fields["#spec"]["name"] = "profile_fields";
	profile_fields["fid"]["type"] = "serial";
	profile_fields["fid"]["not null"] = _TRUE;
	profile_fields["fid"]["description"] = "Primary Key: Unique profile field ID.";
	profile_fields["title"]["type"] = "varchar";
	profile_fields["title"]["length"] = "255";
	profile_fields["title"]["not null"] = _FALSE;
	profile_fields["title"]["description"] = "Title of the field shown to the end user.";
	profile_fields["name"]["type"] = "varchar";
	profile_fields["name"]["length"] = "128";
	profile_fields["name"]["not null"] = _TRUE;
	profile_fields["name"]["default"] = "";
	profile_fields["name"]["description"] = "Internal name of the field used in the form HTML and URLs.";
	profile_fields["explanation"]["type"] = "text";
	profile_fields["explanation"]["not null"] = _FALSE;
	profile_fields["explanation"]["description"] = "Explanation of the field to end users.";
	profile_fields["category"]["type"] = "varchar";
	profile_fields["category"]["length"] = "255";
	profile_fields["category"]["not null"] = _FALSE;
	profile_fields["category"]["description"] = "Profile category that the field will be grouped under.";
	profile_fields["page"]["type"] = "varchar";
	profile_fields["page"]["length"] = "255";
	profile_fields["page"]["not null"] = _FALSE;
	profile_fields["page"]["description"] = "Title of page used for browsing by the field's value";
	profile_fields["type"]["type"] = "varchar";
	profile_fields["type"]["length"] = "128";
	profile_fields["type"]["not null"] = _FALSE;
	profile_fields["type"]["description"] = "Type of form field.";
	profile_fields["weight"]["type"] = "int";
	profile_fields["weight"]["not null"] = _TRUE;
	profile_fields["weight"]["default"] = "0";
	profile_fields["weight"]["size"] = "tiny";
	profile_fields["weight"]["description"] = "Weight of field in relation to other profile fields.";
	profile_fields["required"]["type"] = "int";
	profile_fields["required"]["not null"] = _TRUE;
	profile_fields["required"]["default"] = "0";
	profile_fields["required"]["size"] = "tiny";
	profile_fields["required"]["description"] = "Whether the user is required to enter a value. (0 = no, 1 = yes)";
	profile_fields["register"]["type"] = "int";
	profile_fields["register"]["not null"] = _TRUE;
	profile_fields["register"]["default"] = "0";
	profile_fields["register"]["size"] = "tiny";
	profile_fields["register"]["description"] = "Whether the field is visible in the user registration form. (1 = yes, 0 = no)";
	profile_fields["visibility"]["type"] = "int";
	profile_fields["visibility"]["not null"] = _TRUE;
	profile_fields["visibility"]["default"] = "0";
	profile_fields["visibility"]["size"] = "tiny";
	profile_fields["visibility"]["description"] = "The level of visibility for the field. (0 = hidden, 1 = private, 2 = public on profile but not member list pages, 3 = public on profile and list pages)";
	profile_fields["autocomplete"]["type"] = "int";
	profile_fields["autocomplete"]["not null"] = _TRUE;
	profile_fields["autocomplete"]["default"] = "0";
	profile_fields["autocomplete"]["size"] = "tiny";
	profile_fields["autocomplete"]["description"] = "Whether form auto-completion is enabled. (0 = disabled, 1 = enabled)";
	profile_fields["options"]["type"] = "text";
	profile_fields["options"]["not null"] = _FALSE;
	profile_fields["options"]["description"] = "List of options to be used in a list selection field.";
	profile_fields["#indexes"]["category"] = "category";
	profile_fields["#unique keys"]["name"] = "name";
	profile_fields["#primary key"]["fid"] = "fid";
	schema.push_back( profile_fields );

	map <string, map<string, string> > profile_values;
	profile_values["#spec"]["name"] = "profile_values";
	profile_values["#spec"]["description"] = "Stores values for profile fields.";
	profile_values["fid"]["type"] = "int";
	profile_values["fid"]["unsigned"] = _TRUE;
	profile_values["fid"]["not null"] = _TRUE;
	profile_values["fid"]["default"] = "0";
	profile_values["fid"]["description"] = "The {profile_fields}.fid of the field.";
	profile_values["uid"]["type"] = "int";
	profile_values["uid"]["unsigned"] = _TRUE;
	profile_values["uid"]["not null"] = _TRUE;
	profile_values["uid"]["default"] = "0";
	profile_values["uid"]["description"] = "The {users}.uid of the profile user.";
	profile_values["value"]["type"] = "text";
	profile_values["value"]["not null"] = _FALSE;
	profile_values["value"]["description"] = "The value for the field.";
	profile_values["#primary key"]["uid"] = "uid";
	profile_values["#primary key"]["fid"] = "fid";
	profile_values["#indexes"]["fid"] = "fid";
	schema.push_back( profile_values );
}

void profile_controllers()
{
	map <string, string> item;

	item["path"] = "admin/profile";
	item["title"] = "Profiles";
	item["callback"] = _MAP( profile_admin );
	item["access callback"] = _MAP_ACCESS( user_access );
	item["access arguments"] = "administer users";
	item["parent tab"] = "admin/profile";
	poke( controllers, item );

	item["path"] = "admin/profile/add";
	item["title"] = "Add Field";
	item["type"] = str( MENU_CALLBACK );
	item["callback"] = _MAP_FORM( profile_edit );
	item["access callback"] = _MAP_ACCESS( user_access );
	item["access arguments"] = "administer users";
	poke( controllers, item );

	item["path"] = "admin/profile/edit/%";
	item["title"] = "Edit Field";
	item["type"] = str( MENU_CALLBACK );
	item["callback"] = _MAP_FORM( profile_edit );
	item["access callback"] = _MAP_ACCESS( user_access );
	item["access arguments"] = "administer users";
	poke( controllers, item );

	item["path"] = "admin/profile/delete/%";
	item["title"] = "Edit Field";
	item["type"] = str( MENU_CALLBACK );
	item["callback"] = _MAP( profile_delete );
	item["access callback"] = _MAP_ACCESS( user_access );
	item["access arguments"] = "administer users";
	poke( controllers, item );
}

void _profile_get_fields()
{
	if( profile_field.empty() )
	{
		map <string, string> item;
		
		if(DB_TYPE==1)
		{
			REDIS_RES *result = redis_query_fields("SORT profile_fields BY nosort", "GET profile_fields:*->", "#fid,name");
			while( redis_fetch_fields( result, item ) )
			{
				profile_field[ item["name"] ] = item["fid"];
			}
		}
		if(DB_TYPE==2)
		{
			MYSQL_RES *result = db_querya("SELECT name, fid FROM profile_fields");
			while( db_fetch( result, item ) )
			{
				profile_field[ item["name"] ] = item["fid"];
			}
		}
	}
}

string profile_get_field( string field )
{
	_profile_get_fields();
	return profile_field[ field ];
}

void profile_save_value( string field, string value, string uid )
{
	if( !isset(uid) ) uid = user["uid"];
	_profile_get_fields();
	
	if(DB_TYPE==1)
	{
		redis_command("HSET profile_values:%d %s %s", intval(uid), profile_field[ field ].c_str(), value.c_str() );
	}
	if(DB_TYPE==2)
	{
		db_querya("DELETE FROM profile_values WHERE fid = %d AND uid = %d", profile_field[ field ].c_str(), uid.c_str() );
		db_querya("INSERT INTO profile_values (fid, uid, value) VALUES (%d, %d, '%s')", profile_field[ field ].c_str(), uid.c_str(), value.c_str() );
	}
}

string profile_get_value( string field, string uid )
{
	if( !isset(uid) ) uid = user["uid"];
	_profile_get_fields();

	if(DB_TYPE==1)
	{
		return redis_str("HGET profile_values:%d %s", intval(uid), profile_field[ field ].c_str() );
	}
	if(DB_TYPE==2)
	{
		return db_result( db_querya("SELECT v.value FROM profile_values v WHERE v.fid=%d AND uid=%d", profile_field[ field ].c_str(), uid.c_str() ) );
	}
	return "";
}

void profile_save_profile(map <string, string> &account)
{
	_profile_get_fields();

	if(DB_TYPE==1)
	{
		for( map <string, string>::iterator i = profile_field.begin(), end = profile_field.end(); i != end; i++ ) {
			redis_command("HSET profile_values:%d %s %s", intval(account["uid"]), i->second.c_str(), account[ i->first ].c_str() );
		}
	}
	if(DB_TYPE==2)
	{
		for( map <string, string>::iterator i = profile_field.begin(), end = profile_field.end(); i != end; i++ ) {
			db_querya("DELETE FROM profile_values WHERE fid = %d AND uid = %d", i->second.c_str(), account["uid"].c_str() );
			db_querya("INSERT INTO profile_values (fid, uid, value) VALUES (%d, %d, '%s')", i->second.c_str(), account["uid"].c_str(), account[ i->first ].c_str() );
		}
	}
}

void profile_load_profile(map <string, string> &account)
{
	map <string, string> field;
	
	if(DB_TYPE==1)
	{
		_profile_get_fields();
		redis_multi(field, "HGETALL profile_values:%d", intval(account["uid"]) );
		for( map <string, string>::iterator i = profile_field.begin(), end = profile_field.end(); i != end; i++ ) {
			account[ i->first ] = field[ i->second ];
		}
	}

	if(DB_TYPE==2)
	{
		MYSQL_RES *result;	
		result = db_querya("SELECT f.name, f.type, v.value FROM profile_fields f INNER JOIN profile_values v ON f.fid = v.fid WHERE uid = %s", account["uid"].c_str());
		while( db_fetch( result, field ) )
		{
			if (!isset(account[field["name"]]) )
			{
			  account[field["name"]] = field["value"];
			}
		}
	}
}

string profile_admin()
{
	string out;
	map <string, string> item;
	map <string, map<string,string> > form;

	form["profiles"]["#type"] = "table";
	form["profiles"]["#header"] = "Title,Name,Type,Operations";
	form["profiles"]["#weight"] = "1";
	
	size_t i = 0;

	if(DB_TYPE==1)
	{
		REDIS_RES *result = redis_query_fields("SORT profile_fields BY profile_fields:*->title ALPHA", "GET profile_fields:*->", "#fid,title,name,explanation");
		while( redis_fetch_fields( result, item ) )
		{
			form["A"+str(i)]["#type"] = "markup";
			form["A"+str(i)]["#value"] = item["title"];
			form["A"+str(i)]["#table"] = "profiles";

			form["B"+str(i)]["#type"] = "markup";
			form["B"+str(i)]["#value"] = item["name"];
			form["B"+str(i)]["#table"] = "profiles";

			form["C"+str(i)]["#type"] = "markup";
			form["C"+str(i)]["#value"] = item["type"];
			form["C"+str(i)]["#table"] = "profiles";

			form["D"+str(i)]["#type"] = "markup";
			form["D"+str(i)]["#value"] = "<a href=\""+url("admin/profile/edit/"+item["fid"])+"\">Edit</a> <a href=\""+url("admin/profile/delete/"+item["fid"])+"\">Delete</a>";
			form["D"+str(i)]["#table"] = "profiles";

			form["profiles"]["#row"+str(i)] = "A"+str(i)+",B"+str(i)+",C"+str(i)+",D"+str(i);

			i++;
		}	
	}

	if(DB_TYPE==2)
	{	
		MYSQL_RES *result;	
		result = db_querya("SELECT * FROM profile_fields");
		while( db_fetch( result, item ) )
		{
			form["A"+str(i)]["#type"] = "markup";
			form["A"+str(i)]["#value"] = item["title"];
			form["A"+str(i)]["#table"] = "profiles";

			form["B"+str(i)]["#type"] = "markup";
			form["B"+str(i)]["#value"] = item["name"];
			form["B"+str(i)]["#table"] = "profiles";

			form["C"+str(i)]["#type"] = "markup";
			form["C"+str(i)]["#value"] = item["type"];
			form["C"+str(i)]["#table"] = "profiles";

			form["D"+str(i)]["#type"] = "markup";
			form["D"+str(i)]["#value"] = "<a href=\""+url("admin/profile/edit/"+item["fid"])+"\">Edit</a> <a href=\""+url("admin/profile/delete/"+item["fid"])+"\">Delete</a>";
			form["D"+str(i)]["#table"] = "profiles";

			form["profiles"]["#row"+str(i)] = "A"+str(i)+",B"+str(i)+",C"+str(i)+",D"+str(i);

			i++;
		}
	}

	out+= get_form("profiles_admin", form) + "<a href=\""+url("admin/profile/add")+"\">Add field</a>";

	return out;
}

string profile_edit()
{
	map <string, string> item;
	map <string, map<string,string> > form;

	string fid = arg(3);

	if(isset( fid ) )
	{
		if(DB_TYPE==1)
		{
			if( redis_multi( item, "HGETALL profile_fields:%d", intval(fid) ) )
			{
				form["fid"]["#type"] = "hidden";
				form["fid"]["#value"] = num( fid );
			}
		}
		if(DB_TYPE==2)
		{
			MYSQL_RES *result = db_querya("SELECT * FROM profile_fields WHERE fid=%d", fid.c_str() );	
			if( db_fetch( result, item ) )
			{
				form["fid"]["#type"] = "hidden";
				form["fid"]["#value"] = item["fid"];
			}
		}
	}

	form["title"]["#type"] = "textfield";
    form["title"]["#title"] = "Title";
    form["title"]["#description"] = "The title of the new field. The title will be shown to the user. An example title is \"Favorite color\"";
    form["title"]["#required"] = _TRUE;
	form["title"]["#value"] = item["title"];
	form["title"]["#weight"] = "1";
	
	form["name"]["#type"] = "textfield";
	form["name"]["#title"] = "Form name";
	form["name"]["#description"] = "The name of the field. The form name is not shown to the user but used internally in the HTML code and URLs. Unless you know what you are doing, it is highly recommended that you prefix the form name with <code>profile_</code> to avoid name clashes with other fields. Spaces or any other special characters except dash (-) and underscore (_) are not allowed. An example name is \"profile_favorite_color\" or perhaps just \"profile_color\"";
	form["name"]["#required"] = _TRUE;
	form["name"]["#value"] = item["name"];
	form["name"]["#weight"] = "2";

	form["explanation"]["#type"] = "textarea";
    form["explanation"]["#title"] = "Explanation";
    form["explanation"]["#description"] = "An optional explanation to go with the new field. The explanation will be shown to the user.";
	form["explanation"]["#value"] = item["explanation"];
	form["explanation"]["#weight"] = "3";

	form["submit"]["#type"] = "submit";
    form["submit"]["#value"] = "Save field";
	form["submit"]["#weight"] = "4";
  
	return get_form("profile_edit", form);
}

bool profile_edit_validate()
{
	return true;
}

string profile_edit_submit()
{
	if (!isset( cur_form["fid"]["#value"] ) )
	{
		if(DB_TYPE==1)
		{
			int fid = redis_int("INCR profile_fields:ids");
			redis_command("SADD profile_fields %d", fid);
			redis_command("HMSET profile_fields:%d title %s name %s explanation %s", 
				fid, cur_form["title"]["#value"].c_str(), cur_form["name"]["#value"].c_str(), cur_form["explanation"]["#value"].c_str() );
		}
		if(DB_TYPE==2)
		{
			db_querya("INSERT INTO profile_fields (title, name, explanation) VALUES ('%s', '%s', '%s')", 
				cur_form["title"]["#value"].c_str(), cur_form["name"]["#value"].c_str(), cur_form["explanation"]["#value"].c_str() );
		}
	}
	else 
	{
		if(DB_TYPE==1)
		{
			int fid = intval( cur_form["fid"]["#value"] );
			redis_command("HMSET profile_fields:%d title %s name %s explanation %s",
				fid, cur_form["title"]["#value"].c_str(), cur_form["name"]["#value"].c_str(), cur_form["explanation"]["#value"].c_str() );
		}
		if(DB_TYPE==2)
		{
			db_querya("UPDATE profile_fields SET title = '%s', name = '%s', explanation = '%s' WHERE fid = %d",
				cur_form["title"]["#value"].c_str(), cur_form["name"]["#value"].c_str(), cur_form["explanation"]["#value"].c_str(), cur_form["fid"]["#value"].c_str() );
		}
	}

	redirect( url("admin/profile") );

	return "";
}

string profile_delete()
{
	string fid = arg(3);

	if(isset( fid ) )
	{
		if(DB_TYPE==1)
		{
			redis_command("SREM profile_fields %d", intval(fid) );
			redis_command("DEL profile_fields:%d", intval(fid) ); 
		}
		if(DB_TYPE==2)
		{
			db_querya("DELETE FROM profile_fields WHERE fid=%d", fid.c_str() );
		}
	}

	redirect( url("admin/profile") );

	return "";
}
