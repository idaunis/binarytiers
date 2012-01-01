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

void user_hooks()
{
	_MAP_DESC( user, "Manages the user registration and login system." );
	_MAP_VER ( user, "1.3.1" );
	_MAP_PERM( user, "administer permissions, administer users, access user profiles, change own username" );
	_MAP_HOOK( user, controllers );
	_MAP_HOOK( user, help );
	_MAP_HOOK( user, schema );
}

void user_schema()
{
	map <string, map<string, string> > permission;
	permission["#spec"]["name"] = "permission";
	permission["#spec"]["description"] = "Stores permissions for users.";
	permission["pid"]["type"]= "serial";
	permission["pid"]["not null"] = _TRUE;
	permission["pid"]["description"] = "Primary Key: Unique permission ID.";
	permission["rid"]["type"] = "int";
	permission["rid"]["unsigned"] = _TRUE;
	permission["rid"]["not null"] = _TRUE;
	permission["rid"]["default"] = "0";
	permission["rid"]["description"] = "The {role}.rid to which the permissions are assigned.";
	permission["perm"]["type"] = "text";
	permission["perm"]["not null"] = _FALSE;
	permission["perm"]["size"] = "big";
	permission["perm"]["description"] = "List of permissions being assigned.";
	permission["tid"]["type"] = "int";
	permission["tid"]["unsigned"] = _TRUE;
	permission["tid"]["not null"] = _TRUE;
	permission["tid"]["default"] = "0";
	permission["tid"]["description"] = "Originally intended for taxonomy-based permissions, but never used.";
	permission["#primary key"]["pid"] = "pid";
	permission["#indexes"]["rid"] = "rid";
	schema.push_back( permission );

	map <string, map<string, string> > role;
	role["#spec"]["name"] = "role";
    role["#spec"]["description"] = "Stores user roles.";
	role["rid"]["type"] = "serial";
	role["rid"]["unsigned"] = _TRUE;
	role["rid"]["not null"] = _TRUE;
	role["rid"]["description"] = "Primary Key: Unique role id.";
	role["name"]["type"] = "varchar";
	role["name"]["length"] = "64";
	role["name"]["not null"] = _TRUE;
	role["name"]["default"] = "";
	role["name"]["description"] = "Unique role name.";
	role["#unique keys"]["name"] = "name";
	role["#primary key"]["rid"] = "rid";
	schema.push_back( role );
  
	map <string, map<string, string> > users_roles;
	users_roles["#spec"]["name"] = "users_roles";
	users_roles["#spec"]["description"] = "Maps users to roles.";
	users_roles["uid"]["type"] = "int";
	users_roles["uid"]["unsigned"] = _TRUE;
	users_roles["uid"]["not null"] = _TRUE;
	users_roles["uid"]["default"] = "0";
	users_roles["uid"]["description"] = "Primary Key: {users}.uid for user.";
	users_roles["rid"]["type"] = "int";
	users_roles["rid"]["unsigned"] = _TRUE;
	users_roles["rid"]["not null"] = _TRUE;
	users_roles["rid"]["default"] = "0";
	users_roles["rid"]["description"] = "Primary Key: {role}.rid for role.";
	users_roles["#primary key"]["uid"] = "uid";
	users_roles["#primary key"]["rid"] = "rid";
	users_roles["#indexes"]["rid"] = "rid";
	schema.push_back( users_roles );

	map <string, map<string, string> > users;
	users["#spec"]["name"] = "users";
    users["#spec"]["description"] = "Stores user data.";
	users["uid"]["type"] = "serial";
    users["uid"]["unsigned"] = _TRUE;
    users["uid"]["not null"] = _TRUE;
    users["uid"]["description"] = "Primary Key: Unique user ID.";
    users["name"]["type"] = "varchar";
    users["name"]["length"] = "60";
    users["name"]["not null"] = _TRUE;
    users["name"]["default"] = "";
    users["name"]["description"] = "Unique user name.";
    users["pass"]["type"] = "varchar";
    users["pass"]["length"] = "32";
    users["pass"]["not null"] = _TRUE;
    users["pass"]["default"] = "";
    users["pass"]["description"] = "User\"s password (md5 hash).";
    users["mail"]["type"] = "varchar";
    users["mail"]["length"] = "64";
    users["mail"]["not null"] = _FALSE;
    users["mail"]["default"] = "";
    users["mail"]["description"] = "User's email address.";

	users["first"]["type"] = "varchar";
    users["first"]["length"] = "64";
    users["first"]["not null"] = _FALSE;
    users["first"]["default"] = "";
    users["first"]["description"] = "User's first name address.";

	users["last"]["type"] = "varchar";
    users["last"]["length"] = "64";
    users["last"]["not null"] = _FALSE;
    users["last"]["default"] = "";
    users["last"]["description"] = "User's last name address.";
    
	users["mode"]["type"] = "int";
    users["mode"]["not null"] = _TRUE;
    users["mode"]["default"] = "0";
    users["mode"]["size"] = "tiny";
	users["mode"]["description"] = "0: Atlethe, Atlethe Disabled, Couch.   1: Team, My Group.   2: Sport Facility, Federation/League.";
    users["sort"]["type"] = "int";
    users["sort"]["not null"] = _FALSE;
    users["sort"]["default"] = "0";
    users["sort"]["size"] = "tiny";
    users["sort"]["description"] = "Per-user comment sort order (newest vs. oldest first), used by the {comment} model.";
    users["color"]["type"] = "int";
    users["color"]["not null"] = _FALSE;
    users["color"]["default"] = "0";
    users["color"]["size"] = "tiny";
    users["color"]["description"] = "Color of the Theme.";    
	users["theme"]["type"] = "varchar";
    users["theme"]["length"] = "255";
    users["theme"]["not null"] = _TRUE;
    users["theme"]["default"] = "";
    users["theme"]["description"] = "User\"s default theme.";
    users["signature"]["type"] = "varchar";
    users["signature"]["length"] = "255";
    users["signature"]["not null"] = _TRUE;
    users["signature"]["default"] = "";
    users["signature"]["description"] = "User's signature.";
    users["signature_format"]["type"] = "int";
    users["signature_format"]["size"] = "small";
    users["signature_format"]["not null"] = _TRUE;
    users["signature_format"]["default"] = "0";
    users["signature_format"]["description"] = "The {filter_formats}.format of the signature.";
    users["created"]["type"] = "int";
    users["created"]["not null"] = _TRUE;
    users["created"]["default"] = "0";
    users["created"]["description"] = "Timestamp for when user was created.";
	users["changed"]["type"] = "int";
    users["changed"]["not null"] = _TRUE;
    users["changed"]["default"] = "0";
    users["changed"]["description"] = "Timestamp for when user change any content.";
    users["access"]["type"] = "int";
    users["access"]["not null"] = _TRUE;
    users["access"]["default"] = "0";
    users["access"]["description"] = "Timestamp for previous time user accessed the site.";
    users["login"]["type"] = "int";
    users["login"]["not null"] = _TRUE;
    users["login"]["default"] = "0";
    users["login"]["description"] = "Timestamp for user's last login.";
    users["status"]["type"] = "int";
    users["status"]["not null"] = _TRUE;
    users["status"]["default"] = "0";
    users["status"]["size"] = "tiny";
    users["status"]["description"] = "Whether the user is active(1) or blocked(0).";
    
	users["timezone"]["type"] = "varchar";
    users["timezone"]["length"] = "8";
    users["timezone"]["not null"] = _FALSE;
    users["timezone"]["description"] = "User's timezone.";
    
	users["language"]["type"] = "varchar";
	users["language"]["length"] = "12";
    users["language"]["not null"] = _TRUE;
    users["language"]["default"] = "";
    users["language"]["description"] = "User's default language.";
    users["picture"]["type"] = "varchar";
    users["picture"]["length"] = "255";
    users["picture"]["not null"] = _TRUE;
    users["picture"]["default"] = "";
    users["picture"]["description"] = "Path to the user's uploaded picture.";
	users["picture_status"]["type"] = "int";
    users["picture_status"]["not null"] = _TRUE;
    users["picture_status"]["default"] = "0";
    users["picture_status"]["size"] = "tiny";
    users["picture_status"]["description"] = "Whether the picture is local(1), cdn(2) or blocked(0).";
    
    users["init"]["type"] = "varchar";
    users["init"]["length"] = "64";
    users["init"]["not null"] = _FALSE;
    users["init"]["default"] = "";
    users["init"]["description"] = "Email address used for initial account creation.";
    users["data"]["type"] = "text";
    users["data"]["not null"] = _FALSE;
    users["data"]["size"] = "big";
    users["data"]["description"] = "A serialized array of name value pairs that are related to the user.";
	users["#indexes"]["access"] = "access";
    users["#indexes"]["created"] = "created";
    users["#indexes"]["mail"] = "mail";
    users["#unique keys"]["name"] = "name";
    users["#primary key"]["uid"] = "uid";
	schema.push_back( users );
}

void user_help()
{
	string	path = _GET["q"];

	if( path == "admin/user" )
	{
		help = "<p>BinaryTiers allows users to register, login, log out, maintain user profiles, etc. Users of the site may not use their own names to post content until they have signed up for a user account.</p>";
	}

	if( path == "admin/roles" )
	{
      help = "<p>Roles allow you to fine tune the security and administration of Binary Tiers. A role defines a group of users that have certain privileges as defined in <a href=\""+url("admin/permissions")+"\">user permissions</a>. Examples of roles include: anonymous user, authenticated user, moderator, administrator and so on. In this area you will define the <em>role names</em> of the various roles. To delete a role choose \"edit\".</p><p>By default, Binary Tiers comes with two user roles:</p>\
      <ul>\
      <li>Anonymous user: this role is used for users that don\'t have a user account or that are not authenticated.</li>\
      <li>Authenticated user: this role is automatically granted to all logged in users.</li>\
      </ul>";
	}

	if( path == "admin/permissions" )
	{
		help = "<p>Permissions let you control what users can do on your site. Each user role (defined on the <a href=\""+url("admin/roles")+"\">user roles page</a>) has its own set of permissions. For example, you could give users classified as \"Administrators\" permission to \"administer nodes\" but deny this power to ordinary, \"authenticated\" users. You can use permissions to reveal new features to privileged users (those with subscriptions, for example). Permissions also allow trusted users to share the administrative burden of running a busy site.</p>";
	}

	if( path == "admin/user/create" )
	{
		help = "<p>Binary Tiers allows the administrators to register a new users by hand. Note that you cannot have a user where either the e-mail address or the username match another user in the system.</p>";
	}
}

void user_controllers()
{
	map <string, string> item;

	item["path"] = "admin/user";
	item["title"] = "Users";
	item["callback"] = _MAP_FORM( user_admin );
	item["access callback"] = _MAP_ACCESS( user_access );
	item["access arguments"] = "administer users";
	item["parent tab"] = "admin/user";
	poke( controllers, item );

	item["path"] = "admin/user/create";
	item["title"] = "Add user";
	item["weight"] = "2";
	item["callback"] = _MAP_FORM( user_register );
	item["type"] = str( MENU_LOCAL_TASK );
	item["access arguments"] = "administer users";
	item["parent tab"] = "admin/user";
	poke( controllers, item );

	item["path"] = "admin/user";
	item["title"] = "List";
	item["weight"] = "1";
	item["callback"] = _MAP_FORM( user_admin );
	item["type"] = str( MENU_LOCAL_TASK );
	item["access arguments"] = "administer users";
	item["parent tab"] = "admin/user";
	poke( controllers, item );

	item["path"] = "admin/roles";
	item["title"] = "Roles";
	item["callback"] = _MAP_FORM( user_admin_roles );
	item["access arguments"] = "administer permissions";
	poke( controllers, item );

	item["path"] = "admin/permissions";
	item["title"] = "Permissions";
	item["callback"] = _MAP( user_admin_permissions );
	item["access arguments"] = "administer permissions";
	poke( controllers, item );

	item["path"] = "admin/roles/edit/%";
	item["title"] = "Edit role";
	item["callback"] = _MAP_FORM( user_admin_roles_edit );
	item["type"] = str( MENU_LOCAL_TASK );
	item["access callback"] = _MAP_ACCESS( user_access );
	item["access arguments"] = "administer permissions";
	poke( controllers, item );

	item["path"] = "user/%/edit";
	item["title"] = "Edit";
	item["weight"] = "2";
	item["callback"] = _MAP_FORM( user_edit );
	item["type"] = str( MENU_LOCAL_TASK );
	item["access arguments"] = "administer users";
	item["parent"] = "admin/user";
	item["parent tab"] = "user/%";
	poke( controllers, item );

	item["path"] = "user/%";
	item["title"] = "View";
	item["weight"] = "1";
	item["callback"] = _MAP( user_view );
	item["type"] = str( MENU_DEFAULT_LOCAL_TASK );
	item["access arguments"] = "administer users";
	item["parent"] = "admin/user";
	item["parent tab"] = "user/%";
	poke( controllers, item );

	item["path"] = "login";
	item["title"] = "Login";
	item["callback"] = _MAP_FORM( user_login );
	item["access callback"] = _MAP_ACCESS( user_is_not_authenticated );
	poke( controllers, item );

	item["path"] = "logout";
	item["title"] = "Log Out";
	item["callback"] = _MAP( user_logout );
	item["access callback"] = _MAP_ACCESS( user_is_authenticated );
	poke( controllers, item );

	item["path"] = "user/autoverify/%/%";
	item["title"] = "Autoverify";
	item["callback"] = _MAP( user_autoverify );
	item["type"] = str( MENU_CALLBACK );
	item["access arguments"] = "access content";
	poke( controllers, item );
}

string user_view()
{
	string uid = arg(1);
	string created = db_result( db_querya("SELECT created FROM users WHERE uid=%d", uid.c_str() ) );

	string out;
	
	out = "<h2 class=\"title\">History</h2><dl><dt class=\"user-member\">Member for</dt>\
		<dd class=\"user-member\">"+format_interval( time()-intval(created) )+"</dd></dl>";

	return out;
}

string user_edit()
{
	MYSQL_RES *result;
	map <string, string> item;
	map <string, map<string,string> > form;
	vector <pair <string, string> > roles;
	vector <string> users_roles;

	users_roles.push_back( AUTHENTICATED_RID );

	if( DB_TYPE == 1 )
	{
		redis_multi_reversed( roles, "ZRANGE role 1 -1 WITHSCORES");
				
		string the_role;
		REDIS_RES *rr = redis_query( "LRANGE users:%d:roles 0 -1", intval(arg(1)) );
		while( redis_fetch( rr, the_role ) ) {
			users_roles.push_back( the_role );
	    }
		int uid = intval(arg(1));

		item["uid"] = str( uid );
		redis_multi(item, "HGETALL users:%d", uid );
	}

	if( DB_TYPE == 2 )
	{
		result = db_querya("SELECT * FROM role WHERE rid>1 ORDER by rid");
		while( db_fetch( result, item ) ) {
			roles.push_back( make_pair(item["rid"], item["name"]) );
		}

		result = db_querya("SELECT rid FROM users_roles WHERE uid=%d", str( intval(arg(1)) ).c_str());
		while( db_fetch( result, item ) ){
			users_roles.push_back( item["rid"] );
		}
		
		result = db_querya( "SELECT uid, name, mail, created, access, status, theme, signature FROM users WHERE uid<>0 AND uid=%d", str( intval(arg(1)) ).c_str() );
		if( !db_num_rows( result ) ) {
			return "Sorry. The user you are trying to edit doesn't exist.";
		}
		db_fetch( result, item );
	}

	form["roles"]["#type"] = "checkboxes";
	form["roles"]["#title"] = "Roles";
	form["roles"]["#multiple"] = _TRUE;
	form["roles"]["#options"] = serialize_array( roles );
	form["roles"]["#disabled"] = "2";
	nest(users_roles, &form["roles"], "#value");
	form["roles"]["#weight"] = "5";
	form["roles"]["#fieldset"] = "info";
		
	form["info"]["#type"] = "fieldset";
	form["info"]["#title"] = "Account information";
	form["info"]["#collapsible"] = _FALSE;
	form["info"]["#collapsed"] = _FALSE;
	form["info"]["#weight"] = "1";

		form["name"]["#fieldset"] = "info";
		form["name"]["#type"] = "textfield";
		form["name"]["#title"] = "Username";
		form["name"]["#description"] = "Your full name or your preferred username: only letters, numbers and spaces are allowed.";
		form["name"]["#size"] = "60";
		form["name"]["#weight"] = "1";
		form["name"]["#value"] = item["name"];
		form["name"]["#required"] = _TRUE;

		form["mail"]["#fieldset"] = "info";
		form["mail"]["#type"] = "textfield";
		form["mail"]["#title"] = "E-mail address";
		form["mail"]["#description"] = "Insert a valid e-mail address. All e-mails from the system will be sent to this address. The e-mail address is not made public and will only be used if you wish to receive a new password or wish to receive certain news or notifications by e-mail.";
		form["mail"]["#size"] = "60";
		form["mail"]["#weight"] = "2";
		form["mail"]["#value"] = item["mail"];
		form["mail"]["#required"] = _TRUE;
	
		form["pass"]["#fieldset"] = "info";
		form["pass"]["#type"] = "password_confirm";
		form["pass"]["#title"] = "Password";
		form["pass"]["#description"] = "To change the current user password, enter the new password in both fields.";
		form["pass"]["#value"] = "";
		form["pass"]["#size"] = "12";
		form["pass"]["#weight"] = "3";

		form["status"]["#fieldset"] = "info";
		form["status"]["#type"] = "radios";
		form["status"]["#title"] = "Status";
		form["status"]["#value"] = item["status"];
		form["status"]["#options"] = "Blocked,Active";
		form["status"]["#weight"] = "4";

	form["info2"]["#type"] = "fieldset";
	form["info2"]["#title"] = "Signature settings";
	form["info2"]["#collapsible"] = _FALSE;
	form["info2"]["#collapsed"] = _FALSE;
	form["info2"]["#weight"] = "2";

		form["signature"]["#type"] = "textfield";
		form["signature"]["#fieldset"] = "info2";
		form["signature"]["#title"] = "Signature";
		form["signature"]["#value"] = item["signature"];
		form["signature"]["#description"] = "Your signature will be publicly displayed at the end of your comments.";
		form["signature"]["#weight"] = "2";

	form["info3"]["#type"] = "fieldset";
	form["info3"]["#title"] = "Theme configuration";
	form["info3"]["#description"] = "Selecting a different theme will change the look and feel of the site.";
	form["info3"]["#collapsible"] = _TRUE;
	form["info3"]["#collapsed"] = _FALSE;
	form["info3"]["#weight"] = "3";

		form["theme"]["#type"] = "table";
		form["theme"]["#fieldset"] = "info3";		
		form["theme"]["#header"] = "Screenshot,Name,Selected";
		form["theme"]["#weight"] = "5";

	size_t i;
	string options;
	string default_theme = variable_get("theme_default", "admin");

	for( i = 0; i < _THEMES.size(); i++ )
		options += " ,";
	
	for( i = 0; i < _THEMES.size(); i++ ) 
	{ 
		form["screen_"+str(i)]["#type"] = "markup";
		form["screen_"+str(i)]["#value"] = "<img src=\""+string(BASE_URL)+"/themes/"+_THEMES[i]+"/screenshot.png\">";
		form["screen_"+str(i)]["#table"] = "theme";

		form["theme_name_"+str(i)]["#type"] = "markup";
		form["theme_name_"+str(i)]["#value"] = "<b>"+_THEMES[i]+"</b>";
		form["theme_name_"+str(i)]["#table"] = "theme";
		
		form["selected_"+str(i)]["#type"] = "radio";
		form["selected_"+str(i)]["#name"] = "selected_theme";
		form["selected_"+str(i)]["#options"] = options;
		form["selected_"+str(i)]["#value"] = str(i);
		form["selected_"+str(i)]["#attributes"] = "align=\"center\"";
		form["selected_"+str(i)]["#table"] = "theme";
		
		if( _THEMES[i] == default_theme) 
			form["theme_name_"+str(i)]["#value"] += "<br><i>(site default theme)</i>";

		if( _THEMES[i] == item["theme"] ) 
			form["selected_"+str(i)]["#checked"] = _TRUE;

		form["theme"]["#row"+str(i)] = "screen_"+str(i)+",theme_name_"+str(i)+",selected_"+str(i);
	}		
			
	form["submit"]["#type"] = "submit";
	form["submit"]["#value"] = "Submit";
	form["submit"]["#weight"] = "13";

	form["delete"]["#type"] = "submit";
	form["delete"]["#value"] = "Delete";
	form["delete"]["#weight"] = "14";

	return get_form("user_edit", form);
}

string user_edit_submit()
{
	vector <string> users_roles;
	unnest( &_POST, "roles", users_roles );
	int theme = intval( _POST["selected_theme"] );

	if( DB_TYPE == 1 )
	{
		int uid = intval(arg(1));
		redis_command("DEL users:%d:roles");
		for(size_t i=0; i<users_roles.size(); i++) {
			redis_command("LPUSH users:%d:roles %s", uid, users_roles[i].c_str() );
		}

		if( isset( _POST["pass1"] ) ) {
			redis_command("HSET users:%d pass %s", uid, md5( _POST["pass1"] ).c_str() );
		}

		redis_command("HSET users:name %s %d", _POST["name"].c_str(), uid );
		redis_command("HMSET users:%d name %s mail %s status %s theme %s signature %s",
			 uid, _POST["name"].c_str(), _POST["mail"].c_str(), _POST["status"].c_str(), _THEMES[ theme ].c_str(), _POST["signature"].c_str() );
	}

	if( DB_TYPE == 2 )
	{
		db_querya( "DELETE FROM users_roles WHERE uid=%d", str( intval(arg(1)) ).c_str() );
		for(size_t i=0; i<users_roles.size(); i++) {
			db_querya( "INSERT INTO users_roles (uid, rid) VALUES (%d, %d)", str( intval(arg(1)) ).c_str(), users_roles[i].c_str() );
		}

		if( isset( _POST["pass1"] ) ) {
			db_querya( "UPDATE users SET pass='%s' WHERE uid=%d", md5(_POST["pass1"]).c_str(), str( intval(arg(1)) ).c_str() );
		}
			
		db_querya( "UPDATE users SET name='%s', mail='%s', status=%d, theme='%s', signature='%s' WHERE uid=%d", 
			_POST["name"].c_str(),
			_POST["mail"].c_str(),
			_POST["status"].c_str(),
			_THEMES[ theme ].c_str(),
			_POST["signature"].c_str(),
			str( intval(arg(1)) ).c_str() );
	}

	redirect( url("admin/user") );
	
	return "";
}

bool user_edit_validate()
{
	MYSQL_RES *result;

	string uid = arg( 1 );
	int user_exists = 0;

	if( DB_TYPE == 1 )
	{		
		if( _POST["name"] != redis_str("HGET users:%d name", intval(uid) ) )
			user_exists = redis_int("HEXISTS users:name %s", _POST["name"].c_str() );
	}
	if( DB_TYPE == 2 )
	{
		result = db_querya( "SELECT uid FROM users WHERE name='%s' AND uid<>%d", _POST["name"].c_str(), uid.c_str() );
		user_exists = db_num_rows( result );
	}

	if ( user_exists ) 
	{
		form_set_error("name", "The name "+_POST["name"]+" is already taken."); 
		return false;
	}

	if( !valid_email_address( _POST["mail"] ) )
	{
		form_set_error("mail", "The E-mail "+_POST["mail"]+" is invalid."); 
		return false;
	}
	return true;
}

bool user_admin_validate()
{
	return true;
}

string user_admin_submit()
{
	if(isset(_POST["op"]))
	{
		if( _POST["status"] == "0" )
		{
			map <string, string>::iterator curr, end;

			for( curr = _POST.begin(), end = _POST.end();  curr != end;  curr++ )
			{
				if( curr->first.find("sel-") != curr->first.npos )
				{
					db_querya("UPDATE users SET status=1 WHERE uid=%d", curr->second.c_str());
				}
			}
		}
		if( _POST["status"] == "1" )
		{
			map <string, string>::iterator curr, end;

			for( curr = _POST.begin(), end = _POST.end();  curr != end;  curr++ )
			{
				if( curr->first.find("sel-") != curr->first.npos )
				{
					db_querya("UPDATE users SET status=0 WHERE uid=%d", curr->second.c_str());
				}
			}
		}
		if( _POST["status"] == "2" )
		{
			map <string, string>::iterator curr, end;

			for( curr = _POST.begin(), end = _POST.end();  curr != end;  curr++ )
			{
				if( curr->first.find("sel-") != curr->first.npos )
				{
					db_querya("DELETE FROM users WHERE uid=%d", curr->second.c_str());
				}
			}
		}
	}

	redirect( url("admin/user") );
	return "";
}

string user_register()
{
	map <string, map <string, string> > form;
	
	form["user"]["#type"] = "textfield";
	form["user"]["#title"] = "Username";
	form["user"]["#size"] = "30";
	form["user"]["#weight"] = "1";
	form["user"]["#value"] = _POST["user"];
	form["user"]["#description"] = "Your full name or your preferred username; only letters, numbers and spaces are allowed.";
	form["user"]["#required"] = str( true );

	form["mail"]["#type"] = "textfield";
	form["mail"]["#title"] = "E-mail address";
	form["mail"]["#size"] = "30";
	form["mail"]["#weight"] = "2";
	form["mail"]["#value"] = _POST["user"];
	form["mail"]["#description"] = "A password and instructions will be sent to this e-mail address, so make sure it is accurate.";
	form["mail"]["#required"] = str( true );
	
	form["pass"]["#type"] = "password";
	form["pass"]["#title"] = "Password";
	form["pass"]["#size"] = "30";
	form["pass"]["#weight"] = "3";
	form["pass"]["#description"] = "Provide a password for the new account.";
	form["pass"]["#required"] = str( true );

	form["notify"]["#type"] = "checkbox";
	form["notify"]["#title"] = "Notify user of new account";
	form["notify"]["#weight"] = "4";
					
	form["submit"]["#type"] = "submit";
	form["submit"]["#value"] = "Crete new account";
	form["submit"]["#weight"] = "13";

	return get_form("register", form);
}

string user_register_submit()
{
	if( DB_TYPE == 1 )
	{
		int uid = redis_int("INCR users:ids");
		redis_command("SADD users %d", uid);
		redis_command("HSET users:name %s %d", _POST["user"].c_str(), uid );
		redis_command("HMSET users:%d name %s mail %s created %d pass %s status 1 theme %s",
			 uid, _POST["user"].c_str(), _POST["mail"].c_str(), time(), md5( _POST["pass"] ).c_str(), variable_get("theme_default", "admin").c_str() );
	}
	if( DB_TYPE == 2 )
	{
		db_querya("INSERT INTO users (name, mail, pass, created, timezone, theme, status) VALUES('%s', '%s', '%s', %d, 0, '%s', 1)",
			_POST["user"].c_str(),
			_POST["mail"].c_str(),
			md5(_POST["pass"]).c_str(),
			str(time()).c_str(),
			variable_get("theme_default", "admin").c_str() );
	}

	redirect( url("admin/user") );

	return "";
}

bool user_register_validate()
{
	int user_exists = 0;

	if( DB_TYPE == 1 )
	{
		user_exists = redis_int("HEXISTS users:name %s", _POST["user"].c_str() );
	}
	if( DB_TYPE == 2 )
	{
		MYSQL_RES *result;
		result = db_querya( "SELECT uid FROM users WHERE name='%s'", _POST["user"].c_str() );
		user_exists = db_num_rows( result );
	}

	if ( user_exists ) 
	{
		form_set_error("name", "The name "+_POST["user"]+" is already taken."); 
		return false;
	}

	if( !valid_email_address( _POST["mail"] ) )
	{
		form_set_error("mail", "The E-mail "+_POST["mail"]+" is invalid."); 
		return false;
	}

	return true;
}

string user_admin()
{
	string out;
	map <string, string> item;
	map <string, map<string,string> > form;
	MYSQL_RES *result;

	set_page_title( "users" );

	int i;

	vector <pair <string, string> > options;
	options.push_back( make_pair("0","Unblock the selected users") );
	options.push_back( make_pair("1","Block the selected users") );
	options.push_back( make_pair("2","Delete the selected users") );

	form["options"]["#type"] = "fieldset";
	form["options"]["#title"] = "Update options";
	form["options"]["#prefix"] = "<div class=\"container-inline\">";
	form["options"]["#suffix"] = "</div>";
	form["options"]["#weight"] = "1";

	form["status"]["#type"] = "select";
	form["status"]["#options"] = serialize_array( options );
	form["status"]["#weight"] = "1";
	form["status"]["#fieldset"] = "options";

	form["update"]["#type"] = "submit";
	form["update"]["#value"] = "Update";
	form["update"]["#weight"] = "2";
	form["update"]["#fieldset"] = "options";

	form["users"]["#type"] = "table";
	form["users"]["#header"] = "select-all,Username,Status,Member for,Last access,Operations";
	form["users"]["#weight"] = "3";

	if( DB_TYPE == 1 )
	{
		i = 0;
		REDIS_RES *rr = redis_query_fields("SORT users BY users:*->access DESC", "GET users:*->", "#uid,name,mail,created,access,status");
		while( redis_fetch_fields( rr, item ) )
		{
			form["F"+str(i)]["#type"] = "checkbox";
			form["F"+str(i)]["#name"] = "sel-"+item["uid"];
			form["F"+str(i)]["#value"] = item["uid"];
			form["F"+str(i)]["#table"] = "users";

			form["A"+str(i)]["#type"] = "markup";
			form["A"+str(i)]["#value"] = "<a href=\""+url("user/"+item["uid"]+"/edit")+"\">"+item["name"]+"</a>";
			form["A"+str(i)]["#table"] = "users";

			form["B"+str(i)]["#type"] = "markup";
			form["B"+str(i)]["#value"] = (item["status"]=="1") ? "active" : "blocked";
			form["B"+str(i)]["#table"] = "users";

			form["C"+str(i)]["#type"] = "markup";
			form["C"+str(i)]["#value"] = format_interval( time()-intval(item["created"]) );
			form["C"+str(i)]["#table"] = "users";

			form["D"+str(i)]["#type"] = "markup";
			form["D"+str(i)]["#value"] = isset(item["access"]) ? format_interval(time() - intval(item["access"])) : "never";
			form["D"+str(i)]["#table"] = "users";

			form["E"+str(i)]["#type"] = "markup";
			form["E"+str(i)]["#value"] = "<a href=\""+url("user/"+item["uid"]+"/edit")+"\">edit</a>";
			form["E"+str(i)]["#table"] = "users";
			
			form["users"]["#row"+str(i)] = "F"+str(i)+",A"+str(i)+",B"+str(i)+",C"+str(i)+",D"+str(i)+",E"+str(i);

			i++;
		}	
	}
	
	if( DB_TYPE == 2 )
	{
		if( user["uid"] == "1" )
			result = pager_query( sql_parse("SELECT uid, name, created, access, status FROM users WHERE uid<>0 ORDER by access DESC"), 100 );
		else
			result = pager_query( sql_parse("SELECT uid, name, created, access, status FROM users WHERE uid>1 ORDER by access DESC"), 100 );
		
		i = 0;
		while( db_fetch( result, item ) )
		{
			form["F"+str(i)]["#type"] = "checkbox";
			form["F"+str(i)]["#name"] = "sel-"+item["uid"];
			form["F"+str(i)]["#value"] = item["uid"];
			form["F"+str(i)]["#table"] = "users";

			form["A"+str(i)]["#type"] = "markup";
			form["A"+str(i)]["#value"] = "<a href=\""+url("user/"+item["uid"]+"/edit")+"\">"+item["name"]+"</a>";
			form["A"+str(i)]["#table"] = "users";

			form["B"+str(i)]["#type"] = "markup";
			form["B"+str(i)]["#value"] = (item["status"]=="1") ? "active" : "blocked";
			form["B"+str(i)]["#table"] = "users";

			form["C"+str(i)]["#type"] = "markup";
			form["C"+str(i)]["#value"] = format_interval( time()-intval(item["created"]) );
			form["C"+str(i)]["#table"] = "users";

			form["D"+str(i)]["#type"] = "markup";
			form["D"+str(i)]["#value"] = isset(item["access"]) ? format_interval(time() - intval(item["access"])) : "never";
			form["D"+str(i)]["#table"] = "users";

			form["E"+str(i)]["#type"] = "markup";
			form["E"+str(i)]["#value"] = "<a href=\""+url("user/"+item["uid"]+"/edit")+"\">edit</a>";
			form["E"+str(i)]["#table"] = "users";
			
			form["users"]["#row"+str(i)] = "F"+str(i)+",A"+str(i)+",B"+str(i)+",C"+str(i)+",D"+str(i)+",E"+str(i);

			i++;
		}
	}

	out += get_form("user_edit", form);

	out += theme_pager();

	return out;
}

string user_admin_roles_edit()
{
	string rid = arg(3);
	string	out, name;
	map <string, map <string, string> > form;

	if( DB_TYPE == 1 )
	{
		name = redis_str( "ZRANGEBYSCORE role %d %d", intval(rid), intval(rid) );
	}
	if( DB_TYPE == 2 )
	{
		name = db_result( db_querya("SELECT name FROM role WHERE rid=%d", rid.c_str() ) );
	}

	form["new"]["#type"] = "textfield";
	form["new"]["#title"] = "Role name";
	form["new"]["#value"] = name;
	form["new"]["#required"] = _TRUE;
	form["new"]["#description"] = "The name for this role. Example: \"moderator\", \"editorial board\", \"site architect\".";
	form["new"]["#size"] = "30";
	form["new"]["#weight"] = "1";
	
	form["submit"]["#type"] = "submit";
	form["submit"]["#value"] = "Save role";
	form["submit"]["#weight"] = "2";
		
	form["delete"]["#type"] = "submit";
	form["delete"]["#value"] = "Delete role";
	form["delete"]["#weight"] = "3";
	
	out += get_form("roles", form);

	return out;
}

bool user_admin_roles_edit_validate()
{
	return true;
}

string user_admin_roles_edit_submit()
{
	string rid = arg(3);

	if( _POST["op"] == "Save role")
	{
		if( DB_TYPE == 1 )
		{
			redis_command("ZREMRANGEBYSCORE role %d %d", intval(rid), intval(rid) );
			redis_command("ZADD role %d %s", intval(rid), _POST["new"].c_str());
		}
		if( DB_TYPE == 2 )
		{
			db_querya("UPDATE role SET name='%s' WHERE rid=%d", _POST["new"].c_str(), rid.c_str());
		}
	}
	if( _POST["op"] == "Delete role")
	{
		if( DB_TYPE == 1 )
		{
			redis_command("ZREMRANGEBYSCORE role %d %d", intval(rid), intval(rid) );
		}
		if( DB_TYPE == 2 )
		{
			db_querya("DELETE FROM role WHERE rid=%d", rid.c_str());
		}
	}

	redirect( url("admin/roles") );

	return " ";
}

string user_admin_roles()
{
	string	out;
	map <string, map <string, string> > form;
	size_t i = 0;
	
	form["roles"]["#type"] = "table";
	form["roles"]["#header"] = "Name,Operations, ";
	form["roles"]["#weight"] = "1";

	if( DB_TYPE == 1 )
	{
		vector <pair <string, string> > roles;
		redis_multi(roles, "ZRANGE role 0 -1 WITHSCORES");
		for( i = 0; i< roles.size(); i++ )
		{
			string rid = roles[i].second;
			form["name_"+rid]["#type"] = "markup";
			form["name_"+rid]["#value"] = roles[i].first;
			form["name_"+rid]["#table"] = "roles";

			form["opt1_"+rid]["#type"] = "markup";
			form["opt1_"+rid]["#value"] = (rid!=ANONYMOUS_RID && rid!=AUTHENTICATED_RID) ? "<a href=\""+url("admin/roles/edit/"+rid)+"\">edit role</a>" : "locked";
			form["opt1_"+rid]["#table"] = "roles";

			form["opt2_"+rid]["#type"] = "markup";
			form["opt2_"+rid]["#value"] = "<a href=\""+url("admin/permissions/"+rid)+"\">edit permissions</a>";
			form["opt2_"+rid]["#table"] = "roles";

			form["roles"]["#row"+str(i)] = "name_"+rid+",opt1_"+rid+",opt2_"+rid;		
		}
	}

	if( DB_TYPE == 2 )
	{
		MYSQL_RES *result;	
		map <string, string> item;
		result = db_querya("SELECT * FROM role ORDER by rid");
		while( db_fetch( result, item ) )
		{
			string rid = item["rid"];
			form["name_"+rid]["#type"] = "markup";
			form["name_"+rid]["#value"] = item["name"];
			form["name_"+rid]["#table"] = "roles";

			form["opt1_"+rid]["#type"] = "markup";
			form["opt1_"+rid]["#value"] = (rid!=ANONYMOUS_RID && rid!=AUTHENTICATED_RID) ? "<a href=\""+url("admin/roles/edit/"+rid)+"\">edit role</a>" : "locked";
			form["opt1_"+rid]["#table"] = "roles";

			form["opt2_"+rid]["#type"] = "markup";
			form["opt2_"+rid]["#value"] = "<a href=\""+url("admin/permissions/"+rid)+"\">edit permissions</a>";
			form["opt2_"+rid]["#table"] = "roles";

			form["roles"]["#row"+str(i)] = "name_"+rid+",opt1_"+rid+",opt2_"+rid;
			i++;
		}
	}

	form["new"]["#type"] = "textfield";
	form["new"]["#size"] = "30";
	form["new"]["#table"] = "roles";

	form["submit"]["#type"] = "submit";
	form["submit"]["#value"] = "Add role";
	form["submit"]["#attributes"] = "colspan=\"2\"";
	form["submit"]["#table"] = "roles";
	
	form["roles"]["#row"+str(i)] = "new,submit";

	out += get_form("roles", form);

	return out;
}

bool user_admin_roles_validate()
{
	int exists = 0;
	if ( !isset(_POST["new"]) )
	{
		form_set_error("new", "You must specify a valid role name."); 
		return false;
	}

	if(DB_TYPE == 1)
	{
		exists = redis_int("ZSCORE role %s", _POST["new"].c_str());
	}
	if(DB_TYPE == 2)
	{
		exists = db_num_rows( db_querya("SELECT rid FROM role WHERE name='%s'", _POST["new"].c_str() ) );
	}

	if ( exists ) 
	{
		form_set_error("new", "The role name <em>"+_POST["new"]+"</em> already exists. Please choose another role name."); 
		return false;
	}
	return true;
}

string user_admin_roles_submit()
{
	if( isset(_POST["op"]) && _POST["new"].size()>0)
	{
		if(DB_TYPE == 1)
		{
			int rid = redis_int("INCR role:ids");
			redis_command("ZADD role %d %s", rid, _POST["new"].c_str());
		}
		if(DB_TYPE == 2)
		{
			db_querya("INSERT INTO role (name) VALUES ('%s')", _POST["new"].c_str());
		}
	}	
	redirect( url("admin/roles") );
	return " ";
}

string user_admin_permissions()
{
	MYSQL_RES *result;
	map <string, string> item;
	map <string, map<string,string> > form;
	vector <string> roles;	
	vector <string> perms;
	map <string, map<string,string> > enabled;
	string header = "Permission";
	map <string, string> saved;

	if(DB_TYPE == 1)
	{
		REDIS_RES *rr = redis_query("ZRANGE role 0 -1 WITHSCORES");
		while( redis_fetch( rr, item["name"] ) ) {
			redis_fetch( rr, item["rid"] );
			header += "," + item["name"];
			roles.push_back( item["rid"] );
	    }
	}
	
	if(DB_TYPE == 2)
	{
		result = db_querya("SELECT name, rid FROM role ORDER by rid");
		while( db_fetch( result, item ) ) {
			header += "," + item["name"];
			roles.push_back( item["rid"] );
		}
	}

	int n = 0;

	if( isset(_POST["op"]) ) // Save Values
	{
			for( size_t i = 0; i < _MODELS.size(); i++ )
			{
				if( model_exists( _MODELS[i] ) ) {
					perms.clear();
					explode( perms, _PLUGINS_STRINGS[ _MODELS[i] ]["perm"], ',' );

					if(perms.size() > 0) {
						n++;
						
						for(size_t j = 0; j < perms.size(); j++)
						{
							ltrim( perms[ j ] );
							for(size_t k = 0; k < roles.size(); k++)
							{	
								if( _POST["c"+str(n)+"-"+str(k)] == _TRUE )
								{	
									if( isset(saved[ roles[k] ]) ) saved[ roles[k] ] += ", ";
									saved[ roles[k] ] += perms[j];
								}	
							}
							n++;
						}
					}
				}
			}

			if(DB_TYPE == 1)
			{
				for( map <string, string>::iterator curr = saved.begin(), end = saved.end();  curr != end;  curr++ ) {	
					redis_command("HSET permission %d %s", intval( curr->first ), curr->second.c_str() );
				}
			}
			
			if(DB_TYPE == 2)
			{
				for(size_t k = 0; k < roles.size(); k++) {	
					db_querya("DELETE FROM permission WHERE rid=%d", roles[k].c_str() );
				}

				for( map <string, string>::iterator curr = saved.begin(), end = saved.end();  curr != end;  curr++ ) {	
					db_querya("INSERT INTO permission (perm, rid) VALUES ('%s', %d)", curr->second.c_str(), curr->first.c_str() );
				}
			}

			set_page_message( "The changes have been saved.", "status" );
	}

	if(DB_TYPE == 1)
	{
		REDIS_RES *rr = redis_query("HGETALL permission");
		while( redis_fetch( rr, item["rid"] ) ) {
			redis_fetch( rr, item["perm"] );
			perms.clear();
			explode( perms, item["perm"], ',' );
			for(size_t i=0; i<perms.size(); i++)
			{
				ltrim(perms[i]);
				enabled[ item["rid"] ][ perms[i] ] = _TRUE;
			}
	    }
	}

	if(DB_TYPE == 2)
	{
		result = db_querya("SELECT perm, rid FROM permission ORDER by rid");
		while( db_fetch( result, item ) ) {
			perms.clear();
			explode( perms, item["perm"], ',' );
			for(size_t i=0; i<perms.size(); i++)
			{
				ltrim(perms[i]);
				enabled[ item["rid"] ][ perms[i] ] = _TRUE;
			}
		}
	}

	form["permissions"]["#type"] = "table";
	form["permissions"]["#header"] = header;
	form["permissions"]["#attributes"] = "id=\"permissions\"";
	form["permissions"]["#weight"] = "1";

	n = 0;

	for( size_t i = 0; i < _MODELS.size(); i++ )
	{
		if( model_exists( _MODELS[i] ) ) {
			perms.clear();
			explode( perms, _PLUGINS_STRINGS[ _MODELS[i] ]["perm"], ',' );

			if(perms.size() > 0) {
				form["name_"+str(n)]["#type"] = "markup";
				form["name_"+str(n)]["#value"] = _MODELS[ i ] + " model";
				form["name_"+str(n)]["#table"] = "permissions";
				form["name_"+str(n)]["#attributes"] = "class=\"model\" colspan=\""+str(roles.size()+1)+"\"";
				form["permissions"]["#row"+str(n)] = "name_"+str(n);
				n++;
				
				for(size_t j = 0; j < perms.size(); j++)
				{
					ltrim( perms[ j ] );
					form["name_"+str(n)]["#type"] = "markup";
					form["name_"+str(n)]["#value"] = perms[ j ];
					form["name_"+str(n)]["#table"] = "permissions";
					form["name_"+str(n)]["#attributes"] = "class=\"permission\"";
									
					string row = "";

					for(size_t k = 0; k < roles.size(); k++)
					{	
						form["c"+str(n)+"-"+str(k)]["#type"] = "checkbox";
						form["c"+str(n)+"-"+str(k)]["#checked"] = enabled[ roles[k] ][ perms[j] ];
						form["c"+str(n)+"-"+str(k)]["#table"] = "permissions";
						form["c"+str(n)+"-"+str(k)]["#value"] = _TRUE;
						form["c"+str(n)+"-"+str(k)]["#attributes"] = "align=\"center\"";
						row += ",c"+str(n)+"-"+str(k);
					}

					form["permissions"]["#row"+str(n)] = "name_"+str(n)+row;

					n++;
				}
			}
		}
	}

	form["submit"]["#type"] = "submit";
	form["submit"]["#value"] = "Save permissions";
	form["submit"]["#weight"] = "2";

	return get_form("permissions", form);
}

string user_logout()
{
	if(DB_TYPE==1)
	{
		redis_command("DEL sessions:%s", session_id().c_str() );
	}
	if(DB_TYPE==2)
	{
		db_querya("DELETE FROM sessions WHERE sid='%s'", session_id().c_str() );
	}
	
	setcookie("BTSESS", "deleted", str( time()-(3600*24*365) ), "/", _SERVER("HTTP_HOST") );

	redirect( url("login") );

	return "";
}

bool user_access(string arguments)
{
	MYSQL_RES *result;
	map <string, string> item;
	int uid = intval( user["uid"] );

	// User #1 has all privileges:
	if(uid==1) return true;

	if( !isset2(perm, uid) )
	{
		if( DB_TYPE == 1 )
		{	
			string permission;		
			REDIS_RES *rr = redis_query("HMGET permission %s", str_replace(","," ", user["roles"]).c_str() );
			while( redis_fetch( rr, permission ) ) {
				vector <string> roles;
				explode(roles, permission );
				for(size_t i=0; i<roles.size(); i++)
				{
					ltrim( roles[i] );
					perm[uid][ roles[i] ] = true;
				}
			}
		}
		if( DB_TYPE == 2 )
		{		
			result = db_querya( "SELECT p.perm FROM role r INNER JOIN permission p ON p.rid = r.rid WHERE r.rid IN (%s)", user["roles"].c_str() );
			while( db_fetch( result, item ) )
			{
				vector <string> roles;
				explode(roles, item["perm"]);
				for(size_t i=0; i<roles.size(); i++)
				{
					ltrim( roles[i] );
					perm[uid][ roles[i] ] = true;
				}
			}
		}
	}

	//print ( arguments );
	//print ( str( perm[uid][arguments] )+"<br>" );

	return (perm[uid][arguments]);
}

bool user_is_not_authenticated(string arguments)
{
	return !user_is_authenticated();
}

bool user_is_authenticated(string arguments)
{
	if(!isset(user["uid"])) return false;

	if(user["uid"]=="0") return false;
	
	return true;
}

bool user_login_validate(void)
{
	return true;
}

void user_login_by_uid(string uid)
{
	string timestamp = str( time() );
	string sid = md5 ( microtime() );
	string hostname = getenv("REMOTE_ADDR") != NULL ? string( getenv("REMOTE_ADDR") ) : "";

	if( DB_TYPE == 1 )
	{
		while( redis_int("EXISTS sessions:%s", sid.c_str()) == 1 ) {
			sid = md5 ( microtime() );
		}
		redis_command("HMSET sessions:%s uid %d hostname %s", sid.c_str(), intval(uid), hostname.c_str() );
	}

	if( DB_TYPE == 2 )
	{	
		db_querya("INSERT INTO sessions (sid, uid, hostname, timestamp, session) VALUES ('%s', %d, '%s', %d, '')", sid.c_str(), uid.c_str(), hostname.c_str(), timestamp.c_str() );
		while( db_affected_rows() == 0 )
		{
			sid = md5 ( microtime() );
			db_querya("INSERT INTO sessions (sid, uid, hostname, timestamp, session) VALUES ('%s', %d, '%s', %d, '')", sid.c_str(), uid.c_str(), hostname.c_str(), timestamp.c_str() );
		}
	}

	setcookie("BTSESS", sid, str( time()+cookie_lifetime ), "/", getenv("HTTP_HOST") ? string(getenv("HTTP_HOST")) : "");

	if( isset( _POST["rurl"] ) ) redirect( url( _POST["rurl"] ) );
	else redirect( url("") );
}

string user_login_submit(void)
{
	MYSQL_RES *result;

	bool error = true;
	string pass = md5(_POST["pass"]);
	string user = _POST["user"];

	if( DB_TYPE == 1 )
	{
		string	uid;
		REDIS_RES * rr = redis_query("HGET users:name %s", user.c_str() );
		if( redis_fetch( rr, uid ) )
		{
			if( pass == redis_str("HGET users:%d pass", intval(uid) ) && redis_int("HGET users:%d status", intval(uid)) == 1 )
			{
				user_login_by_uid( uid );
				error = false;
			}
		}
	}

	if( DB_TYPE == 2 )
	{
		result = db_querya("SELECT uid FROM users WHERE status = 1 AND name = '%s' AND pass = '%s'", user.c_str(), pass.c_str());
		if ( db_num_rows( result ) ) 
		{
			string uid = db_result( result );
			user_login_by_uid( uid );
			error = false;
		}
	}

	if( error )
	{
		form_set_error("name", "The <i>email address</i> and <i>password</i> you entered do <i>not match</i> with any user."); 
	}
	
	return "";
}

string user_login(void)
{
	map <string, map<string,string> > form;

	form["rurl"]["#type"] = "hidden";
	form["rurl"]["#value"] = _POST["rurl"];

	form["user"]["#type"] = "textfield";
	form["user"]["#title"] = "Email";
	form["user"]["#size"] = "14";
	form["user"]["#weight"] = "-1";
	form["user"]["#value"] = _POST["user"];
	form["user"]["#description"] = "Enter your username.";
	form["user"]["#required"] = _TRUE;
	form["user"]["#required_msg"] = t("Incorrect <i>email address</i>. The email address you entered does not belong to any account. Make sure that it is typed correctly.");
	
	form["pass"]["#type"] = "password";
	form["pass"]["#title"] = "Password";
	form["pass"]["#size"] = "14";
	form["pass"]["#weight"] = "1";
	form["pass"]["#description"] = "Enter the password that accompanies your username.";
	form["pass"]["#required"] = _TRUE;
	form["pass"]["#required_msg"] = t("The <i>password</i> you entered is incorrect. Please try again (make sure your caps lock is off).");
	
	form["submit"]["#type"] = "submit";
	form["submit"]["#value"] = "Login";
	form["submit"]["#weight"] = "13";
	
	add_js("etc/jquery.js");

	return get_form("user", form);
}

void user_load( map <string, string> &account )
{
	if(DB_TYPE==1)
	{
		int uid = intval( account["uid"] );
		if( isset(account["name"]) ) {
			uid = redis_int("HGET users:name %s", account["name"].c_str() );
		}
		if( uid > 0 ) {
			redis_fetch_fields( redis_query_fields(redis_arg("HMGET users:%d", uid), "", "name,mail,created,access,theme,picture,picture_status,signature,data" ), account );
			account["uid"] = str( uid );
		}
	}
	if(DB_TYPE==2)
	{
		map <string, string> role;
		MYSQL_RES *result;
		string	query = "SELECT u.* FROM users u WHERE ";

		for( map <string, string>::iterator i = account.begin(), end = account.end();  i != end;  i++ ) 
		{
			if( i != account.begin() ) query += " AND ";
			query += i->first+"='"+i->second+"'";
		}

		result = db_querya( query.c_str() );
		db_fetch( result, account );
		
		result = db_querya("SELECT * FROM users_roles WHERE uid = %d", account["uid"].c_str());
		while ( db_fetch( result, role ) ) {
			account["roles"] += "," + role["rid"];
		}
	}
}

string user_autoverify()
{
	map <string, string> tag;
	string item = arg(2);
	string str = arg(3);
	string out, list;

	trim(str);

	if( item == "first" || item == "last")
	{
		if( str.length() < 2 )
			out = "{ \"found\":1 }";
		else
			out = "{ \"found\":0 }";		
	}

	if( item == "pass" )
	{
		if( str.length() < 6 )
			out = "{ \"found\":1 }";
		else
			out = "{ \"found\":0 }";
	}

	if( item == "email" )
	{
		if( str.length() < 3 )
		{
			out = "{ \"found\":1 }";
		}
		else
		{	
			string name = db_result( db_querya("SELECT name FROM users WHERE name='%s' OR mail='%s'", str.c_str(), str.c_str() ) );

			if ( !isset(name) )
			{
				if( valid_email_address( str ) )
					out = "{ \"found\":0 }";
				else
					out = "{ \"found\":1 }";
			}
			else
				out = "{ \"found\":1 }";
		}
	}

	if( item == "city" )
	{
		if( str.length() < 1 )
			out = "{ \"found\":1 }";
		else
			out = "{ \"found\":0 }";
	}

	print( out ) ;

	return "";
}
