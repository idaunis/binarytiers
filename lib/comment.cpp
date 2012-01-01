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
#include "comment.h"

void comment_hooks()
{
	_MAP_DESC( comment, "Allows users to comment on and discuss published content." );
	_MAP_VER ( comment, "1.3.1" );
	_MAP_HOOK( comment, schema );
	_MAP_HOOK( comment, delete );
	// _MAP_HOOK( comment, insert );
	_MAP_HOOK( comment, controllers );
	_MAP_HOOK( comment, form_alter );
}

void comment_controllers()
{
	map <string, string> item;

	item["path"] = "comment/popup/%";
	item["title"] = "Send Comment";
	item["callback"] = _MAP_FORM( comment_popup );
	item["type"] = str( ~MENU_VISIBLE_IN_TREE );
	poke( controllers, item );

	item["path"] = "comment/edit/%";
	item["title"] = "Edit Comment";
	item["callback"] = _MAP_FORM( comment_edit );
	item["access arguments"] = "administer nodes";
	item["type"] = str( ~MENU_VISIBLE_IN_TREE );
	item["parent"] = "admin/comment";
	poke( controllers, item );

	item["path"] = "admin/comment";
	item["title"] = "Comments";
	item["callback"] = _MAP( comment_admin );
	item["access arguments"] = "administer nodes";
	item["parent tab"] = "admin/comment";
	poke( controllers, item );

	item["path"] = "admin/comment/approval";
	item["title"] = "Approval queue";
	item["weight"] = "2";
	item["callback"] = _MAP( comment_approval );
	item["type"] = str( MENU_LOCAL_TASK );
	item["access arguments"] = "administer nodes";
	item["parent tab"] = "admin/comment";
	poke( controllers, item );

	item["path"] = "admin/comment";
	item["title"] = "Published comments";
	item["weight"] = "1";
	item["type"] = str( MENU_LOCAL_TASK );
	item["callback"] = _MAP( comment_admin );
	item["access arguments"] = "administer nodes";
	item["parent tab"] = "admin/comment";
	poke( controllers, item );
}

void comment_schema()
{
	map <string, map<string, string> > comments;

	comments["#spec"]["name"] = "comments";
	comments["#spec"]["description"] = "Stores comments and associated data.";
    comments["cid"]["type"] = "serial";
    comments["cid"]["not null"] = _TRUE;
	comments["cid"]["description"] = "Primary Key: Unique comment ID.";
	comments["pid"]["type"] = "int";
	comments["pid"]["not null"] = _TRUE;
	comments["pid"]["default"] = "0";
	comments["pid"]["description"] = "The {comments}.cid to which this comment is a reply. If set to 0, this comment is not a reply to an existing comment.";
	comments["nid"]["type"] = "int";
	comments["nid"]["not null"] = _TRUE;
	comments["nid"]["default"] = "0";
	comments["nid"]["description"] = "The {node}.nid to which this comment is a reply.";
	comments["uid"]["type"] = "int";
	comments["uid"]["not null"] = _TRUE;
	comments["uid"]["default"] = "0";
	comments["uid"]["description"] = "The {users}.uid who authored the comment. If set to 0, this comment was created by an anonymous user.";
	comments["subject"]["type"] = "varchar";
	comments["subject"]["length"] = "64";
	comments["subject"]["not null"] = _TRUE;
	comments["subject"]["default"] = "";
	comments["subject"]["description"] = "The comment title.";
	comments["comment"]["type"] = "text";
	comments["comment"]["not null"] = _TRUE;
	comments["comment"]["size"] = "big";
	comments["comment"]["description"] = "The comment body.";
	comments["hostname"]["type"] = "varchar";
	comments["hostname"]["length"] = "128";
	comments["hostname"]["not null"] = _TRUE;
	comments["hostname"]["default"] = "";
	comments["hostname"]["description"] = "The author's host name.";
	comments["timestamp"]["type"] = "int";
	comments["timestamp"]["not null"] = _TRUE;
	comments["timestamp"]["default"] = "0";
	comments["timestamp"]["description"] = "The time that the comment was created, or last edited by its author, as a Unix timestamp.";
	comments["status"]["type"] = "int";
	comments["status"]["unsigned"] = _TRUE;
	comments["status"]["not null"] = _TRUE;
	comments["status"]["default"] = "0";
	comments["status"]["size"] = "tiny";
	comments["status"]["description"] = "The published status of a comment. (0 = Published, 1 = Not Published)";
	comments["format"]["type"] = "int";
	comments["format"]["size"] = "small";
	comments["format"]["not null"] = _TRUE;
	comments["format"]["default"] = "0";
	comments["format"]["description"] = "The {filter_formats}.format of the comment body.";
	comments["thread"]["type"] = "varchar";
	comments["thread"]["length"] = "255";
	comments["thread"]["not null"] = _TRUE;
	comments["thread"]["description"] = "The vancode representation of the comment's place in a thread.";
	comments["name"]["type"] = "varchar";
	comments["name"]["length"] = "60";
	comments["name"]["not null"] = _FALSE;
	comments["name"]["description"] = "The comment author's name. Uses {users}.name if the user is logged in, otherwise uses the value typed into the comment form.";
	comments["mail"]["type"] = "varchar";
	comments["mail"]["length"] = "64";
	comments["mail"]["not null"] = _FALSE;
	comments["mail"]["description"] = "The comment author's e-mail address from the comment form, if user is anonymous, and the 'Anonymous users may/must leave their contact information' setting is turned on.";
	comments["homepage"]["type"] = "varchar";
	comments["homepage"]["length"] = "255";
	comments["homepage"]["not null"] = _FALSE;
	comments["homepage"]["description"] = "The comment author's home page address from the comment form, if user is anonymous, and the 'Anonymous users may/must leave their contact information' setting is turned on.";
	comments["#indexes"]["pid"] = "pid";
	comments["#indexes"]["nid"] = "nid";
	comments["#indexes"]["status"] = "status"; // This index is probably unused
	comments["#primary key"]["cid"] = "cid";
	schema.push_back( comments );

	map <string, map<string, string> > node_comment_statistics;
	node_comment_statistics["#spec"]["name"] = "node_comment_statistics";
	node_comment_statistics["#spec"]["description"] = "Maintains statistics of node and comments posts to show \"new\" and \"updated\" flags.";
    node_comment_statistics["nid"]["type"] = "int";
    node_comment_statistics["nid"]["unsigned"] = _TRUE;
    node_comment_statistics["nid"]["not null"] = _TRUE;
    node_comment_statistics["nid"]["default"] = "0";
    node_comment_statistics["nid"]["description"] = "The {node}.nid for which the statistics are compiled.";
	node_comment_statistics["last_comment_timestamp"]["type"] = "int";
    node_comment_statistics["last_comment_timestamp"]["not null"] = _TRUE;
    node_comment_statistics["last_comment_timestamp"]["default"] = "0";
    node_comment_statistics["last_comment_timestamp"]["description"] = "The Unix timestamp of the last comment that was posted within this node, from {comments}.timestamp.";
	node_comment_statistics["last_comment_name"]["type"] = "varchar";
	node_comment_statistics["last_comment_name"]["length"] = "60";
	node_comment_statistics["last_comment_name"]["not null"] = _FALSE;
	node_comment_statistics["last_comment_name"]["description"] = "The name of the latest author to post a comment on this node, from {comments}.name.";
	node_comment_statistics["last_comment_uid"]["type"] = "int";
	node_comment_statistics["last_comment_uid"]["not null"] = _TRUE;
	node_comment_statistics["last_comment_uid"]["default"] = "0";
	node_comment_statistics["last_comment_uid"]["description"] = "The user ID of the latest author to post a comment on this node, from {comments}.uid.";
	node_comment_statistics["comment_count"]["type"] = "int";
	node_comment_statistics["comment_count"]["unsigned"] = _TRUE;
	node_comment_statistics["comment_count"]["not null"] = _TRUE;
	node_comment_statistics["comment_count"]["default"] = "0";
	node_comment_statistics["comment_count"]["description"] = "The total number of comments on this node.";
	node_comment_statistics["#primary key"]["nid"] = "nid";
	node_comment_statistics["#indexes"]["node_comment_timestamp"] = "last_comment_timestamp";
	schema.push_back( node_comment_statistics );
}

void comment_delete()
{
	db_querya("DELETE FROM node_comment_statistics WHERE nid=%d", cur_node["nid"].c_str() );
	db_querya("DELETE FROM comments WHERE nid=%d", cur_node["nid"].c_str() );
}

void comment_insert()
{
	db_querya("INSERT INTO node_comment_statistics (nid, last_comment_timestamp, last_comment_name, last_comment_uid, comment_count) VALUES (%d, %d, NULL, %d, 0)",
			cur_node["nid"].c_str(), str( time() ).c_str(), cur_node["uid"].c_str() );
}

string comment_popup()
{
	string	out;
	string	prefix;
	map <string, map<string,string> > form;

	string key = variable_get("recaptcha_public", "");
	
	prefix = "<script>\n\
var RecaptchaOptions = {\n\
   theme: 'custom',\n\
   lang: 'en',\n\
   custom_theme_widget: 'recaptcha_widget'\n\
};\n\
\n\
</script>\n\
<div id=\"recaptcha_widget\" style=\"display:none\">\n\
<div id=\"recaptcha_image\" style=\"margin-top:8px\"></div>\n\
<span class=\"recaptcha_only_if_image\">Enter the words above:</span>\n\
<input type=\"text\" id=\"recaptcha_response_field\" name=\"recaptcha_response_field\" />"+_POST["message"]+"\n\
</div>\n\
";
	form["name"]["#type"] = "textfield";
	form["name"]["#title"] = "Name";
	form["name"]["#weight"] = "1";
	form["name"]["#size"] = "93";

	form["comment"]["#type"] = "textarea";
	form["comment"]["#title"] = "Comment";
	form["comment"]["#weight"] = "2";
	form["comment"]["#rows"] = "4";
	form["comment"]["#cols"] = "70";

	form["submit"]["#prefix"] = prefix+"<script type=\"text/javascript\" src=\"http://api.recaptcha.net/challenge?k="+key+"&lang=en\"></script>";
	form["submit"]["#type"] = "submit";
	form["submit"]["#attributes"] = "style=\"margin-top:18px\" onclick=\"this.disabled=true;this.form.submit();\"";
	form["submit"]["#value"] = "Send comment";
	form["submit"]["#weight"] = "3";
 
	out += "<html><body style=\"background:#fff;color: #333333;font: 76% Arial, Helvetica, sans-serif;padding:0px 16px 0px 16px\">\n\
		<style type=\"text/css\" media=\"all\">\n\
.form-item {width:60px;margin-top:6px}\n\
</style>\n";
	out += get_form("comment_popup", form);
	out += "</body></html>";

	print (out);
	return "";
}

bool comment_popup_validate()
{
	return true;
}

void comment_mail(string name, string ip, string comment, string nid)
{	
	string message;
	string subject = variable_get("site_name", "BinaryTiers") + " [Comment]";
	string title;

	title = db_result( db_querya("SELECT title FROM node WHERE nid=%d", nid.c_str() ) );

	message = "\
New comment on \""+title+"\"\r\n\
Author : "+name+" (IP: "+ip+")\r\n\
Whois  : http://ws.arin.net/cgi-bin/whois.pl?queryinput="+ip+"\r\n\
Comment: "+comment+"\r\n\
"+url("admin/comment/approval");

	sendmail( SENDMAIL_FROM, variable_get("site_mail", SENDMAIL_FROM), subject, message );
}

string comment_popup_submit()
{
	map <string, string> headers;
	map <string, string> result;
	string data;
	string nid = arg(2);
	string key = variable_get("recaptcha_private", "");
	
	data = "privatekey="+key+"&remoteip="+string(getenv("REMOTE_ADDR"));
	data+= "&challenge="+_POST["recaptcha_challenge_field"]+"&response="+urlencode(_POST["recaptcha_response_field"]);

	headers["Content-Type"] = "application/x-www-form-urlencoded;";
	http_request( result, "http://api-verify.recaptcha.net/verify", headers, "POST", data );

	if( result["data"].substr(0, 5) == "false" )
	{
		_POST["message"] = "&nbsp;&nbsp;<span style=\"color:#ff0000\">Incorrect Words!</span>";
	}
	else
	{
		if(DB_TYPE==1)
		{
			map <string, string> item;
			item["cid"] = redis_str("INCR comments:ids");
			int cid = intval( item["cid"] );
			redis_command("SADD comments %d", cid );
			redis_command("SADD comments:nid:%d %d", intval(nid), cid );
			redis_command("SADD comments:status:1 %d", cid );
			redis_command_fields(redis_arg("HMSET comments:%d", cid ), "",
				"name,pid,mail,homepage,hostname,timestamp,comment,thread,nid,status", "%s,%d,%s,%s,%s,%d,%s,%s,%d,%d",
				_POST["name"].c_str(),
				0,
				"",
				"",
				getenv("REMOTE_ADDR"),
				time(),
				_POST["comment"].c_str(),
				"01/",
				intval(nid),
				1 );
		}
		if(DB_TYPE==2)
		{
			db_querya( "INSERT INTO comments (name, pid, mail, homepage, hostname, timestamp, comment, thread, nid, status) VALUES ('%s', %d, '%s', '%s', '%s', %d, '%s', '01/', %d, 1)",
				_POST["name"].c_str(),
				"0",
				"",
				"",
				getenv("REMOTE_ADDR"),
				str( time() ).c_str(),
				_POST["comment"].c_str(),
				nid.c_str() );
		}

		print("<script type=\"text/javascript\">parent.closeComment();</script>");

		comment_mail(_POST["name"], getenv("REMOTE_ADDR"), _POST["comment"], nid);
		
		cur_form["name"]["#value"] = "";
		cur_form["comment"]["#value"] = "";
	}

	return "";
}

void comment_form_alter()
{
	if( cur_form_id == "add-page" ) {
		map <string, string> node;
		
		node = cur_form["#node"];

		cur_form["comment_settings"]["#type"] = "fieldset";
		cur_form["comment_settings"]["#title"] = "Comment settings";
		cur_form["comment_settings"]["#collapsible"] = _TRUE;
		cur_form["comment_settings"]["#collapsed"] = _TRUE;
		cur_form["comment_settings"]["#weight"] = "20";

		cur_form["comment"]["#type"] = "radios";
		cur_form["comment"]["#options"] = "Disabled,Read only,Read/Write";
		cur_form["comment"]["#value"] = node["comment"];
		cur_form["comment"]["#fieldset"] = "comment_settings";
	}
}

string comment_approval()
{
	string out;
	map <string, string> item;
	map <string, map<string,string> > form;
	
	if( isset(_POST["op"]) )
	{
		if( _POST["status"] == "0" )
		{
			for( map <string, string>::iterator curr = _POST.begin(), end = _POST.end();  curr != end;  curr++ )
			{
				if( curr->first.find("sel-") != curr->first.npos )
				{
					if(DB_TYPE==1) {
						int cid = intval(curr->second);
						redis_command("SREM comments:status:1 %d", cid );
						redis_command("SADD comments:status:0 %d", cid );
						redis_command("HSET comments:%d status 2", cid );
					}
					if(DB_TYPE==2)
						db_querya("UPDATE comments SET status=0 WHERE cid=%d", curr->second.c_str());
				}
			}
		}
		if( _POST["status"] == "1" )
		{
			for( map <string, string>::iterator curr = _POST.begin(), end = _POST.end();  curr != end;  curr++ )
			{
				if( curr->first.find("sel-") != curr->first.npos )
				{
					if(DB_TYPE==1) {
						int cid = intval(curr->second);
						int nid = redis_int("HGET comments:%d nid", cid );
						redis_command("SREM comments %d", cid );
						redis_command("SREM comments:nid:%d %d", nid, cid );
						redis_command("SREM comments:status:0 %d", cid );
						redis_command("SREM comments:status:1 %d", cid );
						redis_command("DEL comments:%d", cid );
					}
					if(DB_TYPE==2)
						db_querya("DELETE FROM comments WHERE cid=%d", curr->second.c_str());
				}
			}
		}
	}
	
	vector <pair <string, string> > options;
	options.push_back( make_pair("0","Publish selected comments") );
	options.push_back( make_pair("1","Delete selected comments") );

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

	form["comments"]["#type"] = "table";
	form["comments"]["#header"] = "select-all,Subject,Author,Posted In,Date,Operations";
	form["comments"]["#weight"] = "3";
	
	int i = 0;

	if(DB_TYPE==1)
	{
		REDIS_RES *result;
		result = redis_pager_fields("SORT comments:status:1 BY comments:*->timestamp DESC", "GET comments:*->", "#cid,nid,subject,name,comment,timestamp", 50);
		while( redis_fetch_fields( result, item ) )
		{
			item["title"] = redis_str("HGET node:%d title", intval(item["nid"]) );
			string subject = item["subject"] + item["comment"];
			subject = subject.substr(0, 100);

			form["F"+str(i)]["#type"] = "checkbox";
			form["F"+str(i)]["#name"] = "sel-"+item["cid"];
			form["F"+str(i)]["#value"] = item["cid"];
			form["F"+str(i)]["#table"] = "comments";

			form["A"+str(i)]["#type"] = "markup";
			form["A"+str(i)]["#value"] = "<a href=\""+url("comment/edit/"+item["cid"])+"\">"+subject+"</a>";
			form["A"+str(i)]["#table"] = "comments";

			form["B"+str(i)]["#type"] = "markup";
			form["B"+str(i)]["#value"] = item["name"];
			form["B"+str(i)]["#table"] = "comments";

			form["C"+str(i)]["#type"] = "markup";
			form["C"+str(i)]["#value"] = item["title"];
			form["C"+str(i)]["#table"] = "comments";

			form["D"+str(i)]["#type"] = "markup";
			form["D"+str(i)]["#value"] = date("%m-%d-%Y", item["timestamp"]);
			form["D"+str(i)]["#table"] = "comments";

			form["E"+str(i)]["#type"] = "markup";
			form["E"+str(i)]["#value"] = "<a href=\""+url("comment/edit/"+item["cid"])+"\">edit</a>";
			form["E"+str(i)]["#table"] = "comments";
			
			form["comments"]["#row"+str(i)] = "F"+str(i)+",A"+str(i)+",B"+str(i)+",C"+str(i)+",D"+str(i)+",E"+str(i);
			i++;
		}
	}

	if(DB_TYPE==2)
	{
		MYSQL_RES *result;
		result = pager_query( sql_parse("SELECT c.cid, c.subject, c.name, c.comment, c.timestamp, n.title FROM comments c LEFT JOIN node n ON n.nid=c.nid WHERE c.status=1 ORDER by c.timestamp DESC"), 50 );
		while( db_fetch( result, item ) )
		{
			string subject = item["subject"] + item["comment"];
			subject = subject.substr(0, 100);

			form["F"+str(i)]["#type"] = "checkbox";
			form["F"+str(i)]["#name"] = "sel-"+item["cid"];
			form["F"+str(i)]["#value"] = item["cid"];
			form["F"+str(i)]["#table"] = "comments";

			form["A"+str(i)]["#type"] = "markup";
			form["A"+str(i)]["#value"] = "<a href=\""+url("comment/edit/"+item["cid"])+"\">"+subject+"</a>";
			form["A"+str(i)]["#table"] = "comments";

			form["B"+str(i)]["#type"] = "markup";
			form["B"+str(i)]["#value"] = item["name"];
			form["B"+str(i)]["#table"] = "comments";

			form["C"+str(i)]["#type"] = "markup";
			form["C"+str(i)]["#value"] = item["title"];
			form["C"+str(i)]["#table"] = "comments";

			form["D"+str(i)]["#type"] = "markup";
			form["D"+str(i)]["#value"] = date("%m-%d-%Y", item["timestamp"]);
			form["D"+str(i)]["#table"] = "comments";

			form["E"+str(i)]["#type"] = "markup";
			form["E"+str(i)]["#value"] = "<a href=\""+url("comment/edit/"+item["cid"])+"\">edit</a>";
			form["E"+str(i)]["#table"] = "comments";
			
			form["comments"]["#row"+str(i)] = "F"+str(i)+",A"+str(i)+",B"+str(i)+",C"+str(i)+",D"+str(i)+",E"+str(i);
			i++;
		}
	}

	if( i == 0 )
	{
		form["F0"]["#merge"] = _TRUE;
		form["A0"]["#merge"] = _TRUE;
		form["B0"]["#merge"] = _TRUE;
		form["C0"]["#merge"] = _TRUE;
		form["D0"]["#merge"] = _TRUE;

		form["E0"]["#type"] = "markup";
		form["E0"]["#value"] = "No comments available.";
		form["E0"]["#table"] = "comments";
		form["E0"]["#attributes"] = "colspan=\"6\" style=\"padding:2px 14px\"";

		form["comments"]["#row0"] = "F0,A0,B0,C0,D0,E0";
	}

	out += get_form("comment_approval", form);

	out += theme_pager();

	return out;
}

string comment_admin()
{
	string out;
	map <string, string> item;
	map <string, map<string,string> > form;
	
	if( isset(_POST["op"]) )
	{
		if( _POST["status"] == "0" )
		{
			for( map <string, string>::iterator curr = _POST.begin(), end = _POST.end();  curr != end;  curr++ )
			{
				if( curr->first.find("sel-") != curr->first.npos )
				{
					if(DB_TYPE==1) {
						int cid = intval(curr->second);
						redis_command("SREM comments:status:0 %d", cid );
						redis_command("SADD comments:status:1 %d", cid );
						redis_command("HSET comments:%d status 1", cid );
					}
					if(DB_TYPE==2)
						db_querya("UPDATE comments SET status=1 WHERE cid=%d", curr->second.c_str());
				}
			}
		}
		if( _POST["status"] == "1" )
		{
			for( map <string, string>::iterator curr = _POST.begin(), end = _POST.end();  curr != end;  curr++ )
			{
				if( curr->first.find("sel-") != curr->first.npos )
				{
					if(DB_TYPE==1) {
						int cid = intval(curr->second);
						int nid = redis_int("HGET comments:%d nid", cid );
						redis_command("SREM comments %d", cid );
						redis_command("SREM comments:nid:%d %d", nid, cid );
						redis_command("SREM comments:status:0 %d", cid );
						redis_command("SREM comments:status:1 %d", cid );
						redis_command("DEL comments:%d", cid );
					}
					if(DB_TYPE==2)
						db_querya("DELETE FROM comments WHERE cid=%d", curr->second.c_str());
				}
			}
		}
	}
	
	vector <pair <string, string> > options;
	options.push_back( make_pair("0","Unpublish selected comments") );
	options.push_back( make_pair("1","Delete selected comments") );

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

	form["comments"]["#type"] = "table";
	form["comments"]["#header"] = "select-all,Subject,Author,Posted In,Date,Operations";
	form["comments"]["#weight"] = "3";

	int i = 0;	
	
	if(DB_TYPE==1)
	{
		REDIS_RES *result;
		result = redis_pager_fields("SORT comments:status:0 BY comments:*->timestamp DESC", "GET comments:*->", "#cid,nid,subject,name,comment,timestamp", 50);
		while( redis_fetch_fields( result, item ) )
		{
			item["title"] = redis_str("HGET node:%d title", intval(item["nid"]) );
			string subject = item["subject"] + item["comment"];
			subject = subject.substr(0, 100);

			form["F"+str(i)]["#type"] = "checkbox";
			form["F"+str(i)]["#name"] = "sel-"+item["cid"];
			form["F"+str(i)]["#value"] = item["cid"];
			form["F"+str(i)]["#table"] = "comments";

			form["A"+str(i)]["#type"] = "markup";
			form["A"+str(i)]["#value"] = "<a href=\""+url("comment/edit/"+item["cid"])+"\">"+subject+"</a>";
			form["A"+str(i)]["#table"] = "comments";

			form["B"+str(i)]["#type"] = "markup";
			form["B"+str(i)]["#value"] = item["name"];
			form["B"+str(i)]["#table"] = "comments";

			form["C"+str(i)]["#type"] = "markup";
			form["C"+str(i)]["#value"] = item["title"];
			form["C"+str(i)]["#table"] = "comments";

			form["D"+str(i)]["#type"] = "markup";
			form["D"+str(i)]["#value"] = date("%m-%d-%Y", item["timestamp"]);
			form["D"+str(i)]["#table"] = "comments";

			form["E"+str(i)]["#type"] = "markup";
			form["E"+str(i)]["#value"] = "<a href=\""+url("comment/edit/"+item["cid"])+"\">edit</a>";
			form["E"+str(i)]["#table"] = "comments";
			
			form["comments"]["#row"+str(i)] = "F"+str(i)+",A"+str(i)+",B"+str(i)+",C"+str(i)+",D"+str(i)+",E"+str(i);
			i++;
		}
	}

	if(DB_TYPE==2)
	{
		MYSQL_RES *result;
		result = pager_query( sql_parse("SELECT c.cid, c.subject, c.name, c.comment, c.timestamp, n.title FROM comments c LEFT JOIN node n ON n.nid=c.nid WHERE c.status=0 ORDER by c.timestamp DESC"), 50 );
		while( db_fetch( result, item ) )
		{
			string subject = item["subject"] + item["comment"];
			subject = subject.substr(0, 100);

			form["F"+str(i)]["#type"] = "checkbox";
			form["F"+str(i)]["#name"] = "sel-"+item["cid"];
			form["F"+str(i)]["#value"] = item["cid"];
			form["F"+str(i)]["#table"] = "comments";

			form["A"+str(i)]["#type"] = "markup";
			form["A"+str(i)]["#value"] = "<a href=\""+url("comment/edit/"+item["cid"])+"\">"+subject+"</a>";
			form["A"+str(i)]["#table"] = "comments";

			form["B"+str(i)]["#type"] = "markup";
			form["B"+str(i)]["#value"] = item["name"];
			form["B"+str(i)]["#table"] = "comments";

			form["C"+str(i)]["#type"] = "markup";
			form["C"+str(i)]["#value"] = item["title"];
			form["C"+str(i)]["#table"] = "comments";

			form["D"+str(i)]["#type"] = "markup";
			form["D"+str(i)]["#value"] = date("%m-%d-%Y", item["timestamp"]);
			form["D"+str(i)]["#table"] = "comments";

			form["E"+str(i)]["#type"] = "markup";
			form["E"+str(i)]["#value"] = "<a href=\""+url("comment/edit/"+item["cid"])+"\">edit</a>";
			form["E"+str(i)]["#table"] = "comments";
			
			form["comments"]["#row"+str(i)] = "F"+str(i)+",A"+str(i)+",B"+str(i)+",C"+str(i)+",D"+str(i)+",E"+str(i);
			i++;
		}
	}

	if( i == 0 )
	{
		form["F0"]["#merge"] = _TRUE;
		form["A0"]["#merge"] = _TRUE;
		form["B0"]["#merge"] = _TRUE;
		form["C0"]["#merge"] = _TRUE;
		form["D0"]["#merge"] = _TRUE;

		form["E0"]["#type"] = "markup";
		form["E0"]["#value"] = "No comments available.";
		form["E0"]["#table"] = "comments";
		form["E0"]["#attributes"] = "colspan=\"6\" style=\"padding:2px 14px\"";

		form["comments"]["#row0"] = "F0,A0,B0,C0,D0,E0";
	}

	out += get_form("comment_admin", form);

	out += theme_pager();

	return out;
}

string comment_edit()
{
	string out;
	map <string, map<string,string> > form;
	map <string, string> item;

	string cid = arg(2);

	if(DB_TYPE==1) {
		redis_fetch_fields( redis_query_fields(redis_arg("HMGET comments:%d", intval(cid)), "", "nid,subject,hostname,name,comment,timestamp" ), item );
		item["title"] = redis_str("HGET node:%d title", intval(item["nid"]) );
	}
	if(DB_TYPE==2) {
		MYSQL_RES *result;
		result = db_querya( "SELECT c.cid, c.subject, c.hostname, c.name, c.comment, c.timestamp, n.title FROM comments c LEFT JOIN node n ON n.nid=c.nid WHERE c.cid=%d", cid.c_str() );
		db_fetch( result, item );
	}

	form["ip"]["#type"] = "textfield";
	form["ip"]["#title"] = "IP Address";
	form["ip"]["#size"] = "60";
	form["ip"]["#weight"] = "1";
	form["ip"]["#value"] = item["hostname"];
	
	form["name"]["#type"] = "textfield";
	form["name"]["#title"] = "Name";
	form["name"]["#size"] = "60";
	form["name"]["#weight"] = "2";
	form["name"]["#value"] = item["name"];
	
	form["subject"]["#type"] = "textfield";
	form["subject"]["#title"] = "Subject";
	form["subject"]["#size"] = "60";
	form["subject"]["#weight"] = "3";
	form["subject"]["#value"] = item["subject"];
	
	form["comment"]["#type"] = "textarea";
	form["comment"]["#title"] = "Comment";	
	form["comment"]["#weight"] = "4";
	form["comment"]["#value"] = item["comment"];

	form["submit"]["#type"] = "submit";
	form["submit"]["#value"] = "Save";
	form["submit"]["#weight"] = "13";

	return get_form("comment_edit", form);
}

bool comment_edit_validate()
{
	return "";
}

string comment_edit_submit()
{
	string cid = arg(2);

	if(DB_TYPE==1) {
		redis_command_fields(redis_arg("HMSET comments:%d", intval(cid) ), "", 
			"hostname,name,subject,comment", "%s,%s,%s,%s",
			_POST["ip"].c_str(),
			_POST["name"].c_str(),
			_POST["subject"].c_str(),
			_POST["comment"].c_str() );
	}

	if(DB_TYPE==2) {
		db_querya( "UPDATE comments SET hostname='%s', name='%s', subject='%s', comment='%d' WHERE cid=%d", 
			_POST["ip"].c_str(),
			_POST["name"].c_str(),
			_POST["subject"].c_str(),
			_POST["comment"].c_str(),
			cid.c_str() );
	}

	redirect( url("admin/comment") );

	return "";
}
