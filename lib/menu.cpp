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
#include "menu.h"

void menu_hooks()
{
	_MAP_DESC( menu, "Allows administrators to customize the site navigation menu." );
	_MAP_VER ( menu, "1.3.1" );
	_MAP_PERM( menu, "administer menu" );
	_MAP_HOOK( menu, schema );
	_MAP_HOOK( menu, install );
	_MAP_HOOK( menu, controllers );
}

void menu_schema()
{
	map <string, map<string, string> > menu;

	menu["#spec"]["name"] = "menu";
	menu["#spec"]["description"] = "Stores custom menu items information.";
    menu["mid"]["type"] = "serial";
    menu["mid"]["unsigned"] = _TRUE;
    menu["mid"]["not null"] = _TRUE;
    menu["mid"]["description"] = "The primary identifier for a menu item";

	menu["pid"]["type"] = "int";
    menu["pid"]["unsigned"] = _TRUE;
    menu["pid"]["not null"] = _TRUE;
    menu["pid"]["default"] = "0";
    menu["pid"]["description"] = "Parent for the node item.";

	menu["path"]["type"] = "varchar";
    menu["path"]["length"] = "255";
    menu["path"]["not null"] = _TRUE;
    menu["path"]["default"] = "";
    menu["path"]["description"] = "Path of the menu item.";

	menu["title"]["type"] = "varchar";
    menu["title"]["length"] = "255";
    menu["title"]["not null"] = _TRUE;
    menu["title"]["default"] = "";
    menu["title"]["description"] = "Path of the menu item.";

	menu["description"]["type"] = "varchar";
    menu["description"]["length"] = "255";
    menu["description"]["not null"] = _TRUE;
    menu["description"]["default"] = "";
    menu["description"]["description"] = "Path of the menu item.";

	menu["weight"]["type"] = "int";
    menu["weight"]["not null"] = _TRUE;
    menu["weight"]["default"] = "0";
    menu["weight"]["size"] = "tiny";
    menu["weight"]["description"] = "Weight of this menu item in relation to other items.";

	menu["type"]["type"] = "int";
    menu["type"]["not null"] = _TRUE;
    menu["type"]["default"] = "0";
    menu["type"]["description"] = "Weight of this menu item in relation to other items.";

	menu["#primary key"]["mid"] = "mid";
	schema.push_back( menu );
}

void menu_install()
{
	if(DB_TYPE==1)
	{
		redis_command("SET menu:ids 2");
		redis_command("SADD menu 1 2");
		redis_command("SADD menu:type:115 1 2");		
		redis_command_fields("HMSET menu:1", "", "pid,path,title,description,weight,type", "%d,%s,%s,%s,%d,%d", 0, "", "Primary links", "Primary links are often used at the theme layer to show the major sections of a site. A typical representation for primary links would be tabs along the top.", 0, 115 );
		redis_command_fields("HMSET menu:2", "", "pid,path,title,description,weight,type", "%d,%s,%s,%s,%d,%d", 0, "", "Secondary links", "Secondary links are often used for pages like legal notices, contact details, and other secondary navigation items that play a lesser role than primary links.", 0, 115 );
	}
	if(DB_TYPE==2)
	{
		db_querya("INSERT INTO menu(pid, path, title, description, weight, type) VALUES (0, '', 'Primary links', 'Primary links are often used at the theme layer to show the major sections of a site. A typical representation for primary links would be tabs along the top.', 0, 115)");
		db_querya("INSERT INTO menu(pid, path, title, description, weight, type) VALUES (0, '', 'Secondary links', 'Secondary links are often used for pages like legal notices, contact details, and other secondary navigation items that play a lesser role than primary links.', 0, 115)");
	}
}

void menu_controllers()
{
	map <string, string> item;

	item["path"] = "admin/menu";
	item["title"] = "Menus";
	item["callback"] = _MAP( menu_admin );
	item["access arguments"] = "administer menu";
	poke( controllers, item );

	item["path"] = "admin/menu/%";
	item["title"] = "Menus";
	item["callback"] = _MAP( menu_overview );
	item["type"] = str( ~MENU_VISIBLE_IN_TREE );
	item["access arguments"] = "administer menu";
	poke( controllers, item );

	item["path"] = "admin/menu/%";
	item["title"] = "List";
	item["weight"] = "1";
	item["callback"] = _MAP( menu_overview );
	item["type"] = str( MENU_LOCAL_TASK );
	item["access arguments"] = "administer menu";
	item["parent tab"] = "admin/menu/%";
	poke( controllers, item );

	item["path"] = "admin/menu/%/add";
	item["title"] = "Add Item";
	item["weight"] = "2";
	item["callback"] = _MAP_FORM( menu_edit_item );
	item["type"] = str( MENU_LOCAL_TASK );
	item["access arguments"] = "administer menu";
	item["parent tab"] = "admin/menu/%";
	poke( controllers, item );

	item["path"] = "admin/menu/%/edit";
	item["title"] = "Edit Item";
	item["callback"] = _MAP_FORM( menu_edit_item );
	item["type"] = str( ~MENU_VISIBLE_IN_TREE );
	item["access arguments"] = "administer menu";
	poke( controllers, item );

	item["path"] = "admin/menu/%/delete";
	item["title"] = "Delete Item";
	item["callback"] = _MAP_FORM( menu_delete_item );
	item["type"] = str( ~MENU_VISIBLE_IN_TREE );
	item["access arguments"] = "administer menu";
	poke( controllers, item );
}

vector <pair <string, string> > menu_make_tree(string pid, vector <map <string, string> > &items)
{
	static vector <pair <string, string> > list;
	static int depth = 0;

	// With vector of pairs we can keep the order we want

	if( depth == 0 ) list.clear();

	depth ++;
	
	for( size_t i = 0; i< items.size(); i++ )
	{
		if( items[i]["pid"] == pid )
		{
			list.push_back( make_pair( items[i]["mid"], str_repeat("-",depth-1) + items[i]["title"] ) );
			menu_make_tree( items[i]["mid"], items );
		}
	}

	depth --;
	
	return list;
}

string menu_overview()
{
	map <string, map<string,string> > form;
	vector <map <string, string> > items;
	map <string, string> item;
	
	if(DB_TYPE==1)
	{
		REDIS_RES *result = redis_query_fields("SORT menu BY menu:*->weight BY menu:*->title ALPHA", "GET menu:*->", "#mid,pid,path,title,description,weight,type");
		while( redis_fetch_fields( result, item ) ) {
			items.push_back( item );
		}
	}
	
	if(DB_TYPE==2)
	{
		MYSQL_RES * result = db_querya("SELECT * FROM menu ORDER by weight, title" );
		while ( db_fetch( result, item ) ) {
			items.push_back( item );
		}
	}

	string menu_id = arg(2);

	form["menu"]["#type"] = "table";
	form["menu"]["#header"] = "Menu item,Enabled,Expanded,Operations";
	form["menu"]["#weight"] = "5";

	vector <pair <string, string> >	list;	
	list = menu_make_tree( menu_id, items );

	for( size_t i=0; i<list.size(); i++)
	{	
		string expanded = _FALSE;
		string enabled = _FALSE;
		for( size_t j=0; j<items.size(); j++)
		{
			if( items[j]["mid"] == list[i].first )
			{
				expanded = (intval( items[j]["type"] ) & MENU_EXPANDED) ? _TRUE : _FALSE;
				enabled = (intval( items[j]["type"] ) & MENU_CUSTOM_ITEM) ? _TRUE : _FALSE;
			}
		}

		form["nam_"+str(i)]["#type"] = "markup";
		form["nam_"+str(i)]["#value"] = list[i].second;
		form["nam_"+str(i)]["#table"] = "menu";

		form["ena_"+str(i)]["#type"] = "checkbox";
		form["ena_"+str(i)]["#value"] = "1";
		form["ena_"+str(i)]["#checked"] = enabled;
		form["ena_"+str(i)]["#table"] = "menu";

		form["exp_"+str(i)]["#type"] = "checkbox";
		form["exp_"+str(i)]["#value"] = "1";
		form["exp_"+str(i)]["#checked"] = expanded;
		form["exp_"+str(i)]["#table"] = "menu";

		form["ope_"+str(i)]["#type"] = "markup";
		form["ope_"+str(i)]["#value"] = "<a href=\""+url("admin/menu/"+list[i].first+"/edit")+"\">edit</a>&nbsp<a href=\""+url("admin/menu/"+list[i].first+"/delete")+"\">delete</a>";
		form["ope_"+str(i)]["#table"] = "menu";

		form["menu"]["#row"+str(i)] = "nam_"+str(i)+",ena_"+str(i)+",exp_"+str(i)+",ope_"+str(i);
	}	

	return get_form("menu_overview", form);
}

string menu_delete_item()
{
	map <string, map<string,string> > form;
	string mid = arg(2);

	string title;
	if(DB_TYPE==1)
		title = redis_str("HGET menu:%d title", intval(mid) );
	if(DB_TYPE==2)
		title = db_result( db_querya("SELECT title FROM menu WHERE mid=%d", mid.c_str() ) );

	form["message"]["#type"] = "markup";
	form["message"]["#value"] = "<div>Are you sure you want to delete the custom menu item <b>"+title+"</b>?</div>";
	form["message"]["#weight"] = "1";
	
	form["submit"]["#type"] = "submit";
	form["submit"]["#value"] = "Confirm";
	form["submit"]["#weight"] = "10";
      
	return get_form("menu_delete_item", form);
}

bool menu_delete_item_validate()
{
	return true;
}

string menu_delete_item_submit()
{
	if(DB_TYPE==1) {
		int mid = intval( arg(2) );
		redis_command("SREM menu:type:%d %d", redis_int("HGET menu:%d type", mid), mid);
		redis_command("SREM menu %d", mid);
		redis_command("DEL menu:%d", mid);
	}
	if(DB_TYPE==2)	
		db_querya("DELETE FROM menu WHERE mid = %d", arg(2).c_str() );

	redirect( url( "admin/menu" ) );
	
	return "";
}

string menu_edit_item()
{
	map <string, map<string,string> > form;
	vector <map <string, string> > items;
	map <string, string> item;
	
	if(DB_TYPE==1)
	{
		REDIS_RES *result = redis_query_fields("SORT menu BY nosort", "GET menu:*->", "#mid,pid,path,title,description,weight,type");
		while( redis_fetch_fields( result, item ) ) {
			items.push_back( item );
		}
	}
	if(DB_TYPE==2)
	{
		MYSQL_RES * result = db_querya("SELECT * FROM menu");
		while ( db_fetch( result, item ) ) {
			items.push_back( item );
		}
	}

	item.clear();

	if( arg(3) == "add" )
	{
		item["pid"] = arg(2);
		item["type"] = str( MENU_CUSTOM_ITEM );
	}
	else
	{
		if(DB_TYPE==1) {
			int mid = intval( arg(2) );
			redis_multi( item, "HGETALL menu:%d", mid );
			item["mid"] = str( mid );
		}
		if(DB_TYPE==2)		
			db_fetch( db_querya("SELECT * FROM menu WHERE mid=%d", arg(2).c_str() ), item );
	}

	form["title"]["#type"] = "textfield";
    form["title"]["#title"] = "Title";
    form["title"]["#value"] = item["title"];
    form["title"]["#description"] = "The name of the menu item.";
    form["title"]["#required"] = _TRUE;
	form["title"]["#weight"] = "1";
  
	form["description"]["#type"] = "textfield";
    form["description"]["#title"] = "Description";
    form["description"]["#value"] = item["description"];
    form["description"]["#description"] = "The description displayed when hovering over a menu item.";
	form["description"]["#weight"] = "2";
  
	form["path"]["#type"] = "textfield";
	form["path"]["#title"] = "Path";
	form["path"]["#value"] = item["path"];
	form["path"]["#description"] = "The path this menu item links to.";
	form["path"]["#required"] = _TRUE;
	form["path"]["#weight"] = "6";

	string expanded = intval(item["type"]) & MENU_EXPANDED ? _TRUE : _FALSE;
	form["expanded"]["#type"] = "checkbox";
    form["expanded"]["#title"] = "Expanded";
	form["expanded"]["#value"] = "1";
    form["expanded"]["#checked"] = expanded;
    form["expanded"]["#description"] = "If selected and this menu item has children, the menu will always appear expanded.";
	form["expanded"]["#weight"] = "7";
		
	vector <pair <string, string> > options = menu_make_tree( "0", items );

	form["pid"]["#type"] = "select";
    form["pid"]["#title"] = "Parent item";
    form["pid"]["#value"] = item["pid"];
	form["pid"]["#options"] = serialize_array( options );
	form["pid"]["#weight"] = "8";
 
	form["weight"]["#type"] = "weight";
    form["weight"]["#title"] = "Weight";
    form["weight"]["#value"] = item["weight"];
    form["weight"]["#description"] = "Optional. In the menu, the heavier items will sink and the lighter items will be positioned nearer the top.";
	form["weight"]["#weight"] = "9";

	form["type"]["#type"] = "hidden";
	form["type"]["#value"] = item["type"];

	form["mid"]["#type"] = "hidden";
	form["mid"]["#value"] = item["mid"];
	
	form["submit"]["#type"] = "submit";
	form["submit"]["#value"] = "Submit";
	form["submit"]["#weight"] = "10";
      
	return get_form("menu_edit_item", form);
}

string menu_edit_item_submit()
{
	map <string, string> item;

	item["pid"] = _POST["pid"];
	item["path"] = _POST["path"];
	item["title"] = _POST["title"];
	item["description"] = _POST["description"];
	item["weight"] = _POST["weight"];
	item["mid"] = _POST["mid"];
	item["type"] = _POST["type"];

	if( _POST["expanded"] == _TRUE )
		item["type"] = str( intval( item["type"] ) | MENU_EXPANDED );
	else
		item["type"] = str( intval( item["type"] ) & ~MENU_EXPANDED );
	
	if( isset(item["mid"]) )
	{
		if(DB_TYPE==1)
		{
			int mid = intval( item["mid"] );
			redis_command("SREM menu:type:%d %d", redis_int("GET menu:%d:type", mid), mid);
			redis_command("SADD menu:type:%d %d", intval(item["type"]), mid);
			redis_command_fields(redis_arg("HMSET menu:%d", mid), "", "pid,path,title,description,weight,type", "%d,%s,%s,%s,%d,%d", 
				intval(item["pid"]), item["path"].c_str(), item["title"].c_str(), item["description"].c_str(), intval(item["weight"]), intval(item["type"]) );
		}
		if(DB_TYPE==2)
		{
			db_querya("UPDATE menu SET pid = %d, path = '%s', title = '%s', description = '%s', weight = %d, type = %d WHERE mid = %d",
				item["pid"].c_str(), item["path"].c_str(), item["title"].c_str(), item["description"].c_str(), item["weight"].c_str(), item["type"].c_str(), item["mid"].c_str() );
		}
	}
	else
	{
		if(DB_TYPE==1)
		{
			int mid = redis_int("INCR menu:ids");
			redis_command("SADD menu %d", mid);
			redis_command("SADD menu:type:%d %d", intval(item["type"]), mid);
			redis_command_fields(redis_arg("HMSET menu:%d", mid), "", "pid,path,title,description,weight,type", "%d,%s,%s,%s,%d,%d", 
				intval(item["pid"]), item["path"].c_str(), item["title"].c_str(), item["description"].c_str(), intval(item["weight"]), intval(item["type"]) );
		}
		if(DB_TYPE==2)
		{
			db_querya("INSERT INTO menu ( pid, path, title, description, weight, type) VALUES (%d, '%s', '%s', '%s', %d, %d)", 
				item["pid"].c_str(), item["path"].c_str(), item["title"].c_str(), item["description"].c_str(), item["weight"].c_str(), item["type"].c_str() );
		}
	}
	
	return "";
}

bool menu_edit_item_validate()
{
	return true;
}

string menu_admin()
{
	map <string, string> menu;
	string output;
	
	output = "<dl class=\"node-type-list\">";

	if(DB_TYPE==1)
	{
		REDIS_RES *result = redis_query_fields("SORT menu:type:115 BY menu:*->title ALPHA", "GET menu:*->", "#mid,pid,path,title,description,weight,type");
		while( redis_fetch_fields( result, menu ) ) {
			output += "<dt><a href=\""+url("admin/menu/" + menu["mid"] ) + "\">"+ menu["title"] +"</a></dt>";
			output += "<dd>"+ menu["description"] +"</dd>";
		}	
	}
	if(DB_TYPE==2)
	{
		MYSQL_RES * result = db_querya("SELECT * FROM menu WHERE type=115 ORDER BY title" );
		while ( db_fetch( result, menu ) ) {	
			output += "<dt><a href=\""+url("admin/menu/" + menu["mid"] ) + "\">"+ menu["title"] +"</a></dt>";
			output += "<dd>"+ menu["description"] +"</dd>";
		}
	}
    
	output += "</dl>";

	return output;
}
