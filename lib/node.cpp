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
#include "node.h"
#include "taxonomy.h"
#include "locale.h"

map <string, map<string,string> > cache_filter;
vector <string> cache_options;

void node_hooks()
{
	_MAP_DESC( node, "Allows content to be submitted to the site and displayed on pages." );
	_MAP_VER ( node, "1.3.1" );
	_MAP_PERM( node, "administer content types, administer nodes, access content, view revisions, revert revisions, delete revisions" );
	_MAP_HOOK( node, controllers );
	_MAP_HOOK( node, node_info );
	_MAP_HOOK( node, schema );
}

void node_schema()
{
	AssocOf(Assoc) node;
	node["#spec"]["name"] = "node";
	node["#spec"]["description"] = "The base table for nodes.";
	node["nid"]["description"] = "The primary identifier for a node.";
	node["nid"]["type"] = "serial";
	node["nid"]["unsigned"] = _TRUE;
	node["nid"]["not null"] = _TRUE;
	node["vid"]["description"] = "The current {node_revisions}.vid version identifier.";
	node["vid"]["type"] = "int";
	node["vid"]["unsigned"] = _TRUE;
	node["vid"]["not null"] = _TRUE;
	node["vid"]["default"] = "0";
	node["type"]["description"] = "The {node_type}.type of this node.";
	node["type"]["type"] = "varchar";
	node["type"]["length"] = "32";
	node["type"]["not null"] = _TRUE;
	node["type"]["default"] = "";
	node["language"]["description"] = "The {languages}.language of this node.";
	node["language"]["type"] = "varchar";
	node["language"]["length"] = "12";
	node["language"]["not null"] = _TRUE;
	node["language"]["default"] = "";
	node["title"]["description"] = "The title of this node, always treated as non-markup plain text.";
	node["title"]["type"] = "varchar";
	node["title"]["length"] = "255";
	node["title"]["not null"] = _TRUE;
	node["title"]["default"] = "";
	node["uid"]["description"] = "The {users}.uid that owns this node; initially, this is the user that created it.";
	node["uid"]["type"] = "int";
	node["uid"]["not null"] = _TRUE;
	node["uid"]["default"] = "0";
	node["status"]["description"] = "Boolean indicating whether the node is published (visible to non-administrators).";
	node["status"]["type"] = "int";
	node["status"]["not null"] = _TRUE;
	node["status"]["default"] = "1";
	node["created"]["description"] = "The Unix timestamp when the node was created.";
	node["created"]["type"] = "int";
	node["created"]["not null"] = _TRUE;
	node["created"]["default"] = "0";
	node["changed"]["description"] = "The Unix timestamp when the node was most recently saved.";
	node["changed"]["type"] = "int";
	node["changed"]["not null"] = _TRUE;
	node["changed"]["default"] = "0";
	node["comment"]["description"] = "Whether comments are allowed on this node: 0 = no, 1 = read only, 2 = read/write.";
	node["comment"]["type"] = "int";
	node["comment"]["not null"] = _TRUE;
	node["comment"]["default"] = "0";
	node["promote"]["description"] = "Boolean indicating whether the node should be displayed on the front page.";
	node["promote"]["type"] = "int";
	node["promote"]["not null"] = _TRUE;
	node["promote"]["default"] = "0";
	node["moderate"]["description"] = "Previously, a boolean indicating whether the node was \"in moderation\"; mostly no longer used.";
	node["moderate"]["type"] = "int";
	node["moderate"]["not null"] = _TRUE;
	node["moderate"]["default"] = "0";
	node["sticky"]["description"] = "Boolean indicating whether the node should be displayed at the top of lists in which it appears.";
	node["sticky"]["type"] = "int";
	node["sticky"]["not null"] = _TRUE;
	node["sticky"]["default"] = "0";
	node["tnid"]["description"] = "The translation set id for this node, which equals the node id of the source post in each set.";
	node["tnid"]["type"] = "int";
	node["tnid"]["unsigned"] = _TRUE;
	node["tnid"]["not null"] = _TRUE;
	node["tnid"]["default"] = "0";
	node["translate"]["description"] = "A boolean indicating whether this translation page needs to be updated.";
	node["translate"]["type"] = "int";
	node["translate"]["not null"] = _TRUE;
	node["translate"]["default"] = "0";
	node["#indexes"]["node_changed"] = "changed";
	node["#indexes"]["node_created"] = "created";
	node["#indexes"]["node_moderate"] = "moderate";
	node["#indexes"]["node_promote_status"] = "promote,status";
	node["#indexes"]["node_status_type"] = "status,type,nid";
	node["#indexes"]["node_title_type"] = "title,type|4";
	node["#indexes"]["node_type"] = "type|4";
	node["#indexes"]["uid"] = "uid";
	node["#indexes"]["tnid"] = "tnid";
	node["#indexes"]["translate"] = "translate";
	node["#unique keys"]["vid"] = "vid";
	node["#primary key"]["nid"] = "nid";
	poke( schema, node );

	AssocOf(Assoc) node_revisions;
	node_revisions["#spec"]["name"] = "node_revisions";
	node_revisions["#spec"]["description"] = "Stores information about each saved version of a {node}.";
	node_revisions["nid"]["description"] = "The {node} this version belongs to.";
	node_revisions["nid"]["type"] = "int";
	node_revisions["nid"]["unsigned"] = _TRUE;
	node_revisions["nid"]["not null"] = _TRUE;
	node_revisions["nid"]["default"] = "0";
	node_revisions["vid"]["description"] = "The primary identifier for this version.";
	node_revisions["vid"]["type"] = "serial";
	node_revisions["vid"]["unsigned"] = _TRUE;
	node_revisions["vid"]["not null"] = _TRUE;
	node_revisions["uid"]["description"] = "The {users}.uid that created this version.";
	node_revisions["uid"]["type"] = "int";
	node_revisions["uid"]["not null"] = _TRUE;
	node_revisions["uid"]["default"] = "0";
	node_revisions["title"]["description"] = "The title of this version.";
	node_revisions["title"]["type"] = "varchar";
	node_revisions["title"]["length"] = "255";
	node_revisions["title"]["not null"] = _TRUE;
	node_revisions["title"]["default"] = "";
	node_revisions["body"]["description"] = "The body of this version.";
	node_revisions["body"]["type"] = "text";
	node_revisions["body"]["not null"] = _TRUE;
	node_revisions["body"]["size"] = "big";
	node_revisions["teaser"]["description"] = "The teaser of this version.";
	node_revisions["teaser"]["type"] = "text";
	node_revisions["teaser"]["not null"] = _TRUE;
	node_revisions["teaser"]["size"] = "big";
	node_revisions["log"]["description"] = "The log entry explaining the changes in this version.";
	node_revisions["log"]["type"] = "text";
	node_revisions["log"]["not null"] = _TRUE;
	node_revisions["log"]["size"] = "big";
	node_revisions["timestamp"]["description"] = "A Unix timestamp indicating when this version was created.";
	node_revisions["timestamp"]["type"] = "int";
	node_revisions["timestamp"]["not null"] = _TRUE;
	node_revisions["timestamp"]["default"] = "0";
	node_revisions["format"]["description"] = "The input format used by this version's body.";
	node_revisions["format"]["type"] = "int";
	node_revisions["format"]["not null"] = _TRUE;
	node_revisions["format"]["default"] = "0";
	node_revisions["#indexes"]["nid"] = "nid";
	node_revisions["#indexes"]["uid"] = "uid";
	node_revisions["#primary key"]["vid"] = "vid";
	poke( schema, node_revisions );
}

void node_node_info()
{
	node_types["page"]["name"] = "Page";
	node_types["page"]["description"] = "A <em>page</em>, similar in form to a <em>story</em>, is a simple method for creating and displaying information that rarely changes, such as an \"About us\" section of a website. By default, a <em>page</em> entry does not allow visitor comments and is not featured on the site's initial home page.";

	node_types["story"]["name"] = "Story";
	node_types["story"]["description"] = "A <em>story</em>, similar in form to a <em>page</em>, is ideal for creating and displaying content that informs or engages website visitors. Press releases, site announcements, and informal blog-like entries may all be created with a <em>story</em> entry. By default, a <em>story</em> entry is automatically featured on the site's initial home page, and provides the ability to post comments.";
}

void node_controllers()
{
	map <string, string> item;

	item["path"] = "admin/node";
	item["title"] = "Content";
	item["callback"] = _MAP_FORM( node_admin );
	item["access arguments"] = "administer nodes";
	poke( controllers, item );

	item["path"] = "node/add";
	item["title"] = "Create Content";
	item["callback"] = _MAP( node_add );
	item["access arguments"] = "administer nodes";
	poke( controllers, item );

	map <string, map<string,string> >::iterator nt, nt_end;
	for( nt = node_types.begin(), nt_end = node_types.end();  nt != nt_end;  nt++ )
	{	
		item["path"] = "node/add/" + nt->first;
		item["title"] = "Create " + node_types[nt->first]["name"];
		item["callback"] = _MAP_FORM( node_page_add );
		item["access arguments"] = "administer nodes";
		poke( controllers, item );
	}

	item["path"] = "node/%/edit";
	item["title"] = "Edit Content";
	item["callback"] = _MAP_FORM( node_page_edit );
	item["access arguments"] = "administer nodes";
	item["type"] = str( MENU_LOCAL_TASK );
	item["parent"] = "admin/node";
	poke( controllers, item );

	item["path"] = "node/%";
	item["title"] = "View Content";
	item["access arguments"] = "access content";
	item["callback"] = _MAP( node_page_view );
	item["type"] = str( MENU_LOCAL_TASK );
	poke( controllers, item );
}

string node_add()
{
	string output;
	
	output = "<dl class=\"node-type-list\">";

	for( int i = 0; i < (int) controllers.size(); i++ )
	{	
		if( controllers[i]["path"].find("node/add/") != controllers[i]["path"].npos )
		{	
			string type = controllers[i]["path"].substr(9);

			output += "<dt><a href=\""+url( controllers[i]["path"] ) + "\">"+ node_types[ type ]["name"] +"</a></dt>";
			output += "<dd>"+ node_types[ type ]["description"] +"</dd>";
		}
    }
    output += "</dl>";

	return output;
}

void node_object_prepare( map<string,string> &node )
{
	//TODO: Configure Defauld Comments Enable
	if( !isset( node["comment"] ) )
	{
		if( node["type"] == "page" ) 
			node["comment"] = "0";
		else
			node["comment"] = "2";
	}

	if( isset( node["nid"] ) )
	{
		node["date"] = date("%Y-%m-%d %H:%M", node["created"] );
	}
	else
	{
		node["name"] = user["name"];
	}	
}

string node_form( map <string, map<string,string> > &form )
{
	node_object_prepare( form["#node"] );

	form["title"]["#type"] = "textfield";
	form["title"]["#title"] = "Title";
	form["title"]["#size"] = "60";
	form["title"]["#weight"] = "-1";
	form["title"]["#required"] = _TRUE;
	form["title"]["#value"] = form["#node"]["title"];

	form["body"]["#type"] = "textarea";
	form["body"]["#title"] = "Body";
	form["body"]["#rows"] = "6";
	form["body"]["#cols"] = "80";
	form["body"]["#weight"] = "10";
	form["body"]["#value"] = form["#node"]["body"];

	form["author"]["#type"] = "fieldset";
    //'#access' => user_access('administer nodes'),
    form["author"]["#title"] = "Authoring information";
    form["author"]["#collapsible"] = _TRUE;
    form["author"]["#collapsed"] = _TRUE;
    form["author"]["#weight"] = "20";
  	
	form["name"]["#type"] = "textfield";
    form["name"]["#title"] = "Authored by";
    form["name"]["#maxlength"] = "60";
    form["name"]["#value"] = isset(form["#node"]["name"]) ? form["#node"]["name"] : "";
    form["name"]["#weight"] = "-1";
    form["name"]["#description"] = "Leave blank for Anonymous";
	form["name"]["#fieldset"] = "author";

	form["date"]["#type"] = "textfield";
    form["date"]["#title"] = "Authored on";
    form["date"]["#maxlength"] = "25";
    form["date"]["#description"] = "Format: "+date("%Y-%m-%d %H:%M", str(time()) )+". Leave blank to use the time of form submission.";
	form["date"]["#fieldset"] = "author";

	if (isset(form["#node"]["date"])) {
		form["date"]["#value"] = form["#node"]["date"];
	}

	form["options"]["#type"] = "fieldset";
    //form["options"]["#access"] = user_access('administer nodes'),
    form["options"]["#title"] = "Publishing options";
    form["options"]["#collapsible"] = _TRUE;
    form["options"]["#collapsed"] = _TRUE;
    form["options"]["#weight"] = "25";
  
		form["status"]["#type"] = "checkbox";
		form["status"]["#title"] = "Published";
		form["status"]["#fieldset"] = "options";
		form["status"]["#weight"] = "-1";
		form["status"]["#checked"] = form["#node"]["status"];
		form["promote"]["#type"] = "checkbox";
		form["promote"]["#title"] = "Promoted to front page";
		form["promote"]["#fieldset"] = "options";
		form["promote"]["#checked"] = form["#node"]["promote"];
		form["sticky"]["#type"] = "checkbox";
		form["sticky"]["#title"] = "Sticky at top of lists";
		form["sticky"]["#fieldset"] = "options";
		form["sticky"]["#checked"] = form["#node"]["sticky"];

	form["submit"]["#type"] = "submit";
    form["submit"]["#value"] = "Save";
	form["submit"]["#weight"] = "30";
	form["preview"]["#type"] = "submit";
    form["preview"]["#value"] = "Preview";
	form["preview"]["#weight"] = "31";

	return get_form("add-page", form);
}

string node_page_add()
{
	map <string, map<string,string> > form;
	map <string, string> node_options;
	string	type;
	
	type = arg(2);
	variable_get("node_options_" + type, node_options);

	for( map <string, string>::iterator i = node_options.begin(), end = node_options.end();  i != end;  i++ )
	{
		form[ i->second ]["#checked"] = _TRUE;
	}

	form["#node"]["type"] = type;

	return node_form( form );
}

bool node_page_add_validate()
{
	return true;
}

void node_build( map<string, map<string,string> > &form,  map<string,string> &node)
{
	node = form["#node"];

	for( map <string, map<string,string> >::iterator curr = form.begin(), end = form.end();  curr != end;  curr++ )
	{
		if( curr->first != "submit" && curr->first != "preview" )
		{
			if( isset2( form[curr->first], "#value" ) )
			{
				nestclear( &node, curr->first ); // Clear previous values of the node
				size_t cut;
				node[curr->first] = form[curr->first]["#value"];
				if( (cut = node[curr->first].find("#array[")) != string::npos && cut == 0 ) // is an array
				{	
					size_t len = intval( node[curr->first].substr(7) );

					for(int i=0; i<(int) len; i++)
					{
						node[curr->first+"["+str(i)+"]"] = form[curr->first]["#value[" + str(i) + "]" ];
					}
				}
			}
			if( isset( form[curr->first]["#checked"] ) ) node[curr->first] = form[curr->first]["#checked"];
		}
	}

	if ( !isset(node["status"]) ) node["status"] = _FALSE;
	if ( !isset(node["comment"]) ) node["comment"] = _FALSE;
	if ( !isset(node["promote"]) ) node["promote"] = _FALSE;
	if ( !isset(node["moderate"]) ) node["moderate"] = _FALSE;
	if ( !isset(node["sticky"]) ) node["sticky"] = _FALSE;	
}

void _node_insert_node( map <string,string>	&node )
{
	if(DB_TYPE == 1)
	{
		int nid;
		node["nid"] = redis_str("INCR node:ids");
		nid = intval( node["nid"] );
		redis_command("SADD node %d", nid );
		redis_command("SADD node:type:%s %d", node["type"].c_str(), nid );

		if( intval(node["status"]) == 0 ) redis_command("SADD node:status:0 %d", nid );
		else redis_command("SADD node:status:1 %d", nid );

		if( intval(node["promote"]) == 0 ) redis_command("SADD node:promote:0 %d", nid );
		else redis_command("SADD node:promote:1 %d", nid );
		
		if( intval(node["moderate"]) == 0 ) redis_command("SADD node:moderate:0 %d", nid );
		else redis_command("SADD node:moderate:1 %d", nid );
		
		if( intval(node["sticky"]) == 0 ) redis_command("SADD node:sticky:0 %d", nid );
		else redis_command("SADD node:sticky:1 %d", nid );
		
		redis_command_fields(redis_arg("HMSET node:%d", nid ), "",
			"vid,type,title,uid,status,created,changed,comment,promote,moderate,sticky", "%d,%s,%s,%d,%d,%d,%d,%d,%d,%d,%d",
			intval(node["vid"]), 
			node["type"].c_str(), 
			node["title"].c_str(), 
			intval(node["uid"]), 
			intval(node["status"]),
			intval(node["created"]),
			intval(node["changed"]),
			intval(node["comment"]),
			intval(node["promote"]),
			intval(node["moderate"]),
			intval(node["sticky"]) );
	}
	if(DB_TYPE == 2)
	{
		db_querya("INSERT INTO node (vid, type, title, uid, status, created, changed, comment, promote, moderate, sticky) VALUES ( %d, '%s', '%s', %d, %d, %d, %d, %d, %d, %d, %d )",
			node["vid"].c_str(), 
			node["type"].c_str(), 
			node["title"].c_str(), 
			node["uid"].c_str(), 
			node["status"].c_str(), 
			node["created"].c_str(),
			node["changed"].c_str(),
			node["comment"].c_str(),
			node["promote"].c_str(),
			node["moderate"].c_str(),
			node["sticky"].c_str()
			);

		node["nid"] = db_last_insert_id();
	}
}

void _node_update_node( map <string,string>	&node )
{
	if(DB_TYPE == 1)
	{
		int nid = intval(node["nid"]);

		if( intval(node["status"]) == 0 ) {
			redis_command("SADD node:status:0 %d", nid ); redis_command("SREM node:status:1 %d", nid );
		} else {
			redis_command("SADD node:status:1 %d", nid ); redis_command("SREM node:status:0 %d", nid );
		}

		if( intval(node["promote"]) == 0 ) {
			redis_command("SADD node:promote:0 %d", nid ); redis_command("SREM node:promote:1 %d", nid );
		} else {
			redis_command("SADD node:promote:1 %d", nid ); redis_command("SREM node:promote:0 %d", nid );
		}

		if( intval(node["moderate"]) == 0 ) {
			redis_command("SADD node:moderate:0 %d", nid ); redis_command("SREM node:moderate:1 %d", nid );
		} else {
			redis_command("SADD node:moderate:1 %d", nid ); redis_command("SREM node:moderate:0 %d", nid );
		}

		if( intval(node["sticky"]) == 0 ) {
			redis_command("SADD node:sticky:0 %d", nid ); redis_command("SREM node:sticky:1 %d", nid );
		} else {
			redis_command("SADD node:sticky:1 %d", nid ); redis_command("SREM node:sticky:0 %d", nid );
		}

		redis_command_fields(redis_arg("HMSET node:%d", intval(node["nid"]) ), "",
			"vid,type,title,uid,status,created,changed,comment,promote,moderate,sticky", "%d,%s,%s,%d,%d,%d,%d,%d,%d,%d,%d",
			intval(node["vid"]), 
			node["type"].c_str(), 
			node["title"].c_str(), 
			intval(node["uid"]), 
			intval(node["status"]),
			intval(node["created"]),
			intval(node["changed"]),
			intval(node["comment"]),
			intval(node["promote"]),
			intval(node["moderate"]),
			intval(node["sticky"]) );
	}
	if(DB_TYPE == 2)
	{
		db_querya("UPDATE node SET vid=%d, type='%s', title='%s', uid=%d, status=%d, created=%d, changed=%d, comment=%d, promote=%d, moderate=%d, sticky=%d WHERE nid=%d",
			node["vid"].c_str(), 
			node["type"].c_str(), 
			node["title"].c_str(), 
			node["uid"].c_str(), 
			node["status"].c_str(), 
			node["created"].c_str(),
			node["changed"].c_str(),
			node["comment"].c_str(),
			node["promote"].c_str(),
			node["moderate"].c_str(),
			node["sticky"].c_str(),
			node["nid"].c_str()
			);
	}
}

void _node_insert_revision( map <string,string>	&node, string uid )
{
	if(DB_TYPE == 1)
	{
		node["vid"] = redis_str("INCR node_revisions:ids");
		redis_command_fields(redis_arg("HMSET node_revisions:%d", intval(node["vid"]) ), "",
			"uid,title,body,teaser,log,timestamp,format", "%d,%s,%s,%s,%s,%d,%d",
			intval(uid), node["title"].c_str(), node["body"].c_str(), node["teaser"].c_str(), node["log"].c_str(), intval(node["timestamp"]), intval(node["format"]) );
	}
	
	if(DB_TYPE == 2)
	{
		db_querya("INSERT INTO node_revisions (uid, title, body, teaser, log, timestamp, format) VALUES ( %d, '%s', '%s', '%s', '%s', %d, %d )",
			uid.c_str(), node["title"].c_str(), node["body"].c_str(), node["teaser"].c_str(), node["log"].c_str(), node["timestamp"].c_str(), node["format"].c_str() );
		node["vid"] = db_last_insert_id();
	}
}

void _node_update_revision( map <string,string>	&node, string uid )
{
	if(DB_TYPE == 1)
	{
		redis_command_fields(redis_arg("HMSET node_revisions:%d", intval(node["vid"]) ), "",
			"uid,title,body,teaser,log,timestamp,format", "%d,%s,%s,%s,%s,%d,%d",
			intval(uid), node["title"].c_str(), node["body"].c_str(), node["teaser"].c_str(), node["log"].c_str(), intval(node["timestamp"]), intval(node["format"]) );
	}
	if(DB_TYPE == 2)
	{
		db_querya("UPDATE node_revisions SET uid=%d, title='%s', body='%s', teaser='%s', log='%s', timestamp=%d, format=%d WHERE vid=%d",
			uid.c_str(), 
			node["title"].c_str(), 
			node["body"].c_str(), 
			node["teaser"].c_str(), 
			node["log"].c_str(), 
			node["timestamp"].c_str(), 
			node["format"].c_str(),
			node["vid"].c_str()
			);
	}
}

void node_delete( string nid )
{
	cur_node.clear();
	node_load( cur_node, nid );
	
	if( isset( cur_node["nid"] ) )
	{
		for( map <string, void (*)(void)>::iterator hook = _HOOKS.begin(), hook_end = _HOOKS.end();  hook != hook_end;  hook++ )
		{	
			size_t cut;
			if( (cut = hook->first.find( "_delete" )) != string::npos && (cut == hook->first.size() - 7) )
			{
				hook->second(); // call _delete
			}
		}
	}

	if(DB_TYPE==1)
	{
		int inid = intval(nid);
		int vid = redis_int("HGET node:%d vid", inid );
		string type = redis_str("HGET node:%d type", inid );
		redis_command("DEL node_revisions:%d", vid );
		redis_command("DEL node:%d", inid );
		redis_command("SREM node:status:1 %d", inid );
		redis_command("SREM node:status:0 %d", inid );
		redis_command("SREM node:promote:1 %d", inid );
		redis_command("SREM node:promote:0 %d", inid );
		redis_command("SREM node:moderate:1 %d", inid );
		redis_command("SREM node:moderate:0 %d", inid );
		redis_command("SREM node:sticky:1 %d", inid );
		redis_command("SREM node:sticky:0 %d", inid );
		redis_command("SREM node %d", inid );
		redis_command("SREM node:type:%s %d", type.c_str(), inid );
	}
	if(DB_TYPE==2)
	{
		db_querya("DELETE FROM node_revisions WHERE nid = %d", nid.c_str() );
		db_querya("DELETE FROM node WHERE nid = %d", nid.c_str() );
	}
}

void node_save( map <string,string>	&node )
{
	node["created"] = isset(node["date"]) ? strtotime( node["date"] ) : str(time());

	if ( !isset(node["uid"]) ) node["uid"] = user["uid"];

	// The changed timestamp is always updated for bookkeeping purposes (revisions, searching, ...)
	node["changed"] = str(time());
	node["timestamp"] = str(time());

	if ( !isset(node["format"]) ) node["format"] = FILTER_FORMAT_DEFAULT;

	string op;

	if ( !isset( node["nid"] ) )
	{
		_node_insert_revision(node, node["uid"]);
		_node_insert_node(node);
		if(DB_TYPE==1)
			redis_command("HSET node_revisions:%d nid %d", intval(node["vid"]), intval(node["nid"]) );
		if(DB_TYPE==2)
			db_querya("UPDATE node_revisions SET nid = %d WHERE vid = %d", node["nid"].c_str(), node["vid"].c_str() );
		op = "_insert";
	}
	else
	{
		_node_update_node(node);
		if (isset(node["revision"])) {
			_node_insert_revision(node, user["uid"]);
			if(DB_TYPE==1)
				redis_command("HSET node:%d vid %d", intval(node["nid"]), intval(node["vid"]) );
			if(DB_TYPE==2)
				db_querya("UPDATE node SET vid = %d WHERE nid = %d", node["vid"].c_str(), node["nid"].c_str() );
		}
		else {
		  _node_update_revision(node, user["uid"]);
		}
		op = "_update";
	}

	cur_node = node;
	
	for( map <string, void (*)(void)>::iterator hook = _HOOKS.begin(), hook_end = _HOOKS.end();  hook != hook_end;  hook++ )
	{	
		size_t cut;
		if( (cut = hook->first.find( op )) != string::npos && (cut == hook->first.size() - 7) )
		{
			hook->second(); // call _insert or _update
		}
	}
}

string node_page_add_submit()
{
	map <string,string>	node;

	node_build( cur_form, node );

	map <string,string>	account;
	account["name"] = node["name"];
	user_load( account );
	node["uid"] = account["uid"];

	node_save( node );

	redirect( url( "admin/node" ) );
	
	return "";
}

void node_build_filter_query( string &res_where, string &res_join )
{
	vector <string> WHERE;
	vector <string> ARGS;

	map <string, string> values;

	//if( isset( _COOKIE["BTFILTER"] ) )
		//unserialize_array( _COOKIE["BTFILTER"], values );

	if( isset( _COOKIE["BTFILTER"] ) ) values["filter"] = _COOKIE["BTFILTER"];
	if( isset( _COOKIE["BTSTATUS"] ) ) values["status"] = _COOKIE["BTSTATUS"];
	if( isset( _COOKIE["BTLANGUAGE"] ) ) values["language"] = _COOKIE["BTLANGUAGE"];
	if( isset( _COOKIE["BTKEYWORD"] ) ) values["keyword"] = _COOKIE["BTKEYWORD"];
	if( isset( _COOKIE["BTCATEGORY"] ) ) values["category"] = _COOKIE["BTCATEGORY"];
	if( isset( _COOKIE["BTTYPE"] ) ) values["type"] = _COOKIE["BTTYPE"];

	int index = 1;
	bool build_keyword_set = false;
	
	map <string, map<string,string> > filter;
	vector <string> options;
	node_filters( filter, options );

	int i_opt = intval( values["filter"] );
	string opt = options[ i_opt ];
	
	if( opt == "status" )
	{
		if( isset( values["status"] ) )
		{
			vector <string> pieces;
			explode( pieces, values["status"], '-' ); // Element [0] simply indicates the order;
			if(DB_TYPE==1) {
				WHERE.push_back( "node:" + pieces[1] + ":" + pieces[2] );
			}
			if(DB_TYPE==2) {
				WHERE.push_back( "n." + pieces[1] + " = " + pieces[2] );
			}
		}
	}
	if( opt == "category" )
	{
		string table = "tn"+str(index);
		if(DB_TYPE==1) {
			WHERE.push_back( "term_node:tid:"+values["category"] );
		}
		if(DB_TYPE==2) {
			WHERE.push_back ( table+".tid = "+values["category"] );
		}
        res_join += "INNER JOIN term_node "+table+" ON n.nid = "+table+".nid ";
	}
	if( opt == "type" )
	{
		if(DB_TYPE==1) {
			WHERE.push_back( "node:type:"+values["type"] );
		}
		if(DB_TYPE==2) {
			WHERE.push_back( "n.type = '"+values["type"]+"'" );
		}
	}
	if( opt == "language" )
	{
		if(DB_TYPE==1) {
		}
		if(DB_TYPE==2) {
			WHERE.push_back( "n.language = '"+values["language"]+"'" );
		}
	}
	if( isset( values["keyword"] ) )
	{
		if(DB_TYPE==1) {
			build_keyword_set = true;
		}
		if(DB_TYPE==2) {
			res_join += "INNER JOIN node_revisions nr ON n.vid = nr.vid ";
			WHERE.push_back( "(nr.body LIKE '%%%%"+values["keyword"]+"%%%%' OR n.title LIKE '%%%%"+values["keyword"]+"%%%%')" );
		}
	}

	if(DB_TYPE==1) {
		res_where = WHERE.size() > 0 ? implode( WHERE, "" ) : "node";
	}
	if(DB_TYPE==2) {
		res_where = WHERE.size() > 0 ? "WHERE " + implode( WHERE, " AND " ) : "";
	}

	if( build_keyword_set )
	{
		map <string, string> item;
		REDIS_RES *result;
		result = redis_query_fields("SORT "+res_where+" BY nosort", "GET node:*->", "#nid,vid,title");
		string set = "filter:"+microtime();
		while( redis_fetch_fields( result, item ) )
		{
			if( item["title"].find(values["keyword"]) != string::npos )
				redis_command("SADD %s %d", set.c_str(), intval(item["nid"]) );
		}
		redis_command("EXPIRE %s 10", set.c_str() );
		res_where = set;
	}
}

string node_admin()
{
	string out;
	map <string, string> item;
	
	vector <vector <map <string, string> > > rows;
	vector <map <string, string> > row;
	map <string, string> cell;
	vector <string> header;

	if( load_form_only ) return "";
	
	bool multilanguage = model_exists("locale");
	
	string WHERE, JOIN;
	node_build_filter_query( WHERE, JOIN );
	
	map <string, map<string,string> > form;
	map <string, string> options;

	options["approve"] = "Approve the selected posts";
	options["promote"] = "Promote the selected posts";
	options["duplicate"] = "Duplicate the selected posts";
	options["delete"] = "Delete the selected posts";
	options["unpublish"] = "Unpublish selected posts";
		
	form["operations"]["#type"] = "select";
	form["operations"]["#options"] = serialize_array( options );
	form["operations"]["#fieldset"] = "update";
	
	form["submit"]["#type"] = "submit";
	form["submit"]["#value"] = "Update";
	form["submit"]["#fieldset"] = "update";	

	header.push_back("");
	header.push_back("Title");
	header.push_back("Type");
	header.push_back("Author");
	header.push_back("Status");
	if( multilanguage ) header.push_back("Language");
	header.push_back("Operations");

	if(DB_TYPE==1)
	{
		REDIS_RES *result;
		result = redis_pager_fields("SORT "+WHERE+" BY node:*->changed DESC", "GET node:*->", "#nid,language,title,status,promote,moderate,sticky,type,uid", 40);
		while( redis_fetch_fields( result, item ) )
		{
			item["name"] = redis_str("HGET users:%d name", intval(item["uid"]));

			cell["data"] = "<input type=\"checkbox\" name=\"c"+item["nid"]+"\" value=\"1\"/>";
			poke(row, cell);
			cell["data"] = "<a href=\""+url("node/"+item["nid"])+"\">"+item["title"]+"</a>";
			poke(row, cell);
			cell["data"] = item["type"];
			poke(row, cell);
			cell["data"] = "<a href=\""+url("user/"+item["uid"])+"\">"+item["name"]+"</a>";
			poke(row, cell);
			cell["data"] =  (item["status"]=="1") ? "published" : "not&nbsp;published";
			cell["data"] += (item["promote"]=="1") ? "&nbsp;<span style=\"color:#888\">P</span>" : "";
			cell["data"] += (item["moderate"]=="1") ? "&nbsp;M" : "";
			cell["data"] += (item["sticky"]=="1") ? "&nbsp;S" : "";
			poke(row, cell);

			if( multilanguage )
			{
				cell["data"] = isset( item["language"] ) ? locale_get_name( item["language"] ) : "Language neutral";
				poke(row, cell);
			}

			cell["data"] = "<a href=\""+url("node/"+item["nid"]+"/edit")+"\">edit</a>";
			poke(row, cell);

			poke(rows, row);
		}
	}
	
	if(DB_TYPE==2)
	{	
		MYSQL_RES *result;
		string sql = "SELECT n.nid, n.language, n.title, n.status, n.promote, n.moderate, n.sticky, n.type, n.uid, u.name FROM node n "+JOIN+" LEFT JOIN users u ON u.uid=n.uid "+WHERE+" GROUP BY n.vid ORDER by n.changed DESC";
		result = pager_query( sql_parse( sql.c_str() ), 40 );		
		while( db_fetch( result, item ) )
		{
			cell["data"] = "<input type=\"checkbox\" name=\"c"+item["nid"]+"\" value=\"1\"/>";
			poke(row, cell);
			cell["data"] = "<a href=\""+url("node/"+item["nid"])+"\">"+item["title"]+"</a>";
			poke(row, cell);
			cell["data"] = item["type"];
			poke(row, cell);
			cell["data"] = "<a href=\""+url("user/"+item["uid"])+"\">"+item["name"]+"</a>";
			poke(row, cell);
			cell["data"] =  (item["status"]=="1") ? "published" : "not&nbsp;published";
			cell["data"] += (item["promote"]=="1") ? "&nbsp;<span style=\"color:#888\">P</span>" : "";
			cell["data"] += (item["moderate"]=="1") ? "&nbsp;M" : "";
			cell["data"] += (item["sticky"]=="1") ? "&nbsp;S" : "";
			poke(row, cell);

			if( multilanguage )
			{
				cell["data"] = isset( item["language"] ) ? locale_get_name( item["language"] ) : "Language neutral";
				poke(row, cell);
			}

			cell["data"] = "<a href=\""+url("node/"+item["nid"]+"/edit")+"\">edit</a>";
			poke(row, cell);

			poke(rows, row);
		}
	}

	out += theme_table( header, rows );

	out += theme_pager();

	form["update"]["#prefix"] = "<div class=\"container-inline\">";
	form["update"]["#type"] = "fieldset";
    form["update"]["#title"] = "Update options";
	form["update"]["#suffix"] = "</div>" + out;
	form["update"]["#weight"] = "1";

	return node_filter_form() + get_form("node_admin", form);
}

bool node_load(map <string,string> &node, string nid)
{
	if(DB_TYPE==1)
	{
		node["nid"] = nid;
		if( redis_multi(node, "HGETALL node:%d", intval(nid)) ) {
			if( redis_multi(node, "HGETALL node_revisions:%d", intval(node["vid"])) ) {
				if( !redis_fetch_fields( redis_query_fields(redis_arg("HMGET users:%d", intval(node["uid"])), "", "name,picture,picture_status,signature,data" ), node ) )
					return false;							
			} else return false;
		} else return false;
	}
	if(DB_TYPE==2)
	{
		MYSQL_RES *result;	
		result = db_querya("SELECT n.*, nr.*, u.name, u.picture, u.picture_status, u.signature, u.data FROM node n INNER JOIN users u ON u.uid = n.uid INNER JOIN node_revisions nr ON nr.vid=n.vid WHERE n.nid=%d", nid.c_str() );
		if( !db_fetch( result, node ) )
			return false;
	}
	
	cur_node = node;
	
	for( map <string, void (*)(void)>::iterator hook = _HOOKS.begin(), hook_end = _HOOKS.end();  hook != hook_end;  hook++ )
	{	
		size_t cut;
		if( (cut = hook->first.find( "_load" )) != string::npos && (cut == hook->first.size() - 5) )
		{
			hook->second(); // call _load
		}
	}

	node = cur_node;

	return true;
}

void node_preprocess_template( map <string,string>	&node )
{
	if( model_exists("taxonomy") ) {
		node["terms"] = taxonomy_link( node );
	}
	node["submitted"] = theme("node_submitted", node);
}

string node_view( map <string,string> &node )
{
	node["page"] = "0";
	if( node["type"] == "story" ) // only for texas
		node["node_url"] = url("story/"+node["nid"]);
	else
		node["node_url"] = url("node/"+node["nid"]);
	
	node["content"] = node["body"];
	node_preprocess_template( node );
	return theme( "node", node );
}

string node_page_view()
{
	map <string,string>	node;
	
	if( !node_load( node, str( intval( arg(1) ) ) ) )
	{	
		set_page_title( "Page not found" );
		node["content"] = "The requested page could not be found.";
		return theme( "error", node );
	}
	else
	{
		if( node["type"] != "page" )
		{
			set_page_title( "Page not found" );
			node["content"] = "The requested page could not be found.";
			return theme( "error", node );
		}
	}
	set_page_title( node["title"] );
	node["content"] = node["body"];
	node_preprocess_template( node );
		
	return theme( "node", node );
}

string node_page_edit()
{
	map <string, map<string,string> > form;
	map <string,string>	node;
	
	node_load( node, arg(1) );

	form["#node"] = node;
	
	return node_form( form );
}

bool node_page_edit_validate()
{
	return true;
}

string node_page_edit_submit()
{
	map <string,string>	node;
	
	node_build( cur_form, node );

	map <string,string>	account;
	account["name"] = node["name"];
	user_load( account );
	node["uid"] = account["uid"];

	node_save( node );

	redirect( url( "admin/node" ) );

	return "";
}

void node_filters( map <string, map<string,string> > &filter, vector <string> &options )
{	
	if( !cache_filter.empty() && !cache_options.empty() ) {
		filter = cache_filter;
		options = cache_options;
		return;
	}

	filter["status"]["0-status-1"] = "published";
	filter["status"]["1-status-0"] = "not published";
	filter["status"]["2-moderate-1"] = "in moderation";
	filter["status"]["3-moderate-0"] = "not in moderation";
	filter["status"]["4-promote-1"] = "promoted";
	filter["status"]["5-promote-0"] = "not promoted";
	filter["status"]["6-sticky-1"] = "sticky";
	filter["status"]["7-sticky-0"] = "not sticky";
	options.push_back( "status" );
		
	if( model_exists("taxonomy") )
	{
		map <string,string> voc;
		size_t count = 0;

		if(DB_TYPE==1)
		{
			REDIS_RES *result;
			result = redis_query_fields("SORT vocabulary BY vocabulary:*->name ALPHA", "GET vocabulary:*->", "#vid,name,description,help,relations,hierarchy,multiple,required,tags,weight");
			while( redis_fetch_fields( result, voc ) )
			{
				vector <pair <string, string> > data;
				vector <map <string, string> > terms;
				taxonomy_get_terms(voc["vid"], terms);

				for( size_t i=0; i< terms.size(); i++ )
				{
					data.push_back( make_pair( terms[i]["tid"], terms[i]["name"] ) );
				}

				filter["category"][ voc["name"] ] = serialize_array( data );
				count ++;
			}
		}
		if(DB_TYPE==2)
		{		
			MYSQL_RES *result;
			result = db_querya("SELECT v.* FROM vocabulary v ORDER BY v.weight, v.name");
			while ( db_fetch(result, voc) )
			{
				vector <pair <string, string> > data;
				vector <map <string, string> > terms;
				taxonomy_get_terms(voc["vid"], terms);

				for( size_t i=0; i< terms.size(); i++ )
				{
					data.push_back( make_pair( terms[i]["tid"], terms[i]["name"] ) );
				}

				filter["category"][ voc["name"] ] = serialize_array( data );
				count ++;
			}
		}
		if( count > 0 ) 
			options.push_back("category");
	}

	for( map <string, map<string,string> >::iterator nt = node_types.begin(), nt_end = node_types.end();  nt != nt_end;  nt++ )
	{	
		filter["type"][nt->first] = nt->first;
	}
	options.push_back("type");

	if( model_exists("locale") )
	{
		vector <map <string, string> > *languages;

		languages = language_list();

		for(size_t i=0; i<languages->size(); i++)
		{
			filter["language"][ (*languages)[i]["language"] ] = (*languages)[i]["name"];
		}
		options.push_back("language");
	}

	cache_filter = filter;
	cache_options = options;
}

string theme_node_filters()
{
	string out;
	
	out += "<ul><li><dl class=\"multiselect\">";

	out += "<dd class=\"a\">";
	out += form_render("filter", cur_form);
	out += "</dd>";

	out += "<dt>is</dt>";

	out += "<dd class=\"b\">";
	out += form_render("status", cur_form);
	out += form_render("category", cur_form);	
	out += form_render("type", cur_form);
	out += form_render("language", cur_form);
	out += "</dd>";

	out += "<dt>and</dt>";
	out += "<dt>" + form_render("keyword", cur_form) + "</dt>";
	out += "<dt>is within the content</dt>";

	out += "</dl>";
	out += "<div class=\"container-inline\" id=\"node-admin-buttons\">" + form_render("submit", cur_form) + "</div>";
	out += "</li></ul><br class=\"clear\" />";
	
	return out;
}

string node_filter_form()
{
	map <string, map<string,string> > form;
	map <string, map<string,string> > filter;
	vector <string> options;
	map <string, string> values;

	//if( isset( _COOKIE["BTFILTER"] ) )
		//unserialize_array( _COOKIE["BTFILTER"], values );

	if( isset( _COOKIE["BTFILTER"] ) ) values["filter"] = _COOKIE["BTFILTER"];
	if( isset( _COOKIE["BTSTATUS"] ) ) values["status"] = _COOKIE["BTSTATUS"];
	if( isset( _COOKIE["BTLANGUAGE"] ) ) values["language"] = _COOKIE["BTLANGUAGE"];
	if( isset( _COOKIE["BTKEYWORD"] ) ) values["keyword"] = _COOKIE["BTKEYWORD"];
	if( isset( _COOKIE["BTCATEGORY"] ) ) values["category"] = _COOKIE["BTCATEGORY"];
	if( isset( _COOKIE["BTTYPE"] ) ) values["type"] = _COOKIE["BTTYPE"];
	
	node_filters( filter, options );

	form["filters"]["#type"] = "fieldset";
    form["filters"]["#title"] = "Show only items where";
    form["filters"]["#theme"] = _MAP( theme_node_filters );

	for( map <string, map<string,string> >::iterator i = filter.begin(), end = filter.end();  i != end;  i++ )
	{
		form[i->first]["#type"] = "select";
		form[i->first]["#options"] = serialize_array( filter[i->first] );
		form[i->first]["#fieldset"] = "filters";
		form[i->first]["#value"] = values[ i->first ];
	}

	form["filter"]["#type"] = "radios";
	form["filter"]["#options"] = implode(options,",");
	form["filter"]["#value"] = values["filter"];
	form["filter"]["#fieldset"] = "filters";

	form["keyword"]["#type"] = "textfield";
	form["keyword"]["#value"] = values["keyword"];
	form["keyword"]["#size"] = "20";
	form["keyword"]["#fieldset"] = "filters";

	form["submit"]["#type"] = "submit";
	form["submit"]["#value"] = "Filter";
	form["submit"]["#fieldset"] = "filters";

	return "<div id=\"node-admin-filter\">" + get_form("node_filter_form", form) + "</div>";
}

bool node_admin_validate()
{
	return true;
}

string node_admin_submit()
{
	// TODO: Still Work in progress on filters
	if( _POST["op"] == "Update" )
	{
		vector <string> nids;
		
		for( map <string, string>::iterator curr = _POST.begin(), end = _POST.end();  curr != end;  curr++ )
		{
			if( curr->first[0] == 'c' )
				nids.push_back( curr->first.substr(1) );
		}

		for( size_t i=0; i<nids.size(); i++ )
		{
			if(DB_TYPE==1)
			{
				int nid = intval( nids[i] );
				if( _POST["operations"] == "promote" ) {
					redis_command("HSET node:%d promote 1", nid ); redis_command("SADD node:promote:1 %d", nid ); redis_command("SREM node:promote:0 %d", nid );							
				}					
				if( _POST["operations"] == "approve" ) {
					redis_command("HSET node:%d status 1", nid ); redis_command("SADD node:status:1 %d", nid ); redis_command("SREM node:status:0 %d", nid );
				}
				if( _POST["operations"] == "unpublish" ) {
					redis_command("HSET node:%d status 0", nid ); redis_command("SADD node:status:0 %d", nid ); redis_command("SREM node:status:1 %d", nid );
				}
			}

			if(DB_TYPE==2)
			{
				if( _POST["operations"] == "promote" )
					db_querya("UPDATE node SET promote=1 WHERE nid=%d", nids[i].c_str() );

				if( _POST["operations"] == "approve" )
					db_querya("UPDATE node SET status=1 WHERE nid=%d", nids[i].c_str() );

				if( _POST["operations"] == "unpublish" )
					db_querya("UPDATE node SET status=0 WHERE nid=%d", nids[i].c_str() );
			}

			if( _POST["operations"] == "duplicate" )
			{
				map <string, string> node;
				node_load( node, nids[i] );
				node.erase( node.find( "vid" ) ) ;
				node.erase( node.find( "nid" ) ) ;
				node["language"] = "";

				_POST["upload-total"] = str( cur_files.size() );
				for( size_t i = 0; i<cur_files.size(); i++ )
				{
					_POST["upload-data3"+str(i)] = cur_files[i]["fid"];
					_POST["upload-list"+str(i)] = cur_files[i]["list"];
					_POST["upload-description"+str(i)] = cur_files[i]["description"];
					_POST["upload-weight"+str(i)] = cur_files[i]["weight"];
				}

				node_save( node );
			}

			if( _POST["operations"] == "delete" )
			{
				node_delete( nids[i] );
			}
		}

		redirect( url("admin/node") );
	}
	else
	{
		setcookie("BTFILTER", _POST["filter"], str( time()+cookie_lifetime ), "/", getenv("HTTP_HOST") ? string(getenv("HTTP_HOST")) : "");
		setcookie("BTSTATUS", _POST["status"], str( time()+cookie_lifetime ), "/", getenv("HTTP_HOST") ? string(getenv("HTTP_HOST")) : "");
		setcookie("BTLANGUAGE", _POST["language"], str( time()+cookie_lifetime ), "/", getenv("HTTP_HOST") ? string(getenv("HTTP_HOST")) : "");
		setcookie("BTKEYWORD", _POST["keyword"], str( time()+cookie_lifetime ), "/", getenv("HTTP_HOST") ? string(getenv("HTTP_HOST")) : "");
		setcookie("BTCATEGORY", _POST["category"], str( time()+cookie_lifetime ), "/", getenv("HTTP_HOST") ? string(getenv("HTTP_HOST")) : "");
		setcookie("BTTYPE", _POST["type"], str( time()+cookie_lifetime ), "/", getenv("HTTP_HOST") ? string(getenv("HTTP_HOST")) : "");

		redirect( url("admin/node") );
	}

	return "";
}
