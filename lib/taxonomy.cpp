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
#include "taxonomy.h"
#include "node.h"

void taxonomy_hooks()
{
	_MAP_DESC( taxonomy, "Enables the categorization of content." );
	_MAP_VER ( taxonomy, "1.3.1" );
	_MAP_PERM( taxonomy, "administer taxonomy" );
	_MAP_HOOK( taxonomy, form_alter );
	_MAP_HOOK( taxonomy, insert );
	_MAP_HOOK( taxonomy, delete );
	_MAP_HOOK( taxonomy, update );
	_MAP_HOOK( taxonomy, load );
	_MAP_HOOK( taxonomy, schema );
	_MAP_HOOK( taxonomy, controllers );
	_MAP_HOOK( taxonomy, help );
}

void taxonomy_schema()
{
	map <string, map<string, string> > term_data;
	term_data["#spec"]["name"] = "term_data";
	term_data["#spec"]["description"] = "Stores term information.";
	term_data["tid"]["type"] = "serial";
	term_data["tid"]["unsigned"] = _TRUE;
	term_data["tid"]["not null"] = _TRUE;
	term_data["tid"]["description"] = "Primary Key: Unique term ID.";
	term_data["vid"]["type"] = "int";
	term_data["vid"]["unsigned"] = _TRUE;
	term_data["vid"]["not null"] = _TRUE;
	term_data["vid"]["default"] = "0";
	term_data["vid"]["description"] = "The {vocabulary}.vid of the vocabulary to which the term is assigned.";
	term_data["name"]["type"] = "varchar";
	term_data["name"]["length"] = "255";
	term_data["name"]["not null"] = _TRUE;
	term_data["name"]["default"] = "";
	term_data["name"]["description"] = "The term name.";
	term_data["description"]["type"] = "text";
	term_data["description"]["not null"] = _FALSE;
	term_data["description"]["size"] = "big";
	term_data["description"]["description"] = "A description of the term.";
	term_data["weight"]["type"] = "int";
	term_data["weight"]["not null"] = _TRUE;
	term_data["weight"]["default"] = "0";
	term_data["weight"]["size"] = "tiny";
	term_data["weight"]["description"] = "The weight of this term in relation to other terms.";
	term_data["#primary key"]["tid"] = "tid";
	term_data["#indexes"]["taxonomy_tree"] = "vid,weight,name";
	term_data["#indexes"]["vid_name"] = "vid,name";
	schema.push_back( term_data );

	map <string, map<string, string> > term_hierarchy;
	term_hierarchy["#spec"]["name"] = "term_hierarchy";
	term_hierarchy["#spec"]["description"] = "Stores the hierarchical relationship between terms.";
	term_hierarchy["tid"]["type"] = "int";
	term_hierarchy["tid"]["unsigned"] = _TRUE;
	term_hierarchy["tid"]["not null"] = _TRUE;
	term_hierarchy["tid"]["default"] = "0";
	term_hierarchy["tid"]["description"] = "Primary Key: The {term_data}.tid of the term.";
	term_hierarchy["parent"]["type"] = "int";
	term_hierarchy["parent"]["unsigned"] = _TRUE;
	term_hierarchy["parent"]["not null"] = _TRUE;
	term_hierarchy["parent"]["default"] = "0";
	term_hierarchy["parent"]["description"] = "Primary Key: The {term_data}.tid of the term's parent. 0 indicates no parent.";
	term_hierarchy["#indexes"]["parent"] = "parent";    
	term_hierarchy["#primary key"]["tid"] = "tid";
	term_hierarchy["#primary key"]["parent"] = "parent";
	schema.push_back( term_hierarchy );

	map <string, map<string, string> > term_node;
	term_node["#spec"]["name"] = "term_node";
	term_node["#spec"]["description"] = "Stores the relationship of terms to nodes.";
	term_node["nid"]["type"] = "int";
	term_node["nid"]["unsigned"] = _TRUE;
	term_node["nid"]["not null"] = _TRUE;
	term_node["nid"]["default"] = "0";
	term_node["nid"]["description"] = "Primary Key: The {node}.nid of the node.";
	term_node["vid"]["type"] = "int";
	term_node["vid"]["unsigned"] = _TRUE;
	term_node["vid"]["not null"] = _TRUE;
	term_node["vid"]["default"] = "0";
	term_node["vid"]["description"] = "Primary Key: The {node}.vid of the node.";
	term_node["tid"]["type"] = "int";
	term_node["tid"]["unsigned"] = _TRUE;
	term_node["tid"]["not null"] = _TRUE;
	term_node["tid"]["default"] = "0";
	term_node["tid"]["description"] = "Primary Key: The {term_data}.tid of a term assigned to the node.";
	term_node["#indexes"]["vid"] = "vid";
	term_node["#indexes"]["nid"] = "nid";
	term_node["#primary key"]["tid"] = "tid";
	term_node["#primary key"]["vid"] = "vid";
	schema.push_back( term_node );

	map <string, map<string, string> > term_relation;
	term_relation["#spec"]["name"] = "term_relation";
	term_relation["#spec"]["description"] = "Stores non-hierarchical relationships between terms.";
	term_relation["trid"]["type"] = "serial";
	term_relation["trid"]["not null"] = _TRUE;
	term_relation["trid"]["description"] = "Primary Key: Unique term relation ID.";
	term_relation["tid1"]["type"] = "int";
	term_relation["tid1"]["unsigned"] = _TRUE;
	term_relation["tid1"]["not null"] = _TRUE;
	term_relation["tid1"]["default"] = "0";
	term_relation["tid1"]["description"] = "The {term_data}.tid of the first term in a relationship.";
	term_relation["tid2"]["type"] = "int";
	term_relation["tid2"]["unsigned"] = _TRUE;
	term_relation["tid2"]["not null"] = _TRUE;
	term_relation["tid2"]["default"] = "0";
	term_relation["tid2"]["description"] = "The {term_data}.tid of the second term in a relationship.";
	term_relation["#unique keys"]["tid1_tid2"] = "tid1,tid2";
	term_relation["#indexes"]["tid2"] = "tid2";
	term_relation["#primary key"]["trid"] = "trid";
	schema.push_back( term_relation );

	map <string, map<string, string> > term_synonym;
	term_synonym["#spec"]["name"] = "term_synonym";
	term_synonym["#spec"]["description"] = "Stores term synonyms.";
	term_synonym["tsid"]["type"] = "serial";
	term_synonym["tsid"]["not null"] = _TRUE;
	term_synonym["tsid"]["description"] = "Primary Key: Unique term synonym ID.";;
	term_synonym["tid"]["type"] = "int";
	term_synonym["tid"]["unsigned"] = _TRUE;
	term_synonym["tid"]["not null"] = _TRUE;
	term_synonym["tid"]["default"] = "0";
	term_synonym["tid"]["description"] = "The {term_data}.tid of the term.";
	term_synonym["name"]["type"] = "varchar";
	term_synonym["name"]["length"] = "255";
	term_synonym["name"]["not null"] = _TRUE;
	term_synonym["name"]["default"] = "";
	term_synonym["name"]["description"] = "The name of the synonym.";
	term_synonym["#indexes"]["tid"] = "tid";
	term_synonym["#indexes"]["name_tid"] = "name,tid";
	term_synonym["#primary key"]["tsid"] = "tsid";
	schema.push_back( term_synonym );
  
	map <string, map<string, string> > vocabulary;
	vocabulary["#spec"]["name"] = "vocabulary";
    vocabulary["#spec"]["description"] = "Stores vocabulary information.";
	vocabulary["vid"]["type"] = "serial";
	vocabulary["vid"]["unsigned"] = _TRUE;
	vocabulary["vid"]["not null"] = _TRUE;
	vocabulary["vid"]["description"] = "Primary Key: Unique vocabulary ID.";
	vocabulary["name"]["type"] = "varchar";
	vocabulary["name"]["length"] = "255";
	vocabulary["name"]["not null"] = _TRUE;
	vocabulary["name"]["default"] = "";
	vocabulary["name"]["description"] = "Name of the vocabulary.";
	vocabulary["description"]["type"] = "text";
	vocabulary["description"]["not null"] = _FALSE;
	vocabulary["description"]["size"] = "big";
	vocabulary["description"]["description"] = "Description of the vocabulary.";
	vocabulary["help"]["type"] = "varchar";
	vocabulary["help"]["length"] = "255";
	vocabulary["help"]["not null"] = _TRUE;
	vocabulary["help"]["default"] = "";
	vocabulary["help"]["description"] = "Help text to display for the vocabulary.";
	vocabulary["relations"]["type"] = "int";
	vocabulary["relations"]["unsigned"] = _TRUE;
	vocabulary["relations"]["not null"] = _TRUE;
	vocabulary["relations"]["default"] = "0";
	vocabulary["relations"]["size"] = "tiny";
	vocabulary["relations"]["description"] = "Whether or not related terms are enabled within the vocabulary. (0 = disabled, 1 = enabled)";
	vocabulary["hierarchy"]["type"] = "int";
	vocabulary["hierarchy"]["unsigned"] = _TRUE;
	vocabulary["hierarchy"]["not null"] = _TRUE;
	vocabulary["hierarchy"]["default"] = "0";
	vocabulary["hierarchy"]["size"] = "tiny";
	vocabulary["hierarchy"]["description"] = "The type of hierarchy allowed within the vocabulary. (0 = disabled, 1 = single, 2 = multiple)";
	vocabulary["multiple"]["type"] = "int";
	vocabulary["multiple"]["unsigned"] = _TRUE;
	vocabulary["multiple"]["not null"] = _TRUE;
	vocabulary["multiple"]["default"] = "0";
	vocabulary["multiple"]["size"] = "tiny";
	vocabulary["multiple"]["description"] = "Whether or not multiple terms from this vocabulary may be assigned to a node. (0 = disabled, 1 = enabled)";
	vocabulary["required"]["type"] = "int";
	vocabulary["required"]["unsigned"] = _TRUE;
	vocabulary["required"]["not null"] = _TRUE;
	vocabulary["required"]["default"] = "0";
	vocabulary["required"]["size"] = "tiny";
	vocabulary["required"]["description"] = "Whether or not terms are required for nodes using this vocabulary. (0 = disabled, 1 = enabled)";
	vocabulary["tags"]["type"] = "int";
	vocabulary["tags"]["unsigned"] = _TRUE;
	vocabulary["tags"]["not null"] = _TRUE;
	vocabulary["tags"]["default"] = "0";
	vocabulary["tags"]["size"] = "tiny";
	vocabulary["tags"]["description"] = "Whether or not free tagging is enabled for the vocabulary. (0 = disabled, 1 = enabled)";
	vocabulary["model"]["type"] = "varchar";
	vocabulary["model"]["length"] = "255";
	vocabulary["model"]["not null"] = _TRUE;
	vocabulary["model"]["default"] = "";
	vocabulary["model"]["description"] = "The model which created the vocabulary.";
	vocabulary["weight"]["type"] = "int";
	vocabulary["weight"]["not null"] = _TRUE;
	vocabulary["weight"]["default"] = "0";
	vocabulary["weight"]["size"] = "tiny";
	vocabulary["weight"]["description"] = "The weight of the vocabulary in relation to other vocabularies.";
	vocabulary["#primary key"]["vid"] = "vid";
	vocabulary["#indexes"]["list"] = "weight,name";
	schema.push_back( vocabulary );

	map <string, map<string, string> > vocabulary_node_types;
	vocabulary_node_types["#spec"]["name"] = "vocabulary_node_types";
	vocabulary_node_types["#spec"]["description"] = "Stores which node types vocabularies may be used with.";
	vocabulary_node_types["vid"]["type"] = "int";
	vocabulary_node_types["vid"]["unsigned"] = _TRUE;
	vocabulary_node_types["vid"]["not null"] = _TRUE;
	vocabulary_node_types["vid"]["default"] = "0";
	vocabulary_node_types["vid"]["description"] = "Primary Key: the {vocabulary}.vid of the vocabulary.";
	vocabulary_node_types["type"]["type"] = "varchar";
	vocabulary_node_types["type"]["length"] = "32";
	vocabulary_node_types["type"]["not null"] = _TRUE;
	vocabulary_node_types["type"]["default"] = "";
	vocabulary_node_types["type"]["description"] = "The {node}.type of the node type for which the vocabulary may be used.";
	vocabulary_node_types["#primary key"]["type"] = "type";
	vocabulary_node_types["#primary key"]["vid"] = "vid";
	vocabulary_node_types["#indexes"]["vid"] = "vid";
	schema.push_back( vocabulary_node_types );
}

void taxonomy_help()
{
	string	path = _GET["q"];

	if( path == "admin/taxonomy" )
	{
		help = "The taxonomy model allows you to classify content into categories and subcategories; it allows multiple lists of categories for classification (controlled vocabularies) and offers the possibility of creating thesauri (controlled vocabularies that indicate the relationship of terms), taxonomies (controlled vocabularies where relationships are indicated hierarchically), and free vocabularies where terms, or tags, are defined during content creation. To view and manage the terms of each vocabulary, click on the associated list terms link. To delete a vocabulary and all its terms, choose \"edit vocabulary\".";
	}

	if( path == "admin/taxonomy/add/vocabulary" )
	{
		help = "When you create a controlled vocabulary you are creating a set of terms to use for describing content (known as descriptors in indexing lingo). SSE allows you to describe each piece of content (blog, story, etc.) using one or many of these terms. For simple implementations, you might create a set of categories without subcategories, similar to Slashdot.org's or Kuro5hin.org's sections. For more complex implementations, you might create a hierarchical list of categories.";
	}
}

void taxonomy_controllers()
{
	map <string, string> item;

	item["path"] = "admin/taxonomy";
	item["title"] = "Categories";
	item["callback"] = _MAP( taxonomy_admin );
	item["access arguments"] = "administer taxonomy";
	item["parent tab"] = "admin/taxonomy";
	poke( controllers, item );

	item["path"] = "admin/taxonomy/add/vocabulary";
	item["title"] = "Add vocabulary";
	item["weight"] = "2";
	item["callback"] = _MAP_FORM( taxonomy_add_vocabulary );
	item["type"] = str( MENU_LOCAL_TASK );
	item["access arguments"] = "administer taxonomy";
	item["parent tab"] = "admin/taxonomy";
	item["parent"] = "admin";
	poke( controllers, item );

	item["path"] = "admin/taxonomy";
	item["title"] = "List";
	item["weight"] = "1";
	item["callback"] = _MAP( taxonomy_admin );
	item["type"] = str( MENU_LOCAL_TASK );
	item["access arguments"] = "administer taxonomy";
	item["parent tab"] = "admin/taxonomy";
	poke( controllers, item );

	item["path"] = "admin/taxonomy/edit/vocabulary/%";
	item["title"] = "Edit vocabulary";
	item["weight"] = "2";
	item["callback"] = _MAP_FORM( taxonomy_edit_vocabulary );
	item["type"] = str( MENU_LOCAL_TASK );
	item["access arguments"] = "administer taxonomy";
	item["parent"] = "admin/taxonomy";
	poke( controllers, item );

	item["path"] = "admin/taxonomy/%";
	item["title"] = "List";
	item["weight"] = "3";
	item["callback"] = _MAP( taxonomy_overview_terms );
	item["type"] = str( MENU_LOCAL_TASK );				
	item["access arguments"] = "administer taxonomy";
	item["parent tab"] = "admin/taxonomy/%"; // Parent tab and path must be equal so we can have tabs.
	item["parent"] = "admin/taxonomy";
	poke( controllers, item );

	item["path"] = "admin/taxonomy/%/add/term";
	item["title"] = "Add term";
	item["weight"] = "4";
	item["callback"] = _MAP_FORM( taxonomy_add_term );
	item["type"] = str( MENU_LOCAL_TASK );
	item["access arguments"] = "administer taxonomy";
	item["parent tab"] = "admin/taxonomy/%"; // Parent tab and path must be equal so we can have tabs.
	item["parent"] = "admin/taxonomy";
	poke( controllers, item );

	item["path"] = "admin/taxonomy/edit/term/%";
	item["title"] = "Edit term";
	item["weight"] = "5";
	item["callback"] = _MAP_FORM( taxonomy_edit_term );
	item["type"] = str( MENU_LOCAL_TASK );
	item["access arguments"] = "administer taxonomy";
	item["parent"] = "admin/taxonomy";
	item["parent tab"] = "admin/taxonomy/edit/term/%";
	poke( controllers, item );

	item["path"] = "taxonomy/term/%";
	item["title"] = "Term Page";
	item["weight"] = "6";
	item["callback"] = _MAP( taxonomy_term_page );
	item["type"] = str( MENU_CALLBACK );
	item["access arguments"] = "administer taxonomy";
	poke( controllers, item );

	item["path"] = "taxonomy/autocomplete/%";
	item["title"] = "Autocomplete";
	item["callback"] = _MAP( taxonomy_autocomplete );
	item["type"] = str( MENU_CALLBACK );
	item["access arguments"] = "access content";
	poke( controllers, item );
	
	item["path"] = "rss";
	item["title"] = "RSS";
	item["weight"] = "6";
	item["callback"] = _MAP( taxonomy_rss );
	item["type"] = str( ~MENU_VISIBLE_IN_TREE );
	item["access arguments"] = "administer taxonomy";
	poke( controllers, item );

	item["path"] = "rss/list";
	item["title"] = "RSS";
	item["weight"] = "6";
	item["callback"] = _MAP( taxonomy_rss_list );
	item["type"] = str( ~MENU_VISIBLE_IN_TREE );
	item["access arguments"] = "administer taxonomy";
	poke( controllers, item );

	item["path"] = "rss/featured";
	item["title"] = "RSS";
	item["weight"] = "6";
	item["callback"] = _MAP( taxonomy_rss_story );
	item["type"] = str( ~MENU_VISIBLE_IN_TREE );
	item["access arguments"] = "administer taxonomy";
	poke( controllers, item );
}

string taxonomy_rss_list()
{
	string out;

	map <string, string> extra;
	extra["title"] = "";
	set_page_extra("", extra);
	out = "\
		  <div style=\"margin-top:10px; margin-bottom:10px; height:28px; line-height:28px; font-weight:bold; color:#0A3C6A\" class=\"title\">TEXAS WATCHDOG\
		  <img style=\"margin-bottom:4px\" align=\"absmiddle\" src=\"themes/twd/ico/rss2.gif\">\n\
		  </div>\n\
		  <div style=\"line-height:20px;padding:0px 2px\">Subscribe to Texas Watchdog RSS (Really Simple Syndication) feeds to get news <br>delivered directly to your desktop.</div>\n\
		  <div style=\"margin:10px 20px 0px 0px\">\n\
		  <table width=\"100%\" border=\"0\" cellpadding=\"6\" cellspacing=\"0\" style=\"background:#FFFFFF; font-weight:bold; font-size:13px\">\n\
		  <tr>\n\
			<td onclick=\"location.href='"+url("rss/featured")+"'\" style=\"cursor:pointer; border:6px solid #FFF;background:#EDEFEE\">Texas Watchdog <span style=\"color:#4389BC\">Featured Reports</span></td>\n\
			<td onclick=\"location.href='"+url("rss/featured")+"'\" style=\"cursor:pointer; border:6px solid #FFF;background:#EDEFEE; text-align:center\">RSS Feed<img style=\"margin-bottom:4px; margin-left:10px\" align=\"absmiddle\" src=\"themes/twd/ico/rss3.gif\"></td>\n\
		  </tr>\n\
		  <tr>\n\
			<td onclick=\"location.href='"+url("rss/videos")+"'\" style=\"cursor:pointer; border:6px solid #FFF;background:#EDEFEE\">Texas Watchdog <span style=\"color:#4389BC\">Videos</span></td>\n\
			<td onclick=\"location.href='"+url("rss/videos")+"'\" style=\"cursor:pointer; border:6px solid #FFF;background:#EDEFEE; text-align:center\">RSS Feed<img style=\"margin-bottom:4px; margin-left:10px\" align=\"absmiddle\" src=\"themes/twd/ico/rss3.gif\"></td>\n\
		  </tr>\n\
		  <tr>\n\
			<td onclick=\"location.href='"+url("rss/blog")+"'\" style=\"cursor:pointer; border:6px solid #FFF;background:#EDEFEE\">Texas Watchdog <span style=\"color:#4389BC\">Blog</span></td>\n\
			<td onclick=\"location.href='"+url("rss/blog")+"'\" style=\"cursor:pointer; border:6px solid #FFF;background:#EDEFEE; text-align:center\">RSS Feed<img style=\"margin-bottom:4px; margin-left:10px\" align=\"absmiddle\" src=\"themes/twd/ico/rss3.gif\"></td>\n\
		  </tr>\n\
		  <tr>\n\
			<td onclick=\"location.href='"+url("rss/links")+"'\" style=\"cursor:pointer; border:6px solid #FFF;background:#EDEFEE\">Texas Watchdog <span style=\"color:#4389BC\">Links</span></td>\n\
			<td onclick=\"location.href='"+url("rss/links")+"'\" style=\"cursor:pointer; border:6px solid #FFF;background:#EDEFEE; text-align:center\">RSS Feed<img style=\"margin-bottom:4px; margin-left:10px\" align=\"absmiddle\" src=\"themes/twd/ico/rss3.gif\"></td>\n\
		  </tr>\n\
		  <tr>\n\
			<td onclick=\"location.href='"+url("rss")+"'\" style=\"cursor:pointer; border:6px solid #FFF;background:#EDEFEE\">Texas Watchdog <span style=\"color:#4389BC\">All Feeds</span></td>\n\
			<td onclick=\"location.href='"+url("rss")+"'\" style=\"cursor:pointer; border:6px solid #FFF;background:#EDEFEE; text-align:center\">RSS Feed<img style=\"margin-bottom:4px; margin-left:10px\" align=\"absmiddle\" src=\"themes/twd/ico/rss3.gif\"></td>\n\
		  </tr>\n\
		  </table>\n\
		  </div>\n\
		  ";

	return out;
}

string taxonomy_rss()
{
	MYSQL_RES *result;
	map <string, string> item, node;

string rss = "\
<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n\
<rss version=\"2.0\" xmlns:media=\"http://search.yahoo.com/mrss/\" xmlns:ynews=\"http://news.yahoo.com/rss/\">\n\
<channel>\n\
<title>"+variable_get("site_name", "")+"</title>\n\
<copyright>Copyright (c) "+date("%Y",str(time()))+" Texas Watchdog and Use Labs. All rights reserved.</copyright>\n\
<link>"+url("")+"</link>\n\
<category>local news</category>\n\
<description>"+variable_get("site_slogan", "")+"</description>\n\
<language>en-us</language> \n\
<lastBuildDate>"+date("%#c",str(time()))+" CST</lastBuildDate>\n\
<ttl>5</ttl>\n\
<image>\n\
<title>"+variable_get("site_name", "")+"</title>\n\
<width>435</width>\n\
<height>98</height>\n\
<link>"+url("")+"</link>\n\
<url>"+url("themes/twd/img/logo.gif")+"</url>\n\
</image>\n";

result = db_querya("SELECT n.nid FROM node n WHERE n.status>0 AND (n.type='story' OR n.type='blog') ORDER BY n.created DESC LIMIT 0, 20");
while( db_fetch( result, item ) )
{
	if( node_load( node, item["nid"] ) )
	{
		string desc;

		desc = htmlencode(node["body"]);

	if( node["type"] == "story" )
		node["url"] = url("story/"+item["nid"]);
	else if( node["type"] == "link" )
		node["url"] = node["headline"];
	else
		node["url"] = url("node/"+item["nid"]);

rss += "\
<item>\n\
<title>"+htmlencode(node["title"])+"</title>\n\
\n\
<link>"+htmlencode(node["url"])+"</link>\n\
<guid isPermaLink=\"false\">"+node["nid"]+"</guid>\n\
<category>world</category>\n\
<pubDate>"+date("%#c",node["created"])+" CST</pubDate>\n\
<description>"+desc+"</description>\n\
\n";
for(size_t i=0; i< cur_files.size(); i++)
{
	trim( cur_files[i]["filepath"] );
	rss += "<media:content url=\""+ htmlencode( url( cur_files[i]["filepath"] ) ) +"\" type=\"image/jpeg\" height=\"91\" width=\"130\"/>\n";
}
rss += "\
<media:credit role=\"author\">"+htmlencode( node["signature"] )+"</media:credit>\n\
</item>\n";
	}
}

rss += "\
</channel>\n\
</rss>";

print (rss);

return "";
}

string taxonomy_rss_story()
{
	MYSQL_RES *result;
	map <string, string> item, node;

string rss = "\
<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n\
<rss version=\"2.0\" xmlns:media=\"http://search.yahoo.com/mrss/\" xmlns:ynews=\"http://news.yahoo.com/rss/\">\n\
<channel>\n\
<title>"+variable_get("site_name", "")+"</title>\n\
<copyright>Copyright (c) "+date("%Y",str(time()))+" Texas Watchdog and Use Labs. All rights reserved.</copyright>\n\
<link>"+url("")+"</link>\n\
<category>local news</category>\n\
<description>"+variable_get("site_slogan", "")+"</description>\n\
<language>en-us</language> \n\
<lastBuildDate>"+date("%#c",str(time()))+" CST</lastBuildDate>\n\
<ttl>5</ttl>\n\
<image>\n\
<title>"+variable_get("site_name", "")+"</title>\n\
<width>435</width>\n\
<height>98</height>\n\
<link>"+url("")+"</link>\n\
<url>"+url("themes/twd/img/logo.gif")+"</url>\n\
</image>\n";

result = db_querya("SELECT n.nid FROM node n WHERE n.status>0 AND n.type='story' ORDER BY n.created DESC LIMIT 0, 10");
while( db_fetch( result, item ) )
{
	if( node_load( node, item["nid"] ) )
	{
		string desc, title;

		desc = htmlencode(node["body"]);
		title = htmlencode(node["title"]);

	if( node["type"] == "story" )
		node["url"] = url("story/"+item["nid"]);
	else
		node["url"] = url("node/"+item["nid"]);

rss += "\
<item>\n\
<title>"+title+"</title>\n\
<link>"+node["url"]+"</link>\n\
<guid isPermaLink=\"false\">"+node["nid"]+"</guid>\n\
<category>world</category>\n\
<pubDate>"+date("%#c",node["created"])+" CST</pubDate>\n\
<description>"+desc+"</description>\n\
\n";
for(size_t i=0; i< cur_files.size(); i++)
{
	trim( cur_files[i]["filepath"] );
	rss += "<media:content url=\""+ htmlencode( url( cur_files[i]["filepath"] ) ) +"\" type=\"image/jpeg\" height=\"91\" width=\"130\"/>\n";
}
rss += "\
</item>\n";
	}
}

rss += "\
</channel>\n\
</rss>";

print (rss);

return "";
}

void taxonomy_get_vocabulary(map <string, string> &ret, string vid)
{
	vector <string> node_types;

	if(DB_TYPE==1)
	{
		if( redis_multi(ret, "HGETALL vocabulary:%d", intval(vid)) )
		{
			ret["vid"] = vid;
			REDIS_RES *rr2 = redis_query("SMEMBERS vocabulary:%d:node_types", intval(vid) );
			while( redis_fetch( rr2, ret["type"] ) )
			{
				string vid = ret["vid"];
				node_types.push_back( ret["type"] );
				ret.erase( "type" ); //unset "type"
				nest( node_types, &ret, "nodes" );
			}
		}
	}
    
	if(DB_TYPE==2)
	{                   
		MYSQL_RES *result;
		result = db_querya("SELECT v.*, n.type FROM vocabulary v LEFT JOIN vocabulary_node_types n ON v.vid = n.vid WHERE v.vid = %d ORDER BY v.weight, v.name", vid.c_str());

		while ( db_fetch(result, ret) )
		{
			string vid = ret["vid"];
			node_types.push_back( ret["type"] );
			ret.erase( "type" ); //unset "type"
			nest( node_types, &ret, "nodes" );
		}
	}

	return;
}

void taxonomy_get_vocabularies(map <string, map<string,string> > &ret, string type = "")
{
	MYSQL_RES *result;
	map <string, string> voc;
	map <string, vector <string> > node_types;

	if(DB_TYPE == 1)
	{
		REDIS_RES *rr;
		rr = redis_query_fields("SORT vocabulary BY vocabulary:*->name ALPHA", "GET vocabulary:*->", "#vid,name,description,help,relations,hierarchy,multiple,required,tags,weight");
		while( redis_fetch_fields( rr, voc ) )
		{
			string vid = voc["vid"];
			REDIS_RES *rr2 = redis_query("SMEMBERS vocabulary:%d:node_types", intval(vid) );
			while( redis_fetch( rr2, voc["type"] ) )
			{
				node_types[ vid ].push_back( voc["type"] );
				voc.erase( "type" ); //unset "type"
				ret[ vid ] = voc;
				nest( node_types[ vid ], &ret[ vid ], "nodes" );
			}			
		}
	}

	if(DB_TYPE == 2)
	{
		if (type.length()>0) {
			result = db_querya("SELECT v.vid, v.*, n.type FROM vocabulary v LEFT JOIN vocabulary_node_types n ON v.vid = n.vid WHERE n.type = '%s' ORDER BY v.weight, v.name", type.c_str() );
		}
		else {
			result = db_querya("SELECT v.*, n.type FROM vocabulary v LEFT JOIN vocabulary_node_types n ON v.vid = n.vid ORDER BY v.weight, v.name");
		}

		while ( db_fetch(result, voc) )
		{
			string vid = voc["vid"];
			node_types[ vid ].push_back( voc["type"] );
			voc.erase( "type" ); //unset "type"
			ret[ vid ] = voc;
			nest( node_types[ vid ], &ret[ vid ], "nodes" );
		}
	}

	return;
}

void taxonomy_get_term( string tid, map <string, string> &term )
{
	// simple cache using a static var?
	if(DB_TYPE == 1)
	{
		redis_multi(term, "HGETALL term_data:%d", intval(tid));
	}
	if(DB_TYPE == 2)
	{
		db_fetch( db_querya("SELECT * FROM term_data WHERE tid = %d", tid.c_str() ), term );
	}
}

void taxonomy_get_parents(string tid, vector <string> &parents)
{
	map <string, string> item;
	
	if(DB_TYPE == 1)
	{
		REDIS_RES *rr2 = redis_query("SMEMBERS term_hierarchy:%d:parent", intval(tid) );
		while( redis_fetch( rr2, item["parent"] ) )
		{
			parents.push_back( item["parent"] );
		}
	}
	if(DB_TYPE == 2)
	{
		MYSQL_RES *result;
		result = db_querya("SELECT t.tid FROM term_data t INNER JOIN term_hierarchy h ON h.parent = t.tid WHERE h.tid = %d ORDER BY weight, name", tid.c_str() );
		while ( db_fetch(result, item) )
		{
			parents.push_back( item["tid"] );
		}
	}
}

void taxonomy_get_terms(string vid, vector <map <string, string> > &terms)
{
	map <string, string> term;
	terms.clear();
	
	if(DB_TYPE == 1)
	{
		REDIS_RES *rr = redis_query_fields( redis_arg("SORT vocabulary:%d:tids BY term_data:*->weight BY term_data:*->name ALPHA", intval(vid) ), "GET term_data:*->", "#tid,name,description,vid,weight");
		while( redis_fetch_fields( rr, term ) )
		{
			REDIS_RES *rr2 = redis_query("SMEMBERS term_hierarchy:%d:parent", intval(term["tid"]) );
			while( redis_fetch( rr2, term["parent"] ) )
			{
				terms.push_back( term );
			}
		}
	}
	
	if(DB_TYPE == 2)
	{
		MYSQL_RES *result = db_querya("SELECT t.tid, t.*, parent FROM term_data t INNER JOIN term_hierarchy h ON t.tid = h.tid WHERE t.vid = %d ORDER BY weight, name", vid.c_str() );
		while ( db_fetch(result, term) )
		{
			terms.push_back( term );
		}
	}
}

void taxonomy_get_terms(string vid, map <string, map<string,string> > &terms)
{
	map <string, string> term;
	terms.clear();

	if(DB_TYPE == 1)
	{
		REDIS_RES *rr = redis_query_fields( redis_arg("SORT vocabulary:%d:tids BY term_data:*->name ALPHA", intval(vid) ), "GET term_data:*->", "#tid,name,description,vid,weight");
		while( redis_fetch_fields( rr, term ) )
		{
			REDIS_RES *rr2 = redis_query("SMEMBERS term_hierarchy:%d:parent", intval(term["tid"]) );
			while( redis_fetch( rr2, term["parent"] ) )
			{
				terms[ term["tid"] ] = term;
			}
		}
	}
	if(DB_TYPE == 2)
	{
		MYSQL_RES *result;
		result = db_querya("SELECT t.tid, t.*, parent FROM term_data t INNER JOIN term_hierarchy h ON t.tid = h.tid WHERE t.vid = %d ORDER BY weight, name", vid.c_str() );
		while ( db_fetch(result, term) )
		{
			terms[ term["tid"] ] = term;
		}
	}
}

vector <pair <string, string> > taxonomy_make_tree(string parent, vector <map <string, string> > &terms)
{
	static vector <pair <string, string> > options;
	static int depth = 0;

	// With vector of pairs we can keep the order we want

	if( depth == 0 ) options.clear();

	depth ++;
	
	for( size_t i = 0; i< terms.size(); i++ )
	{
		if( terms[i]["parent"] == parent )
		{
			options.push_back( make_pair( terms[i]["tid"], str_repeat("-",depth-1) + t(terms[i]["name"]) ) );
			taxonomy_make_tree( terms[i]["tid"], terms );
		}
	}

	depth --;
	
	return options;
}

map <string, string> taxonomy_form( map <string, string> &vocabulary )
{
	vector <map <string, string> > terms;
	map <string, string>	form;
	vector <pair <string, string> > options;

	taxonomy_get_terms( vocabulary["vid"], terms );

	options = taxonomy_make_tree( "0", terms );

	if( vocabulary["required"] == _FALSE ) {
		options.insert( options.begin(), make_pair("0","<none>") );
	}

	form["#type"] = "select";
    form["#title"] = vocabulary["name"];
    form["#options"] = serialize_array( options );
	form["#required"] = vocabulary["required"];
    form["#description"] = vocabulary["description"];
    form["#multiple"] = vocabulary["multiple"];
    form["#size"] = "9";

	return form;
}

void taxonomy_form_alter()
{
	if( cur_form_id == "add-page" ) {

		map <string, string> node;
		map <string, string> vocabulary;
		vector <map <string, string> > vocabularies;
				
		node = cur_form["#node"];

		if(DB_TYPE==1)
		{
			REDIS_RES *result = redis_query_fields( redis_arg("SORT vocabulary:node_types:%s BY vocabulary:*->weight", node["type"].c_str()), "GET vocabulary:*->", "#vid,name,description,help,relations,hierarchy,multiple,required,tags,weight");
			while( redis_fetch_fields( result, vocabulary ) ) {
				vocabularies.push_back( vocabulary );
			}
		}
		if(DB_TYPE==2)
		{
			MYSQL_RES *result = db_querya("SELECT v.* FROM vocabulary v INNER JOIN vocabulary_node_types n ON v.vid = n.vid WHERE n.type = '%s' ORDER BY v.weight, v.name", node["type"].c_str() );
			while( db_fetch( result, vocabulary ) ) {
				vocabularies.push_back( vocabulary );
			}
		}
		
		for(size_t n=0; n< vocabularies.size(); n++ )
		{
			vocabulary = vocabularies[n];
			if( vocabulary["tags"] == _TRUE )
			{
				cur_form["taxonomy-"+vocabulary["vid"]]["#type"] = "textfield";
				cur_form["taxonomy-"+vocabulary["vid"]]["#title"] = vocabulary["name"];
				cur_form["taxonomy-"+vocabulary["vid"]]["#description"] = "A comma-separated list of terms describing this content. Example: funny, bungee jumping, ...";
				cur_form["taxonomy-"+vocabulary["vid"]]["#required"] = vocabulary["required"];
				cur_form["taxonomy-"+vocabulary["vid"]]["#value"] = node[ "taxonomy-"+vocabulary["vid"] ];
				cur_form["taxonomy-"+vocabulary["vid"]]["#autocomplete_path"] = "taxonomy/autocomplete/"+vocabulary["vid"];
				cur_form["taxonomy-"+vocabulary["vid"]]["#weight"] = vocabulary["weight"];
				cur_form["taxonomy-"+vocabulary["vid"]]["#maxlength"] = "1024";
			}
			else
			{
				cur_form[ "taxonomy-"+vocabulary["vid"] ] = taxonomy_form( vocabulary );
				cur_form[ "taxonomy-"+vocabulary["vid"] ]["#weight"] = vocabulary["weight"];

				size_t cut, len;
				if( (cut = node[ "taxonomy-"+vocabulary["vid"] ].find("#array[")) != string::npos && cut == 0) // is an array
				{
					len = intval( node[ "taxonomy-"+vocabulary["vid"] ].substr(7) );
					for( int i=0; i<(int)len; i++) {
						cur_form[ "taxonomy-"+vocabulary["vid"] ]["#value["+str(i)+"]"] = node[ "taxonomy-"+vocabulary["vid"]+"["+str(i)+"]" ];
					}
				}
				if( isset( node[ "taxonomy-"+vocabulary["vid"] ] ) )
					cur_form[ "taxonomy-"+vocabulary["vid"] ]["#value"] = node[ "taxonomy-"+vocabulary["vid"] ];
			}
		}

	}
}

void taxonomy_node_delete_revision( map <string, string> &node )
{
	if(DB_TYPE==1) {
		string tid;
		REDIS_RES *result = redis_query("SMEMBERS term_node:vid:%d", intval(node["vid"]) );
		while ( redis_fetch(result, tid ) ) {
			redis_command("SREM term_node:tid:%d %d", intval(tid), intval(node["nid"]) );
		}
		redis_command("DEL term_node:vid:%d", intval(node["vid"]) );
	}
	if(DB_TYPE==2) {
		db_querya("DELETE FROM term_node WHERE vid = %d", node["vid"].c_str() );
	}
}

void taxonomy_node_save( map <string, string> &node )
{
	if(DB_TYPE==1)
	{
		map <string, string> vocabulary;
		taxonomy_node_delete_revision( node );
		REDIS_RES *result = redis_query_fields( redis_arg("SORT vocabulary:node_types:%s BY vocabulary:*->weight", node["type"].c_str()), "GET vocabulary:*->", "#vid,name,description,help,relations,hierarchy,multiple,required,tags,weight");
		while( redis_fetch_fields( result, vocabulary ) ) 
		{
			vector <string>		terms;

			if( vocabulary["tags"] == _TRUE )
			{
				explode( terms, node["taxonomy-"+vocabulary["vid"]] );
				trim( terms );
				for(size_t i=0; i<terms.size(); i++)
				{
					int tid = redis_int("GET vocabulary:%d:tid_by_name:%s", intval(vocabulary["vid"]), strtolower(terms[i]).c_str() );
					if( !tid ) {
						map <string, string> new_term;
						new_term["name"] = terms[i];
						new_term["vid"] = vocabulary["vid"];
						new_term["weight"] = "0";
						taxonomy_save_term( new_term );
						tid = intval(new_term["tid"]);
					}
					redis_command("SADD term_node:vid:%d %d", intval(node["vid"]), tid );
					redis_command("SADD term_node:tid:%d %d", tid, intval(node["nid"]) );
				}
			}
			else
			{	
				unnest( &node, "taxonomy-"+vocabulary["vid"], terms );
				for( size_t i=0 ; i< terms.size(); i++) {
					redis_command("SADD term_node:vid:%d %d", intval(node["vid"]), intval(terms[i]) );
					redis_command("SADD term_node:tid:%d %d", intval(terms[i]), intval(node["nid"]) );
				}
			}			
		}
	}

	if(DB_TYPE==2)
	{
		MYSQL_RES *result;
		map <string, string> vocabulary;

		taxonomy_node_delete_revision( node );
		
		result = db_querya("SELECT v.* FROM vocabulary v INNER JOIN vocabulary_node_types n ON v.vid = n.vid WHERE n.type = '%s' ORDER BY v.weight, v.name", node["type"].c_str() );
		while( db_fetch( result, vocabulary ) )
		{	
			vector <string>		terms;

			if( vocabulary["tags"] == _TRUE )
			{
				explode( terms, node["taxonomy-"+vocabulary["vid"]] );
				trim( terms );
				for(size_t i=0; i<terms.size(); i++)
				{
					string tid = db_result(db_querya("SELECT t.tid FROM term_data t WHERE t.vid=%d AND LOWER(t.name) = LOWER('%s')", vocabulary["vid"].c_str(), terms[i].c_str()) );
					if( !isset(tid) )
					{
						map <string, string> new_term;
						new_term["name"] = terms[i];
						new_term["vid"] = vocabulary["vid"];
						new_term["weight"] = "0";
						taxonomy_save_term( new_term );
						tid = new_term["tid"];
					}
					db_querya("INSERT INTO term_node (nid, vid, tid) VALUES (%d, %d, %d)", node["nid"].c_str(), node["vid"].c_str(), tid.c_str() );
				}
			}
			else
			{	
				unnest( &node, "taxonomy-"+vocabulary["vid"], terms );

				for( size_t i=0 ; i< terms.size(); i++)
				{
					db_querya("INSERT INTO term_node (nid, vid, tid) VALUES (%d, %d, %d)", node["nid"].c_str(), node["vid"].c_str(), terms[i].c_str() );
				}
			}
		}
	}
}

void taxonomy_load()
{
	map <string, string> term;
	map <string, string> vids;
	map <string, vector <string> > tags;

	if(DB_TYPE==1)
	{
		REDIS_RES *result;
		result = redis_query_fields( redis_arg("SORT term_node:vid:%d BY term_data:*->weight", intval(cur_node["vid"]) ), "GET term_data:*->", "#tid,vid,name,description,weight");
		while( redis_fetch_fields( result, term ) )
		{
			if( redis_int("HGET vocabulary:%d tags", intval(term["vid"]) ) )
			{
				tags[ term["vid"] ].push_back(term["name"]);
			}
			else
			{
				string i = cur_node[ "taxonomy-"+term["vid"] ];
				if( !isset(i) ) i = "0";
				else i = i.substr(7, i.length()-8);
				
				cur_node[ "taxonomy-"+term["vid"] + "["+i+"]" ] = term["tid"];
				i = "#array[" + str( intval( i ) + 1 ) + "]";
				cur_node[ "taxonomy-"+term["vid"] ] = i;
			}

			vids[ term["vid"] ] = term["vid"];
		}
	}

	if(DB_TYPE==2)
	{
		MYSQL_RES *result;
		result = db_querya("SELECT t.*, v.tags FROM term_node r INNER JOIN term_data t ON r.tid = t.tid INNER JOIN vocabulary v ON t.vid = v.vid WHERE r.vid = %d ORDER BY v.weight, t.weight, t.name", cur_node["vid"].c_str() );
		while( db_fetch( result, term ) )
		{
			if( term["tags"] == _TRUE )
			{
				tags[ term["vid"] ].push_back(term["name"]);
			}
			else
			{
				string i = cur_node[ "taxonomy-"+term["vid"] ];
				if( !isset(i) ) i = "0";
				else i = i.substr(7, i.length()-8);
				
				cur_node[ "taxonomy-"+term["vid"] + "["+i+"]" ] = term["tid"];
				i = "#array[" + str( intval( i ) + 1 ) + "]";
				cur_node[ "taxonomy-"+term["vid"] ] = i;
			}

			vids[ term["vid"] ] = term["vid"];
		}
	}

	// If the vocabulary has Free Tagging we just put all the terms separated by comma
	for( map <string, vector <string> >::iterator curr = tags.begin(), end = tags.end();  curr != end;  curr++ ) {
		cur_node[ "taxonomy-"+curr->first ] = implode(curr->second, ", ");
	}
	
	cur_node[ "taxonomy" ] = "";
	for( map <string, string>::iterator curr = vids.begin(), end = vids.end();  curr != end;  curr++ ) {
		if( curr != vids.begin() ) cur_node[ "taxonomy" ] += ",";
		cur_node[ "taxonomy" ] += curr->second;
	}
}

void taxonomy_delete()
{
	map <string, string> node = cur_node;
	taxonomy_node_delete_revision( node );
}

void taxonomy_insert()
{
	map <string, string> node = cur_node;
	taxonomy_node_save( node );
}

void taxonomy_update()
{
	map <string, string> node = cur_node;
	taxonomy_node_save( node );
}

string taxonomy_link( map <string,string> &node )
{
	string	out;
	map <string, map<string,string> > terms;
	vector <string> node_terms;
	vector <string> vocabularies;
	
	out += "<ul class=\"links inline\">\n";

	if(DB_TYPE==1)
	{
		REDIS_RES *result;
		string vocabulary;
		result = redis_query("SMEMBERS vocabulary:node_types:%s %d", node["type"].c_str() );
		while ( redis_fetch(result, vocabulary ) ) {
			vocabularies.push_back( vocabulary );
		}
	}
	if(DB_TYPE==2)
	{
		MYSQL_RES *result;
		map <string, string> vocabulary;
		result = db_querya("SELECT vid FROM vocabulary_node_types WHERE type='%s'", node["type"].c_str() );
		while ( db_fetch(result, vocabulary ) ) {
			vocabularies.push_back( vocabulary["vid"] );
		}
	}
	
	for(size_t i=0; i<vocabularies.size(); i++)
	{
		terms.clear();
		taxonomy_get_terms( vocabularies[i], terms );
		
		node_terms.clear();
		unnest( &node, "taxonomy-"+vocabularies[i], node_terms );
		for(size_t j=0; j<node_terms.size(); j++)
		{
			out += "<li class=\"taxonomy_term_"+node_terms[j]+"\"><a href=\""+url("taxonomy/term/"+node_terms[j])+"\" rel=\"tag\" title=\"\">";
			out += terms[node_terms[j]]["name"];
			out += "</a></li>\n";
		}
	}

	out += "</ul>\n";

	return out;
}

string node_get_types()
{
	map <string, string> types;
	
	for( map <string, map<string,string> >::iterator nt = node_types.begin(), nt_end = node_types.end();  nt != nt_end;  nt++ )
	{	
		types[ nt->first ] = nt->first;
	}

	return serialize_array( types );
}

void _vocabulary_form( map <string, map<string,string> > &form )
{
	form["name"]["#type"] = "textfield";
    form["name"]["#title"] = "Vocabulary name";
    form["name"]["#maxlength"] = "64";
    form["name"]["#description"] = "The name for this vocabulary.  Example: \"Topic\"";
    form["name"]["#required"] = _TRUE;
	form["name"]["#weight"] = "1";

	form["description"]["#type"] = "textarea";
	form["description"]["#title"] = "Description";
    form["description"]["#description"] = "Description of the vocabulary; can be used by models.";
	form["description"]["#weight"] = "2";
  
	form["help"]["#type"] = "textfield";
    form["help"]["#title"] = "Help text";
    form["help"]["#description"] = "Instructions to present to the user when choosing a term.";
	form["help"]["#weight"] = "3";

	form["nodes"]["#type"] = "checkboxes";
    form["nodes"]["#title"] = "Types";
    form["nodes"]["#options"] = node_get_types();
    form["nodes"]["#description"] = "A list of node types you want to associate with this vocabulary.";
    form["nodes"]["#required"] = _TRUE;
	form["nodes"]["#weight"] = "4";
  
	form["hierarchy"]["#type"] = "radios";
    form["hierarchy"]["#title"] = "Hierarchy";
    form["hierarchy"]["#options"] = "Disabled,Single,Multiple";
    form["hierarchy"]["#description"] = "Allows <a href=\""+url("admin/help/taxonomy")+"\">a tree-like hierarchy</a> between terms of this vocabulary.";
	form["hierarchy"]["#weight"] = "5";
  
	form["relations"]["#type"] = "checkbox";
    form["relations"]["#title"] = "Related terms";
	form["relations"]["#value"] = _TRUE;
    form["relations"]["#description"] = "Allows <a href=\""+url("admin/help/taxonomy")+"\">related terms</a> in this vocabulary.";
	form["relations"]["#weight"] = "6";
  
	form["tags"]["#type"] = "checkbox";
    form["tags"]["#title"] = "Free tagging";
	form["tags"]["#value"] = _TRUE;
    form["tags"]["#description"] = "Content is categorized by typing terms instead of choosing from a list.";
	form["tags"]["#weight"] = "7";
  
	form["multiple"]["#type"] = "checkbox";
    form["multiple"]["#title"] = "Multiple select";
	form["multiple"]["#value"] = _TRUE;
    form["multiple"]["#description"] = "Allows nodes to have more than one term from this vocabulary (always true for free tagging).";
	form["multiple"]["#weight"] = "8";
  
	form["required"]["#type"] = "checkbox";
    form["required"]["#title"] = "Required";
	form["required"]["#value"] = _TRUE;
    form["required"]["#description"] = "If enabled, every node <strong>must</strong> have at least one term in this vocabulary.";
	form["required"]["#weight"] = "9";
  
	form["weight"]["#type"] = "weight";
	form["weight"]["#title"] = "Weight";
    form["weight"]["#description"] = "In listings, the heavier vocabularies will sink and the lighter vocabularies will be positioned nearer the top.";
	form["weight"]["#weight"] = "10";

	form["submit"]["#type"] = "submit";
	form["submit"]["#value"] = "Submit";
	form["submit"]["#weight"] = "13";

	return;
}

string taxonomy_add_vocabulary()
{
	map <string, map<string,string> > form;

	_vocabulary_form( form );
  
	return get_form("taxonomy_add_vocabulary", form);
}

string taxonomy_add_vocabulary_submit()
{
	vector <string> nodes;

	unnest( &cur_form["nodes"], "#value", nodes );

	if(DB_TYPE == 1)
	{
		int vid = redis_int("INCR vocabulary:ids");	
		redis_command("SADD vocabulary %d", vid);
		redis_command("HMSET vocabulary:%d name %s description %s help %s relations %s hierarchy %s multiple %s required %s tags %s weight %s",
			vid,
			cur_form["name"]["#value"].c_str(), 
			cur_form["description"]["#value"].c_str(),
			cur_form["help"]["#value"].c_str(),
			isset( _POST["relations"] ) ? _POST["relations"].c_str():"0",
			isset( _POST["hierarchy"] ) ? _POST["hierarchy"].c_str():"0",
			isset( _POST["multiple"] ) ? _POST["multiple"].c_str():"0",
			isset( _POST["required"] ) ? _POST["required"].c_str():"0",
			isset( _POST["tags"] ) ? _POST["tags"].c_str():"0",
			cur_form["weight"]["#value"].c_str() );

		for(size_t i=0; i<nodes.size(); i++) {
			redis_command("SADD vocabulary:%d:node_types %s", vid, nodes[i].c_str() );
			redis_command("SADD vocabulary:node_types:%s %d", nodes[i].c_str(), vid );
		}
	}
	if(DB_TYPE == 2)
	{
		db_querya("INSERT INTO vocabulary (name, description, help, relations, hierarchy, multiple, required, tags, model, weight) VALUES ( '%s', '%s', '%s', %d, %d, %d, %d, %d, '%s', %d )",
			cur_form["name"]["#value"].c_str(),
			cur_form["description"]["#value"].c_str(),
			cur_form["help"]["#value"].c_str(),
			isset( _POST["relations"] ) ? _POST["relations"].c_str():"0",
			isset( _POST["hierarchy"] ) ? _POST["hierarchy"].c_str():"0",
			isset( _POST["multiple"] ) ? _POST["multiple"].c_str():"0",
			isset( _POST["required"] ) ? _POST["required"].c_str():"0",
			isset( _POST["tags"] ) ? _POST["tags"].c_str():"0",
			"taxonomy",
			cur_form["weight"]["#value"].c_str()
			);

		string vid = db_last_insert_id();

		for(size_t i=0; i<nodes.size(); i++) {
			db_querya("INSERT INTO vocabulary_node_types (vid, type) VALUES (%d, '%s')", vid.c_str(), nodes[i].c_str() );
		}
	}
	
	redirect( url( "admin/taxonomy" ) );

	return "";
}

bool taxonomy_add_vocabulary_validate()
{
	return true;
}

string taxonomy_edit_vocabulary()
{
	map <string, map<string,string> > form;
	map <string, string> vocabulary;

	string vid = arg(4);

	taxonomy_get_vocabulary(vocabulary, vid);

	form["name"]["#value"] = vocabulary["name"];
	form["description"]["#value"] = vocabulary["description"];
	form["help"]["#value"] = vocabulary["help"];
	nestcopy( &vocabulary, "nodes", &form["nodes"], "#value");
	form["hierarchy"]["#value"] = vocabulary["hierarchy"];
	form["relations"]["#checked"] = vocabulary["relations"];
	form["tags"]["#checked"] = vocabulary["tags"];
	form["multiple"]["#checked"] = vocabulary["multiple"];
	form["required"]["#checked"] = vocabulary["required"];
	form["weight"]["#value"] = vocabulary["weight"];

	_vocabulary_form( form );

	form["delete"]["#type"] = "submit";
	form["delete"]["#value"] = "Delete";
	form["delete"]["#weight"] = "14";
  
	return get_form("taxonomy_edit_vocabulary", form);
}

bool taxonomy_edit_vocabulary_validate()
{
	return true;
}

string taxonomy_edit_vocabulary_submit()
{
	vector <string> nodes;
	string vid = arg(4);

	if(_POST["op"]=="Delete")	// Delete vocabulary
	{
		if(DB_TYPE==1)
		{
			int ivid = intval(vid);
			string type;
			REDIS_RES *result = redis_query("SMEMBERS vocabulary:%d:node_types", ivid );
			while( redis_fetch( result, type ) ) {
				redis_command("SREM vocabulary:node_types:%s %d", type.c_str(), ivid );
			}
			redis_command("DEL vocabulary:%d", ivid);
			redis_command("DEL vocabulary:%d:tids", ivid);
			redis_command("DEL vocabulary:%d:node_types", ivid );
			redis_command("SREM vocabulary %d", ivid);
		}
		if(DB_TYPE==2)
		{
			db_querya("DELETE FROM vocabulary WHERE vid=%d", vid.c_str() );
			db_querya("DELETE FROM vocabulary_node_types WHERE vid=%d", vid.c_str() );
		}
	}
	else						// Update vocabulary
	{		
		if(DB_TYPE==1)
		{
			int ivid = intval(vid);
			redis_command_fields(redis_arg("HMSET vocabulary:%d", ivid), "", 
								"name,description,help,relations,hierarchy,multiple,required,tags,weight",
								"%s,%s,%s,%s,%s,%s,%s,%s,%s",
								cur_form["name"]["#value"].c_str(),
								cur_form["description"]["#value"].c_str(),
								cur_form["help"]["#value"].c_str(),
								isset( _POST["relations"] ) ? _POST["relations"].c_str():"0",
								isset( _POST["hierarchy"] ) ? _POST["hierarchy"].c_str():"0",
								isset( _POST["multiple"] ) ? _POST["multiple"].c_str():"0",
								isset( _POST["required"] ) ? _POST["required"].c_str():"0",
								isset( _POST["tags"] ) ? _POST["tags"].c_str():"0",
								cur_form["weight"]["#value"].c_str() );

			string type;
			REDIS_RES *result = redis_query("SMEMBERS vocabulary:%d:node_types", ivid );
			while( redis_fetch( result, type ) ) {
				redis_command("SREM vocabulary:node_types:%s %d", type.c_str(), ivid );
			}

			redis_command("DEL vocabulary:%d:node_types", ivid );
			unnest( &cur_form["nodes"], "#value", nodes );
			for(size_t i=0; i<nodes.size(); i++) {
				redis_command("SADD vocabulary:%d:node_types %s", ivid, nodes[i].c_str() );
				redis_command("SADD vocabulary:node_types:%s %d", nodes[i].c_str(), ivid );
			}
		}
		if(DB_TYPE==2)
		{
			db_querya("UPDATE vocabulary SET name='%s', description='%s', help='%s', relations=%d, hierarchy=%d, multiple=%d, required=%d, tags=%d, weight=%d WHERE vid=%d",
				cur_form["name"]["#value"].c_str(),
				cur_form["description"]["#value"].c_str(),
				cur_form["help"]["#value"].c_str(),
				isset( _POST["relations"] ) ? _POST["relations"].c_str():"0",
				isset( _POST["hierarchy"] ) ? _POST["hierarchy"].c_str():"0",
				isset( _POST["multiple"] ) ? _POST["multiple"].c_str():"0",
				isset( _POST["required"] ) ? _POST["required"].c_str():"0",
				isset( _POST["tags"] ) ? _POST["tags"].c_str():"0",
				cur_form["weight"]["#value"].c_str(),
				vid.c_str()
				);

			db_querya("DELETE FROM vocabulary_node_types WHERE vid=%d", vid.c_str() );
			unnest( &cur_form["nodes"], "#value", nodes );
			for(size_t i=0; i<nodes.size(); i++) {
				db_querya("INSERT INTO vocabulary_node_types (vid, type) VALUES (%d, '%s')", vid.c_str(), nodes[i].c_str() );
			}
		}

	}

	redirect( url( "admin/taxonomy" ) );

	return "";
}

string taxonomy_admin()
{
	map <string, map <string, string> > form;
	map <string, string> item;
	
	form["categories"]["#type"] = "table";
	form["categories"]["#header"] = "Name,Type,Operations, , ";
	form["categories"]["#weight"] = "1";

	map <string, map<string,string> > voc;
	taxonomy_get_vocabularies( voc );
	
	int n = 0;
	for( map <string, map<string,string> >::iterator i = voc.begin(), end = voc.end();  i != end;  i++ )
	{
		form["name_"+i->first]["#type"] = "markup";
		form["name_"+i->first]["#value"] = i->second["name"];
		form["name_"+i->first]["#table"] = "categories";

		vector <string> node_types;
		unnest( &i->second, "nodes", node_types );
		form["type_"+i->first]["#type"] = "markup";
		form["type_"+i->first]["#value"] = implode(node_types, ", ");
		form["type_"+i->first]["#table"] = "categories";

		form["opt1_"+i->first]["#type"] = "markup";
		form["opt1_"+i->first]["#value"] = "<a href=\""+url("admin/taxonomy/edit/vocabulary/"+i->first)+"\">edit vocabulary</a>";
		form["opt1_"+i->first]["#table"] = "categories";

		form["opt2_"+i->first]["#type"] = "markup";
		form["opt2_"+i->first]["#value"] = "<a href=\""+url("admin/taxonomy/"+i->first)+"\">list terms</a>";
		form["opt2_"+i->first]["#table"] = "categories";

		form["opt3_"+i->first]["#type"] = "markup";
		form["opt3_"+i->first]["#value"] = "<a href=\""+url("admin/taxonomy/"+i->first+"/add/term")+"\">add terms</a>";
		form["opt3_"+i->first]["#table"] = "categories";

		form["categories"]["#row"+str(n)] = "name_"+i->first+",type_"+i->first+",opt1_"+i->first+",opt2_"+i->first+",opt3_"+i->first;
		n++;
	}

	return get_form("taxonomy_admin", form);
}

string taxonomy_overview_terms()
{
	vector <map <string, string> > terms;
	map <string, map <string, string> > form;
	map <string, string> item;
	string vid = arg(2);
	
	form["terms"]["#type"] = "table";
	form["terms"]["#header"] = "Name,Operations";
	form["terms"]["#weight"] = "1";

	vector <pair <string, string> > options;

	taxonomy_get_terms( vid, terms );

	options = taxonomy_make_tree( "0", terms );

	for(size_t i=0;i< options.size(); i++)
	{
		form["name_"+options[i].first]["#type"] = "markup";
		form["name_"+options[i].first]["#value"] = "<a href=\""+url("taxonomy/term/"+options[i].first)+"\">"+options[i].second+"</a>";
		form["name_"+options[i].first]["#table"] = "terms";

		form["opt1_"+options[i].first]["#type"] = "markup";
		form["opt1_"+options[i].first]["#value"] = "<a href=\""+url("admin/taxonomy/edit/term/"+options[i].first)+"\">edit</a>";
		form["opt1_"+options[i].first]["#table"] = "terms";

		form["terms"]["#row"+str(i)] = "name_"+options[i].first+",opt1_"+options[i].first;
	}

	return get_form("taxonomy_overview_terms", form);
}

void _term_form( map <string, map<string,string> > &form, string vid )
{
	vector <map <string, string> > terms;
	map <string, string> vocabulary;
	vector <pair <string, string> > options;
	
	taxonomy_get_vocabulary(vocabulary, vid);
	int hierarchy = intval( vocabulary["hierarchy"] );

	if( hierarchy > 0 )
	{
		form["parent"]["#type"] = "select";
		form["parent"]["#title"] = "Parent";

		if( hierarchy == 2 ) form["parent"]["#multiple"] = _TRUE;
		if( isset( form["tid"]["#value"] ) )
		{
			vector <string> term_parents;
			taxonomy_get_parents( form["tid"]["#value"], term_parents);
			nest(term_parents, &form["parent"], "#value");
		}

		taxonomy_get_terms( vid, terms );
		options = taxonomy_make_tree( "0", terms );
		options.insert( options.begin(), make_pair("0","<root>") );
		
		form["parent"]["#options"] = serialize_array( options );
	}

	form["name"]["#type"] = "textfield";
    form["name"]["#title"] = "Term name";
    form["name"]["#maxlength"] = "64";
    form["name"]["#description"] = "The name for this term. Example: \"Sports\"";
    form["name"]["#required"] = _TRUE;
	form["name"]["#weight"] = "1";

	form["description"]["#type"] = "textarea";
	form["description"]["#title"] = "Description";
    form["description"]["#description"] = "A description of the term.";
	form["description"]["#rows"] = "5";	
	form["description"]["#resizable"] = _TRUE;
	form["description"]["#weight"] = "2";
	
	form["synonyms"]["#type"] = "textarea";
	form["synonyms"]["#title"] = "Synonyms";
	form["synonyms"]["#description"] = "Synonyms of this term, one synonym per line.";
	form["synonyms"]["#rows"] = "5";
	form["synonyms"]["#resizable"] = _TRUE;    
	form["synonyms"]["#weight"] = "3";

	form["weight"]["#type"] = "weight";
	form["weight"]["#title"] = "Weight";
    form["weight"]["#description"] = "In listings, the heavier vocabularies will sink and the lighter vocabularies will be positioned nearer the top.";
	form["weight"]["#weight"] = "10";

	form["vid"]["#type"] = "hidden";
	form["vid"]["#value"] = vid;

	form["submit"]["#type"] = "submit";
	form["submit"]["#value"] = "Submit";
	form["submit"]["#weight"] = "13";

	return;
}

string taxonomy_add_term()
{
	map <string, map<string,string> > form;
	string	vid = arg(2);

	_term_form( form, vid );

	return get_form("taxonomy_add_term", form);
}

string taxonomy_add_term_submit()
{
	taxonomy_save_term( _POST );

	set_page_message( "Created new term <em>"+_POST["name"]+"</em>" , "status" );

	cur_form.clear();
	cur_form["vid"]["#type"] = "hidden";
	cur_form["vid"]["#value"] = _POST["vid"];

	//redirect( url( "admin/taxonomy/" + _POST["vid"] ) );

	return "";
}

bool taxonomy_add_term_validate()
{
	return true;
}

string taxonomy_edit_term()
{
	map <string, map<string,string> > form;
	map <string,string> term;
	string tid = arg(4);

	taxonomy_get_term( tid, term );	
	form["name"]["#value"] = term["name"];
	form["description"]["#value"] = term["description"];
	form["synonyms"]["#value"] = term["synonyms"];
	form["weight"]["#value"] = term["weight"];

	form["tid"]["#type"] = "hidden";
	form["tid"]["#value"] = tid;

	form["delete"]["#type"] = "submit";
	form["delete"]["#value"] = "Delete";
	form["delete"]["#weight"] = "14";
	
	_term_form( form, term["vid"] );
  
	return get_form("taxonomy_edit_vocabulary", form);
}

bool taxonomy_edit_term_validate()
{
	return true;
}

string taxonomy_edit_term_submit()
{	
	if( _POST["op"] == "Delete" ) _POST.erase( "name" );
	taxonomy_save_term( _POST );

	redirect( url( "admin/taxonomy/" + _POST["vid"] ) );

	return "";
}

int taxonomy_save_term( map<string,string> &form_values )
{
	if( isset( form_values["tid"] ) && isset( form_values["name"] ) )
	{
		// Update
		if(DB_TYPE==1)
		{
			int tid = intval(form_values["tid"]);
			
			int vid = redis_int("HGET term_data:%d vid", tid );
			string name = redis_str("HGET term_data:%d name", tid );
			redis_command("DEL vocabulary:%d:tid_by_name:%s", vid, name.c_str() );
			redis_command("SET vocabulary:%d:tid_by_name:%s %d", vid, strtolower(form_values["name"]).c_str(), tid);
			
			redis_command_fields(redis_arg("HMSET term_data:%d", tid ), "",
				"name,description,weight", "%s,%s,%s",
				form_values["name"].c_str(),
				form_values["description"].c_str(),
				form_values["weight"].c_str() );
		}
		if(DB_TYPE==2)
		{
			db_querya("UPDATE term_data SET name='%s', description='%s', weight=%d WHERE tid=%d",
				form_values["name"].c_str(),
				form_values["description"].c_str(),
				form_values["weight"].c_str(),
				form_values["tid"].c_str()
				);
		}
	}
	else if( isset( form_values["tid"] ) )
	{
		// Delete
		if(DB_TYPE==1)
		{
			int vid = redis_int("HGET term_data:%d vid", intval(form_values["tid"]) );
			string name = redis_str("HGET term_data:%d name", intval(form_values["tid"]) );
			redis_command("DEL term_data:%d", intval(form_values["tid"]) );
			redis_command("SREM vocabulary:%d:tids %d", vid, intval(form_values["tid"]) );
			redis_command("DEL vocabulary:%d:tid_by_name:%s", vid, name.c_str() );
			redis_command("DEL term_hierarchy:%d:parent", intval(form_values["tid"]) );
		}
		if(DB_TYPE==2)
		{
			db_querya("DELETE FROM term_data WHERE tid = %d", form_values["tid"].c_str() );
			db_querya("DELETE FROM term_hierarchy WHERE tid = %d", form_values["tid"].c_str() );
		}
		return 0;
	}
	else
	{
		// Insert
		if(DB_TYPE==1)
		{
			form_values["tid"] = redis_str("INCR term_data:ids");
			int tid = intval( form_values["tid"] );
			redis_command("SADD vocabulary:%d:tids %d", intval(form_values["vid"]), tid);			
			redis_command("SET vocabulary:%d:tid_by_name:%s %d", intval(form_values["vid"]), strtolower(form_values["name"]).c_str(), tid);
			redis_command_fields(redis_arg("HMSET term_data:%d", tid), "",
				"name,description,weight,vid", "%s,%s,%s,%d",
				form_values["name"].c_str(),
				form_values["description"].c_str(),
				form_values["weight"].c_str(),
				intval(form_values["vid"]) );
		}
		if(DB_TYPE==2)
		{
			db_querya("INSERT INTO term_data (name, description, weight, vid) VALUES ( '%s', '%s', %d, %d )",
				form_values["name"].c_str(),
				form_values["description"].c_str(),
				form_values["weight"].c_str(),
				form_values["vid"].c_str()
				);
			form_values["tid"] = db_last_insert_id();
		}
	}

	vector <string> parents;

	if(DB_TYPE==1)
	{
		redis_command("DEL term_hierarchy:%d:parent", intval(form_values["tid"]) );
		if ( !isset(form_values["parent"]) ) {
			form_values["parent"] = "0";
		}
		unnest( &form_values, "parent", parents );	
		for( size_t i=0 ; i< parents.size(); i++) {
			redis_command("SADD term_hierarchy:%d:parent %s", intval(form_values["tid"]), parents[i].c_str() );
		}
	}
	if(DB_TYPE==2)
	{
		db_querya("DELETE FROM term_hierarchy WHERE tid = %d", form_values["tid"].c_str() );
		if ( !isset(form_values["parent"]) ) {
			form_values["parent"] = "0";
		}
		unnest( &form_values, "parent", parents );	
		for( size_t i=0 ; i< parents.size(); i++) {
			db_querya("INSERT INTO term_hierarchy (tid, parent) VALUES (%d, %d)", form_values["tid"].c_str(), parents[i].c_str() );
		}
	}

	return 0;
}

string taxonomy_term_page()
{
	string out;
	map <string, string> term;
	map <string, string> node;
	string tid = num(arg(2));
	int count = 0;
	
	taxonomy_get_term( tid, term );
	set_page_title( term["name"] );

	if(DB_TYPE==1)
	{
		string set = "filter:"+microtime();
		redis_command("SINTERSTORE %s term_node:tid:%d node:status:1 node:moderate:0", set.c_str(), intval(tid) );
		REDIS_RES *result = redis_pager_fields( "SORT "+set+" BY node:*->sticky DESC BY node:*->created DESC", "GET node:*->", "#nid,language,title,status,promote,moderate,sticky,type,uid", 40);
		while( redis_fetch_fields( result, node ) )
		{
			node_load( node, node["nid"] );
			node["comment"] = "0";
			out += node_view( node );
			node.clear();
			count++;
		}
		redis_command("EXPIRE %s 10", set.c_str() );
	}
	if(DB_TYPE==2)
	{
		string sql = "SELECT DISTINCT(n.nid), n.sticky, n.title, n.created FROM node n INNER JOIN term_node tn ON n.nid = tn.nid WHERE tn.tid = " + tid + " AND n.status = 1 AND n.moderate = 0 ORDER BY n.sticky DESC, n.created DESC";
		MYSQL_RES *result = pager_query( sql_parse( sql.c_str() ), 10 );
		while ( db_fetch(result, node) ) 
		{
			node_load( node, node["nid"] );
			node["comment"] = "0";
			out += node_view( node );
			node.clear();
			count++;
		}
	}

	if(!count) {
		out += "There are currently no posts in this category.";
	} else {	
		out += theme_pager();
	}

	return out;
}

string taxonomy_autocomplete()
{
	string vid = arg(2);
	string str = _POST["str"];
	string out, list;

	vector <string> tags;
	size_t cut = str.find_last_of(',');
	if( cut != string::npos ) 
	{
		explode( tags, str.substr( 0, cut+1) );
		trim( tags );
		str = str.substr( cut+1 );	
	}
	trim(str);

	if( str.length() == 0 )
		out = "[  ]";
	else
	{	
		if(DB_TYPE==1)
		{
			string	tag;
			REDIS_RES *result = redis_query("KEYS vocabulary:%d:tid_by_name:*%s*", intval(vid), strtolower(str).c_str() );
			while ( redis_fetch(result, tag ) )
			{
				string key = implode(tags, ", ");
				size_t last = tag.find_last_of(":");
				if( last!=string::npos) {
					tag = tag.substr(last+1);				
					key += tag;
					list +=" \""+jsonencode(key)+"\": \""+jsonencode(tag)+"\",";
				}
			}
		}
		if(DB_TYPE==2)
		{
			map <string, string> tag;
			MYSQL_RES *result = db_querya("SELECT t.tid, t.name FROM term_data t WHERE t.vid = %d AND LOWER(t.name) LIKE LOWER('%%%s%%') LIMIT 0,10", vid.c_str(), str.c_str() );
			while ( db_fetch(result, tag ) )
			{
				string key = implode(tags, ", ");
				//if( key.length() > 0 ) key += ", ";
				key += tag["name"];
				list +=" \""+jsonencode(key)+"\": \""+jsonencode(tag["name"])+"\",";
			}
		}
		trim(list, ',');
		out = "{"+ list + " }";
	}

	print( out ) ;

	return "";
}
