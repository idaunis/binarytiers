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
#include "aggregator.h"

string element, tag;
map <string, string> channel;
map <string, string> image;
map < int, map <string, string> > items;
int item;

map <string, map<string,string> > feeds;

void aggregator_hooks()
{
	_MAP_DESC( aggregator, "Aggregator" );
	_MAP_VER ( aggregator, "1.3.1" );
	_MAP_PERM( aggregator, "administer news feeds, access news feeds" );
	_MAP_HOOK( aggregator, controllers );
	_MAP_HOOK( aggregator, schema );
	_MAP_HOOK( aggregator, cron );
}

void aggregator_schema()
{
	AssocOf(Assoc) aggregator_feed;
	aggregator_feed["#spec"]["description"] = "Stores feeds to be parsed by the aggregator.";
	aggregator_feed["#spec"]["name"] = "aggregator_feed";
	aggregator_feed["fid"]["type"] = "serial";
	aggregator_feed["fid"]["not null"] = _TRUE;
	aggregator_feed["fid"]["description"] = "Primary Key: Unique feed ID.";
	aggregator_feed["title"]["type"] = "varchar";
	aggregator_feed["title"]["length"] = "255";
	aggregator_feed["title"]["not null"] = _TRUE;
	aggregator_feed["title"]["default"] = "";
	aggregator_feed["title"]["description"] = "Title of the feed.";
	aggregator_feed["url"]["type"] = "varchar";
	aggregator_feed["url"]["length"] = "255";
	aggregator_feed["url"]["not null"] = _TRUE;
	aggregator_feed["url"]["default"] = "";
	aggregator_feed["url"]["description"] = "URL to the feed.";
	aggregator_feed["refresh"]["type"] = "int";
	aggregator_feed["refresh"]["not null"] = _TRUE;
	aggregator_feed["refresh"]["default"] = "0";
	aggregator_feed["refresh"]["description"] = "How often to check for new feed items, in seconds.";
	aggregator_feed["livetime"]["type"] = "int";
	aggregator_feed["livetime"]["not null"] = _TRUE;
	aggregator_feed["livetime"]["default"] = "1209600"; // 2 Weeks
	aggregator_feed["livetime"]["description"] = "Live time of the feed, as a Unix timestamp.";
	aggregator_feed["checked"]["type"] = "int";
	aggregator_feed["checked"]["not null"] = _TRUE;
	aggregator_feed["checked"]["default"] = "0";
	aggregator_feed["checked"]["description"] = "Last time feed was checked for new items, as Unix timestamp.";
	aggregator_feed["link"]["type"] = "varchar";
	aggregator_feed["link"]["length"] = "255";
	aggregator_feed["link"]["not null"] = _TRUE;
	aggregator_feed["link"]["default"] = "";
	aggregator_feed["link"]["description"] = "The parent website of the feed; comes from the &lt;link&gt; element in the feed.";
	aggregator_feed["description"]["type"] = "text";
	aggregator_feed["description"]["not null"] = _TRUE;
	aggregator_feed["description"]["size"] = "big";
	aggregator_feed["description"]["description"] = "The parent website's description; comes from the &lt;description&gt; element in the feed.";
	aggregator_feed["image"]["type"] = "text";
	aggregator_feed["image"]["not null"] = _TRUE;
	aggregator_feed["image"]["size"] = "big";
	aggregator_feed["image"]["description"] = "An image representing the feed.";
	aggregator_feed["etag"]["type"] = "varchar";
	aggregator_feed["etag"]["length"] = "255";
	aggregator_feed["etag"]["not null"] = _TRUE;
	aggregator_feed["etag"]["default"] = "";
	aggregator_feed["etag"]["description"] = "Entity tag HTTP response header, used for validating cache.";
	aggregator_feed["modified"]["type"] = "int";
	aggregator_feed["modified"]["not null"] = _TRUE;
	aggregator_feed["modified"]["default"] = "0";
	aggregator_feed["modified"]["description"] = "When the feed was last modified, as a Unix timestamp.";
	aggregator_feed["block"]["type"] = "int";
	aggregator_feed["block"]["not null"] = _TRUE;
	aggregator_feed["block"]["default"] = "0";
	aggregator_feed["block"]["size"] = "tiny";
	aggregator_feed["block"]["description"] = "Number of items to display in the feed's block.";
	aggregator_feed["#primary key"]["fid"] = "fid";
	aggregator_feed["#unique keys"]["url"] = "url";
	aggregator_feed["#unique keys"]["title"] = "title";
	poke( schema, aggregator_feed );

	AssocOf(Assoc) aggregator_item;
	aggregator_item["#spec"]["description"] = "Stores the individual items imported from feeds.";
	aggregator_item["#spec"]["name"] = "aggregator_item";
	aggregator_item["iid"]["type"] = "serial";
	aggregator_item["iid"]["not null"] = _TRUE;
	aggregator_item["iid"]["description"] = "Primary Key: Unique ID for feed item.";
	aggregator_item["fid"]["type"] = "int";
	aggregator_item["fid"]["not null"] = _TRUE;
	aggregator_item["fid"]["default"] = "0";
	aggregator_item["fid"]["description"] = "The {aggregator_feed}.fid to which this item belongs.";
	aggregator_item["title"]["type"] = "varchar";
	aggregator_item["title"]["length"] = "255";
	aggregator_item["title"]["not null"] = _TRUE;
	aggregator_item["title"]["default"] = "";
	aggregator_item["title"]["description"] = "Title of the feed item.";
	aggregator_item["link"]["type"] = "varchar";
	aggregator_item["link"]["length"] = "255";
	aggregator_item["link"]["not null"] = _TRUE;
	aggregator_item["link"]["default"] = "";
	aggregator_item["link"]["description"] = "Link to the feed item.";
	aggregator_item["author"]["type"] = "varchar";
	aggregator_item["author"]["length"] = "255";
	aggregator_item["author"]["not null"] = _TRUE;
	aggregator_item["author"]["default"] = "";
	aggregator_item["author"]["description"] = "Author of the feed item.";
	aggregator_item["description"]["type"] = "text";
	aggregator_item["description"]["not null"] = _TRUE;
	aggregator_item["description"]["size"] = "big";
	aggregator_item["description"]["description"] = "Body of the feed item.";
	aggregator_item["timestamp"]["type"] = "int";
	aggregator_item["timestamp"]["not null"] = _FALSE;
	aggregator_item["timestamp"]["description"] = "Post date of feed item, as a Unix timestamp.";
	aggregator_item["livetime"]["type"] = "int";
	aggregator_item["livetime"]["not null"] = _TRUE;
	aggregator_item["livetime"]["default"] = "1209600"; // 2 Weeks
	aggregator_item["livetime"]["description"] = "Live time of feed item, as a Unix timestamp.";
	aggregator_item["guid"]["type"] = "varchar";
	aggregator_item["guid"]["length"] = "255";
	aggregator_item["guid"]["not null"] = _FALSE;
	aggregator_item["guid"]["description"] = "Unique identifier for the feed item.";
	aggregator_item["#primary key"]["iid"] = "iid";
    aggregator_item["#indexes"]["fid"] = "fid";
	poke( schema, aggregator_item );
}

void aggregator_controllers()
{
	map <string, string> item;

	item["path"] = "admin/aggregator";
	item["title"] = "Feed aggregator";
	item["callback"] = _MAP( aggregator_admin_overview );
	item["access arguments"] = "administer news feeds";
	item["parent tab"] = "admin/aggregator";
	poke( controllers, item );

	item["path"] = "admin/aggregator";
	item["title"] = "List";
	item["weight"] = "1";
	item["callback"] = _MAP( aggregator_admin_overview );
	item["type"] = str( MENU_LOCAL_TASK );
	item["access arguments"] = "administer news feeds";
	item["parent tab"] = "admin/aggregator";
	poke( controllers, item );

	item["path"] = "admin/aggregator/add/feed";
	item["title"] = "Add feed";
	item["weight"] = "3";
	item["callback"] = _MAP_FORM( aggregator_add_feed );
	item["type"] = str( MENU_LOCAL_TASK );
	item["access arguments"] = "administer news feeds";
	item["parent tab"] = "admin/aggregator";
	poke( controllers, item );

	item["path"] = "admin/aggregator/update/feed/%";
	item["title"] = "Update items";
	item["callback"] = _MAP( aggregator_admin_refresh_feed );
	item["type"] = str( ~MENU_VISIBLE_IN_TREE );
	item["access arguments"] = "administer news feeds";
	poke( controllers, item );

	item["path"] = "admin/aggregator/remove/feed/%";
	item["title"] = "Remove items";
	item["callback"] = _MAP( aggregator_admin_remove_feed );
	item["type"] = str( ~MENU_VISIBLE_IN_TREE );
	item["access arguments"] = "administer news feeds";
	poke( controllers, item );

	item["path"] = "admin/aggregator/edit/feed/%";
	item["title"] = "Edit feed";
	item["callback"] = _MAP_FORM( aggregator_edit_feed );
	item["type"] = str( ~MENU_VISIBLE_IN_TREE );
	item["access arguments"] = "administer news feeds";
	poke( controllers, item );

	item["path"] = "aggregator/sources/%";
	item["callback"] = _MAP( aggregator_page_source );
	item["type"] = str( ~MENU_VISIBLE_IN_TREE );
	item["access arguments"] = "access news feeds";
	poke( controllers, item );
}

string aggregator_admin_overview()
{
	map <string, map <string, string> > form;
	vector < map <string, string> > feeds;
	map <string, string> feed;	

	set_page_title( "Feed aggregator" );
	
	form["feeds"]["#type"] = "table";
	form["feeds"]["#header"] = "Title,Items,Last update,Next update,Operations, , ";
	form["feeds"]["#weight"] = "1";
	form["feeds"]["#prefix"] = "<h3>Feed overview</h3>";

	if(DB_TYPE==1)
	{
		REDIS_RES *result = redis_query_fields("SORT aggregator_feed BY aggregator_feed:*->title ALPHA", "GET aggregator_feed:*->", "#fid,title,url,refresh,checked,link,description,etag,modified,image,block");
		while( redis_fetch_fields( result, feed ) ) {
			feed["items"] = redis_str("SCARD aggregator_item:fid:%d", intval(feed["fid"]) );
			feeds.push_back( feed );
		}
	}
	if(DB_TYPE==2)
	{
		MYSQL_RES * result = db_querya("SELECT f.*, COUNT(i.iid) AS items FROM aggregator_feed f LEFT JOIN aggregator_item i ON f.fid = i.fid GROUP BY f.fid, f.title, f.url, f.refresh, f.checked, f.link, f.description, f.etag, f.modified, f.image, f.block ORDER BY f.title");
		while ( db_fetch( result, feed ) ) {	
			feeds.push_back( feed );
		}
	}

	for(size_t n=0; n< feeds.size(); n++)
	{
		form["tit_"+str(n)]["#type"] = "markup";
		form["tit_"+str(n)]["#value"] = "<a href=\""+url("aggregator/sources/"+feeds[n]["fid"])+"\">"+feeds[n]["title"]+"</a>";
		form["tit_"+str(n)]["#table"] = "feeds";

		form["ite_"+str(n)]["#type"] = "markup";
		form["ite_"+str(n)]["#value"] = format_plural( intval(feeds[n]["items"]), "%i item", "%i items" );
		form["ite_"+str(n)]["#table"] = "feeds";

		form["ago_"+str(n)]["#type"] = "markup";
		form["ago_"+str(n)]["#value"] = intval( feeds[n]["checked"] ) ? format_interval(time() - intval(feeds[n]["checked"]) )+" ago" : "never";
		form["ago_"+str(n)]["#table"] = "feeds";

		form["lef_"+str(n)]["#type"] = "markup";
		form["lef_"+str(n)]["#value"] = intval( feeds[n]["checked"] ) ? format_interval( intval(feeds[n]["checked"]) + intval(feeds[n]["refresh"]) - time()) + " left" : "never";
		form["lef_"+str(n)]["#table"] = "feeds";

		form["edi_"+str(n)]["#type"] = "markup";
		form["edi_"+str(n)]["#value"] = "<a href=\""+url("admin/aggregator/edit/feed/"+feeds[n]["fid"])+"\">edit</a>";
		form["edi_"+str(n)]["#table"] = "feeds";

		form["rem_"+str(n)]["#type"] = "markup";
		form["rem_"+str(n)]["#value"] = "<a href=\""+url("admin/aggregator/remove/feed/"+feeds[n]["fid"])+"\">remove items</a>";
		form["rem_"+str(n)]["#table"] = "feeds";

		form["upd_"+str(n)]["#type"] = "markup";
		form["upd_"+str(n)]["#value"] = "<a href=\""+url("admin/aggregator/update/feed/"+feeds[n]["fid"])+"\">update items</a>";
		form["upd_"+str(n)]["#table"] = "feeds";
		
		form["feeds"]["#row"+str(n)] = "tit_"+str(n)+",ite_"+str(n)+",ago_"+str(n)+",lef_"+str(n)+",edi_"+str(n)+",rem_"+str(n)+",upd_"+str(n);
	}

	return get_form("aggregator_admin_overview", form);
}

XML_Parser my_xml_parser_create(string &data) 
{
	// Default XML encoding is UTF-8
	string encoding = "utf-8";
	bool bom = false;

	// Check for UTF-8 byte order mark (PHP5's XML parser doesn't handle it).
	// if (!strncmp($data, "\xEF\xBB\xBF", 3)) 
	if( !data.substr(0, 3).compare("\xEF\xBB\xBF") )
	{
		bom = true;
		data = data.substr(3);
	}

	// Check for an encoding declaration in the XML prolog if no BOM was found.
	vector <string>	match;
	if (!bom && preg_match((char*) "^<\?xml[^>]+encoding=\"([^\"]+)\"", (char *) data.c_str(), &match)) {
		encoding = match[1];
	}
	
	XML_Parser xml_parser;

	//xml_parser = XML_ParserCreate( (const XML_Char *) encoding.c_str() );
	
	xml_parser = XML_ParserCreate( NULL );

	//xml_parser_set_option($xml_parser, XML_OPTION_TARGET_ENCODING, 'utf-8');
	return xml_parser;
}

void aggregator_element_start(void * parser, const char *wname, const char **atts)
{
	map <string, string> attributes;
		
	int i = 0;
#if BT_SIZEOFWCHAR == 2
	size_t size_name = 256;
	size_t len;
	char * name = (char *) malloc( size_name + 1 );
	const wchar_t ** a = (const wchar_t **) atts;
	while( a[i] )
	{
		string key, value;
		len = wcslen( (const wchar_t *) a[i] );
		if( len > size_name ) {
			size_name = len;
			free( name );
			name = (char*) malloc( size_name + 1 );
		}
		wcstombs( name, (const wchar_t *) a[i], size_name);
		strtoupper( name );
		key = string( name );
		i++;
		
		len = wcslen( (const wchar_t *) a[i] );
		if( len > size_name ) {
			size_name = len;
			free( name );
			name = (char*) malloc( size_name + 1 );
		}
		wcstombs( name, (const wchar_t *) a[i], size_name);
		value = string( name );
		i ++;
		attributes[ key ] = value;
	}
	len = wcslen( (const wchar_t *) wname );
	if( len > size_name ) {
		size_name = len;
		free( name );
		name = (char*) malloc( size_name + 1 );
	}
	wcstombs( name, (const wchar_t *) wname, size_name);
#else
	int size_name = 0;
	char * name = (char *) wname;
	while( atts[i] )
	{
		string key, value;
		strtoupper( (char *) atts[i] );
		key = string( atts[i] );
		i++;
		value = string( atts[i] );
		i ++;
		attributes[ key ] = value;
	}
#endif
	
	strtoupper( name );

	if( !strcmp(name, "IMAGE" ) ||
		!strcmp(name, "TEXTINPUT" ) ||
		!strcmp(name, "CONTENT" ) ||
		!strcmp(name, "SUMMARY" ) ||
		!strcmp(name, "TAGLINE" ) ||
		!strcmp(name, "SUBTITLE" ) ||
		!strcmp(name, "LOGO" ) ||
		!strcmp(name, "INFO" ) )
	{
		element = string( name );
	}

	if( !strcmp(name, "ID" ) )
	{
		if (element != "ITEM") {
			element = name;
		}
	}

	if( !strcmp(name, "LINK" ) )
	{
		if ( isset(attributes["REL"]) && attributes["REL"] == "alternate") 
		{
			if ( element == "ITEM" ) 
			{
				items[item]["LINK"] = attributes["HREF"];
			}
			else 
			{
				channel["LINK"] = attributes["HREF"];
			}
		}	
	}

	if( !strcmp(name, "ITEM" ) )
	{
		item ++;
		element = name;
	}

	if( !strcmp(name, "ENTRY" ) )
	{
		item ++;
		element = "ITEM";
	}

	tag = name;
	
	if( size_name > 0 ) free( name );
}

void aggregator_element_end(void * parser, const char *wname)
{
	char name[64];

#if BT_SIZEOFWCHAR == 2
	wcstombs( name, (const wchar_t *) wname, 127);
#else
	strcpy( name, wname );
#endif
	strtoupper( name );
	
	if(	!strcmp(name, "IMAGE") ||
		!strcmp(name, "TEXTINPUT") ||
		!strcmp(name, "ITEM") ||
		!strcmp(name, "ENTRY") ||
		!strcmp(name, "CONTENT") ||
		!strcmp(name, "INFO") )
	{
		element = "";
	}
	if(	!strcmp(name, "ID") )
	{
		if (element == "ID")
		{
			element = "";
		}
	}  
}

void aggregator_element_data(void * parser, const XML_Char *s, int len)
{
	char * data = (char *) malloc( len + 4 );

#if BT_SIZEOFWCHAR == 2
	wcstombs( data, (const wchar_t *) s, len);
#else
	strncpy( data, s, len );
#endif

	data[len] = (char) NULL;

	if( element == "ITEM" )
	{
		items[item][tag] += data;
	}
	else if( element == "IMAGE" || element == "LOGO" )
	{
		image[tag] += data;
	}
	else if( element == "LINK" )
	{
		if ( len > 0) 
		{	
			items[item][tag] += data;
		}
	}
	else if( element == "CONTENT" )
	{
		items[item]["CONTENT"] += data;
	}
    else if( element == "SUMMARY" )
	{
		items[item]["SUMMARY"] += data;
	}
    else if( element == "TAGLINE" || element == "SUBTITLE" )
	{	
		channel["DESCRIPTION"] += data;
	}
	else if( element == "INFO" || element == "ID" || element == "TEXTINPUT" )
	{
		// The sub-element is not supported. However, we must recognize
		// it or its contents will end up in the item array.
	}
	else
	{
		channel[tag] += data;
	}

	free( data );
}

bool aggregator_parse_feed( string data, map <string, string> &feed )
{
	channel.clear();
	image.clear();
	items.clear();
	item = 0;

	XML_Parser xml_parser = my_xml_parser_create( data );

	XML_SetUserData(xml_parser, &xml_parser);
	XML_SetElementHandler(xml_parser, aggregator_element_start, aggregator_element_end);
	XML_SetCharacterDataHandler(xml_parser, aggregator_element_data);
	
	if( !XML_Parse(xml_parser, data.c_str(), (int) data.length(), 1) )
	{
		string error = "The feed from "+feed["title"]+" seems to be broken, because of error " +
			XML_ErrorString( XML_GetErrorCode( xml_parser ) ) + " on line " +
			str( XML_GetCurrentLineNumber( xml_parser ) ) + ".";
		set_page_message( error, "error");
		return false;
	}

	XML_ParserFree(xml_parser);

	for( int i = item; i>=0; i-- )
	{
		string	title, link, guid, date;

		for( map <string, string>::iterator curr = items[i].begin(), end = items[i].end();  curr != end;  curr++ )
		{
			trim( curr->second );
			items[i][curr->first] = curr->second;
		}
		
		// Resolve the item's title. If no title is found, we use up to 40
		// characters of the description ending at a word boundary but not
		// splitting potential entities.
		if (isset(items[i]["TITLE"]))
		{
			title = items[i]["TITLE"];
		}
		else if (isset(items[i]["DESCRIPTION"]))
		{
			title = items[i]["DESCRIPTION"].substr(0, 40);
		}
		else
		{
			title = "";
		}

		// Resolve the items link.
		if (isset(items[i]["LINK"])) 
		{
			link = items[i]["LINK"];
		}
		else 
		{
			link = feed["link"];
		}
		guid = isset(items[i]["GUID"]) ? items[i]["GUID"] : "";

		// Atom feeds have a CONTENT and/or SUMMARY tag instead of a DESCRIPTION tag.
		if (isset(items[i]["CONTENT:ENCODED"])) {
			items[i]["DESCRIPTION"] = items[i]["CONTENT:ENCODED"];
		}
		else if (isset(items[i]["SUMMARY"])) {
			items[i]["DESCRIPTION"] = items[i]["SUMMARY"];
		}
		else if (isset(items[i]["CONTENT"])) {
			items[i]["DESCRIPTION"] = items[i]["CONTENT"];
		}

		if (isset(items[i]["DC:CREATOR"])) {
			items[i]["AUTHOR"] = items[i]["DC:CREATOR"];
		}

		// Try to resolve and parse the item's publication date. If no date is
		// found, we use the current date instead.
		date = "now";
		if (isset(items[i]["PUBDATE"])) { date = items[i]["PUBDATE"]; }
		if (isset(items[i]["DC:DATE"])) { date = items[i]["DC:DATE"]; }
		if (isset(items[i]["DCTERMS:ISSUED"])) { date = items[i]["DCTERMS:ISSUED"]; }
		if (isset(items[i]["DCTERMS:CREATED"])) { date = items[i]["DCTERMS:CREATED"]; }
		if (isset(items[i]["DCTERMS:MODIFIED"])) { date = items[i]["DCTERMS:MODIFIED"]; }
		if (isset(items[i]["ISSUED"])) { date = items[i]["ISSUED"]; }
		if (isset(items[i]["CREATED"])) { date = items[i]["CREATED"]; }
		if (isset(items[i]["MODIFIED"])) { date = items[i]["MODIFIED"]; }
		if (isset(items[i]["PUBLISHED"])) { date = items[i]["PUBLISHED"]; }
		if (isset(items[i]["UPDATED"])) { date = items[i]["UPDATED"]; }

		string timestamp = strtotime( date );
		if( timestamp == "") timestamp = str( time() );

		map <string, string> entry;

		if(DB_TYPE==1)
		{
			if( isset( guid ) )
			{
				map <string, string> item;
				REDIS_RES *result = redis_query_fields( redis_arg("SORT aggregator_item:fid:%d BY nosort", intval(feed["fid"])), "GET aggregator_item:*->", "#iid,guid");
				while( redis_fetch_fields( result, item ) ) {
					if( item["guid"] == guid) {
						redis_free(result); 
						entry["iid"] = item["iid"];
						break;
					}
				}
			}
			else if (isset(link) && link != feed["link"] && link != feed["url"])
			{
				map <string, string> item;
				REDIS_RES *result = redis_query_fields( redis_arg("SORT aggregator_item:fid:%d BY nosort", intval(feed["fid"])), "GET aggregator_item:*->", "#iid,link");
				while( redis_fetch_fields( result, item ) ) {
					if( entry["link"] == link) {
						redis_free(result); 
						entry["iid"] = item["iid"];
						break;
					}
				}
			}
			else
			{
				map <string, string> item;
				REDIS_RES *result = redis_query_fields( redis_arg("SORT aggregator_item:fid:%d BY nosort", intval(feed["fid"])), "GET aggregator_item:*->", "#iid,title");
				while( redis_fetch_fields( result, item ) ) {
					if( entry["title"] == title) {
						redis_free(result); 
						entry["iid"] = item["iid"];
						break;
					}
				}
			}
		}
		if(DB_TYPE==2)
		{
			if( isset( guid ) )
			{
				db_fetch(db_querya("SELECT iid FROM aggregator_item WHERE fid = %d AND guid = '%s'", feed["fid"].c_str(), guid.c_str()), entry );
			}
			else if (isset(link) && link != feed["link"] && link != feed["url"])
			{
				db_fetch(db_querya("SELECT iid FROM aggregator_item WHERE fid = %d AND link = '%s'", feed["fid"].c_str(), link.c_str()), entry );
			}
			else {
				db_fetch(db_querya("SELECT iid FROM aggregator_item WHERE fid = %d AND title = '%s'", feed["fid"].c_str(), title.c_str()), entry );
			}
		}

		entry["fid"] = feed["fid"];
		entry["livetime"] = feed["livetime"];
		entry["timestamp"] = timestamp;
		entry["title"] = title;
		entry["link"] = link;
		entry["author"] = items[i]["AUTHOR"];
		entry["description"] = items[i]["DESCRIPTION"];
		entry["guid"] = guid;

		aggregator_save_item( entry );
	}

	// Remove all items that are older than flush item timer.
	if(DB_TYPE==1)
	{
	}
	if(DB_TYPE==2)
	{
		db_querya("DELETE FROM aggregator_item WHERE fid = %d AND livetime>0 AND timestamp < unix_timestamp()-livetime", feed["fid"].c_str() );
	}
	
	return true;
}

void aggregator_save_item(map <string, string> edit)
{
	if(DB_TYPE==1)
	{
		if ( isset(edit["iid"]) && isset(edit["title"]) ) 
		{
			redis_command("HMSET aggregator_item:%d fid %d title %s link %s author %s description %s timestamp %d guid %s", 
				intval(edit["iid"]), intval(edit["fid"]), edit["title"].c_str(), edit["link"].c_str(), edit["author"].c_str(), edit["description"].c_str(), intval(edit["timestamp"]), edit["guid"].c_str() );
		}
		else if ( isset(edit["iid"]) ) 
		{
			redis_command("SREM aggregator_item:fid:%d %d", redis_int("HGET aggregator_item:%d fid", intval(edit["iid"]) ), intval(edit["iid"]) );
			redis_command("SREM aggregator_item %d", intval(edit["iid"]) );
			redis_command("DEL aggregator_item:%d", intval(edit["iid"]) );
		}
		else if ( isset( edit["title"] ) && isset( edit["link"] ) ) 
		{
			edit["iid"] = redis_str("INCR aggregator_item:ids");
			redis_command("SADD aggregator_item %d", intval(edit["iid"]) );
			redis_command("SADD aggregator_item:fid:%d %d", intval(edit["fid"]),  intval(edit["iid"]) );
			redis_command("HMSET aggregator_item:%d fid %d title %s link %s author %s description %s timestamp %d guid %s", 
				intval(edit["iid"]), intval(edit["fid"]), edit["title"].c_str(), edit["link"].c_str(), edit["author"].c_str(), edit["description"].c_str(), intval(edit["timestamp"]), edit["guid"].c_str() );
		}
	}

	if(DB_TYPE==2)
	{
		if ( isset(edit["iid"]) && isset(edit["title"]) ) 
		{
			db_querya("UPDATE aggregator_item SET title = '%s', link = '%s', author = '%s', description = '%s', guid = '%s', timestamp = %d WHERE iid = %d", edit["title"].c_str(), edit["link"].c_str(), edit["author"].c_str(), edit["description"].c_str(), edit["guid"].c_str(), edit["timestamp"].c_str(), edit["iid"].c_str() );
		}
		else if ( isset(edit["iid"]) ) 
		{
			db_querya("DELETE FROM aggregator_item WHERE iid = %d", edit["iid"].c_str() );
		}
		else if ( isset( edit["title"] ) && isset( edit["link"] ) ) 
		{
			db_querya("INSERT INTO aggregator_item (fid, title, link, author, description, timestamp, guid) VALUES (%d, '%s', '%s', '%s', '%s', %d, '%s')", edit["fid"].c_str(), edit["title"].c_str(), edit["link"].c_str(), edit["author"].c_str(), edit["description"].c_str(), edit["timestamp"].c_str(), edit["guid"].c_str() );
		}

		edit["iid"] = db_last_insert_id();
	}
}

void aggregator_save_feed( map <string, string> &edit )
{
	if(DB_TYPE==1)
	{
		if (isset(edit["fid"]) && isset(edit["title"]))
		{
			redis_command("HMSET aggregator_feed:%d title %s url %s refresh %d", intval(edit["fid"]), edit["title"].c_str(), edit["url"].c_str(), intval(edit["refresh"]) );
		}
		else if (isset( edit["fid"] ))
		{
			string iid;
			redis_command("DEL aggregator_feed:%d", intval(edit["fid"]) );
			REDIS_RES * result = redis_query("SMEMBERS aggregator_item:fid:%d", intval(edit["fid"]) );
			while( redis_fetch( result, iid ) ) {
				redis_command("DEL aggregator_item:%d", intval(iid) );
				redis_command("SREM aggregator_item %d", intval(iid) );
			}
			redis_command("DEL aggregator_item:fid:%d", intval(edit["fid"]) );
		}
		else if ( isset(edit["title"]) )
		{
			edit["fid"] = redis_str("INCR aggregator_feed:ids");
			redis_command("SADD aggregator_feed %d", intval(edit["fid"]) );
			redis_command("HMSET aggregator_feed:%d title %s url %s refresh %d block %d", intval(edit["fid"]), edit["title"].c_str(), edit["url"].c_str(), intval(edit["refresh"]), 5 );
		}
	}
	
	if(DB_TYPE==2)
	{
		if (isset(edit["fid"]) && isset(edit["title"]))
		{
			db_querya("UPDATE aggregator_feed SET title = '%s', url = '%s', refresh = %d WHERE fid = %d", edit["title"].c_str(), edit["url"].c_str(), edit["refresh"].c_str(), edit["fid"].c_str() );
		}
		else if (isset( edit["fid"] ))
		{
			db_querya("DELETE FROM aggregator_feed WHERE fid = %d", edit["fid"].c_str() );
			db_querya("DELETE FROM aggregator_item WHERE fid = %d", edit["fid"].c_str() );
		}
		else if ( isset(edit["title"]) )
		{
			db_querya("INSERT INTO aggregator_feed (title, url, refresh, block, description, image) VALUES ('%s', '%s', %d, 5, '', '')", edit["title"].c_str(), edit["url"].c_str(), edit["refresh"].c_str() );
			edit["fid"] = db_last_insert_id();
		}
	}	
}

void aggregator_refresh(map <string, string> &feed)
{
	// Generate conditional GET headers.
	map <string, string> headers;
  
	if ( isset(feed["etag"]) ) 
	{
		headers["If-None-Match"] = feed["etag"];
	}
	if ( isset(feed["modified"]) ) 
	{
		headers["If-Modified-Since"] = gmdate("D, d M Y H:i:s", feed["modified"]) + " GMT";
	}
	
	map <string, string> result;
  
	// Request feed.
	http_request( result, feed["url"], headers );

	// Process HTTP response code.
	switch ( intval( result["response_code"] ) ) 
	{
		case 304:
			if(DB_TYPE==1)
				redis_command("HSET aggregator_feed:%d checked %d", intval(feed["fid"]), time() );
			if(DB_TYPE==2)
				db_querya("UPDATE aggregator_feed SET checked = %d WHERE fid = %d", str( time() ).c_str(), feed["fid"].c_str() );

			set_page_message( "There is no new syndicated content from " + feed["title"] );
			break;
		case 301:
			feed["url"] = result["redirect_url"];
		case 200:
		case 302:
		case 307:
			// Filter the input data:
			if ( aggregator_parse_feed( result["data"], feed ) ) 
			{
				string modified = !isset2(result,"Last-Modified") ? "0" : strtotime( result["Last-Modified"] );

				// Prepare the channel data.
				for( map <string, string>::iterator curr = channel.begin(), end = channel.end();  curr != end;  curr++ )
				{
					trim( curr->second );
					channel[curr->first] = curr->second;
				}

				// Prepare the image data (if any).
				for( map <string, string>::iterator curr = image.begin(), end = image.end();  curr != end;  curr++ )
				{
					trim( curr->second );
					image[curr->first] = curr->second;
				}

				string str_image;
				if (isset(image["LINK"]) && isset(image["URL"]) && isset(image["TITLE"]) ) 
				{
					// Note, we should really use theme_image() here but that only works with local images it won't work with images fetched with a URL unless PHP version > 5
					str_image = "<a href=\""+ check_url(image["LINK"]) +"\" class=\"feed-image\"><img src=\"" + check_url(image["URL"]) + "\" alt=\"" + check_plain(image["TITLE"]) +"\" /></a>";
				}
				else {
					str_image = "";
				}

				string etag = !isset(result["ETag"]) ? "" : result["ETag"];
				// Update the feed data.

				if(DB_TYPE==1)
				{
					redis_command_fields(redis_arg("HMSET aggregator_feed:%d", intval(feed["fid"]) ), "", 
						"url,checked,link,description,image,etag,modified", "%s,%d,%s,%s,%s,%s,%d", 
						feed["url"].c_str(), 
						time(), 
						channel["LINK"].c_str(), 
						channel["DESCRIPTION"].c_str(), 
						str_image.c_str(), 
						etag.c_str(), 
						intval( modified ) );
				}
				if(DB_TYPE==2)
				{
					db_querya("UPDATE aggregator_feed SET url = '%s', checked = %d, link = '%s', description = '%s', image = '%s', etag = '%s', modified = %d WHERE fid = %d", 
						feed["url"].c_str(), 
						str(time()).c_str(), 
						channel["LINK"].c_str(), 
						channel["DESCRIPTION"].c_str(), 
						str_image.c_str(), 
						etag.c_str(), 
						modified.c_str(), 
						feed["fid"].c_str() );
				}

				set_page_message( "There is new syndicated content from " + feed["title"] );
				break;
			}
			result["error"] = "feed not parseable";
		default:
			set_page_message( "The feed from "+feed["title"]+" seems to be broken, because of error \""+result["response_code"]+"\". "+ result["error"] );
	}
}

void aggregator_feed_form( map <string, map <string, string> > &form, map <string, string> item )
{	
  int refresh[] = {900, 1800, 3600, 7200, 10800, 21600, 32400, 43200, 64800, 86400, 172800, 259200, 604800, 1209600, 2419200, -1};
	int livetime[] = {86400, 172800, 259200, 604800, 1209600, 2419200, 4838400, 14515200, 29030400, -1};
	int i = 0;
	vector <pair <string, string> > refresh_options;
	vector <pair <string, string> > livetime_options;

	while( refresh[i] > 0 ) {
		refresh_options.push_back( make_pair( str(refresh[i]), format_interval(refresh[i]) ) );
		i++;
	}

	i = 0;
	livetime_options.push_back( make_pair( "0", "Forever" ) );
	while( livetime[i] > 0 ) {
		livetime_options.push_back( make_pair( str(livetime[i]), format_interval(livetime[i]) ) );
		i++;
	}
  
	form["title"]["#type"] = "textfield";
	form["title"]["#title"] = "Title";
	form["title"]["#value"] = item["title"];
	form["title"]["#maxlength"] = "255";
	form["title"]["#description"] = "The name of the feed (or the name of the website providing the feed).";
	form["title"]["#required"] = _TRUE;
	form["title"]["#weight"] = "1";

	form["url"]["#type"] = "textfield";
	form["url"]["#title"] = "URL";
	form["url"]["#value"] = item["url"];
	form["url"]["#maxlength"] = "255";
	form["url"]["#description"] = "The fully-qualified URL of the feed.";
	form["url"]["#required"] = _TRUE;
	form["url"]["#weight"] = "2";

	form["refresh"]["#type"] = "select";
	form["refresh"]["#title"] = "Update interval";
	form["refresh"]["#value"] = item["refresh"];
	form["refresh"]["#options"] = serialize_array( refresh_options );
	form["refresh"]["#description"] = "The length of time between feed updates. (Requires a correctly configured Cron)";
	form["refresh"]["#weight"] = "3";

  form["livetime"]["#type"] = "select";
	form["livetime"]["#title"] = "Live Time";
	form["livetime"]["#value"] = item["livetime"];
	form["livetime"]["#options"] = serialize_array( livetime_options );
	form["livetime"]["#description"] = "The livetime of the feed.";
	form["livetime"]["#weight"] = "4";

	form["submit"]["#type"] = "submit";
	form["submit"]["#value"] = "Save";
	form["submit"]["#weight"] = "5";
}

void aggregator_feed_load(map <string, string> &feed, string fid) 
{
	if ( !isset(feeds[fid]) ) 
	{
		if(DB_TYPE==1) {
			redis_multi( feed, "HGETALL aggregator_feed:%d", intval(fid) );
			feed["fid"] = fid;
		}
		if(DB_TYPE==2)
			db_fetch( db_querya("SELECT * FROM aggregator_feed WHERE fid = %d", fid.c_str()), feed );
		feeds[fid] = feed;
	}
	feed = feeds[fid];
	return;
}

string aggregator_edit_feed()
{
	map <string, map <string, string> > form;
	map <string, string> feed;

	aggregator_feed_load( feed, arg(4) );

	aggregator_feed_form( form, feed );

	form["delete"]["#type"] = "submit";
	form["delete"]["#value"] = "Delete";
	form["delete"]["#weight"] = "5";
			
	return get_form("aggregator_edit_feed", form);
}

string aggregator_edit_feed_submit()
{
	map <string, string> edit;

	edit["fid"] = arg(4);
	if( _POST["op"] == "Delete" )
	{
		aggregator_remove( edit );
		db_querya("DELETE FROM aggregator_feed WHERE fid = %d", edit["fid"].c_str() );		
	}
	else
	{		
		edit["title"] = _POST["title"];
		edit["url"] = _POST["url"];
		edit["refresh"] = _POST["refresh"];
    edit["livetime"] = _POST["livetime"];

		aggregator_save_feed( edit );
	}
	redirect( url( "admin/aggregator" ) );
	return "";
}

bool aggregator_edit_feed_validate()
{
	return true;
}

string aggregator_add_feed()
{	
	map <string, map <string, string> > form;
	map <string, string> feed;

	aggregator_feed_form( form, feed );		
	return get_form("aggregator_add_feed", form);
}

string aggregator_add_feed_submit()
{
	map <string, string> edit;

	edit["title"] = _POST["title"];
	edit["url"] = _POST["url"];
	edit["refresh"] = _POST["refresh"];
  edit["livetime"] = _POST["livetime"];

	aggregator_save_feed( edit );
	redirect( url( "admin/aggregator" ) );
	return "";
}
bool aggregator_add_feed_validate()
{
	return true;
}

void aggregator_remove( map <string, string> feed ) 
{
	if(DB_TYPE==1)
	{
		string iid;
		REDIS_RES * result = redis_query("SMEMBERS aggregator_item:fid:%d", intval(feed["fid"]) );
		while( redis_fetch( result, iid ) ) {
			redis_command("DEL aggregator_item:%d", intval(iid) );
			redis_command("SREM aggregator_item %d", intval(iid) );
		}
		redis_command("DEL aggregator_item:fid:%d", intval(feed["fid"]) );
		redis_command("HMSET aggregator_feed:%d checked %d etag %s modified %d", intval(feed["fid"]), 0, "", 0 );
	}

	if(DB_TYPE==2)
	{
		db_querya("DELETE FROM aggregator_item WHERE fid = %d", feed["fid"].c_str() );
		db_querya("UPDATE aggregator_feed SET checked = 0, etag = '', modified = 0 WHERE fid = %d", feed["fid"].c_str() );
	}
	
	set_page_message("The news items from "+feed["title"]+" have been removed.");
}

string aggregator_admin_refresh_feed()
{
	map <string, string> feed;

	aggregator_feed_load( feed, arg(4) );
	aggregator_refresh( feed );
	redirect( url( "admin/aggregator" ) );

	return "";
}

string aggregator_admin_remove_feed()
{
	map <string, string> feed;

	aggregator_feed_load( feed, arg(4) );
	aggregator_remove( feed );
	redirect( url( "admin/aggregator" ) );

	return "";
}

string aggregator_filter_xss(string value) 
{
	return filter_xss(value, variable_get("aggregator_allowed_html_tags", "<a> <b> <br> <dd> <dl> <dt> <em> <i> <li> <ol> <p> <strong> <u> <ul> <iframe>") );
}

void template_preprocess_aggregator_feed_source(map <string, string> &variables) 
{
	map <string, string> feed = variables;
	  
	variables["source_icon"] = theme("feed_icon", feed);
	variables["source_image"] = feed["image"];
	variables["source_description"] = aggregator_filter_xss(feed["description"]);
	
	map <string, string> url_options;
	url_options["absolute"] = _TRUE;
	variables["source_url"] = check_url(url( feed["link"], &url_options ));

	if ( isset(feed["checked"]) ) {
		variables["last_checked"] = format_interval( time() - intval(feed["checked"]) ) + " ago";
	}
	else {
		variables["last_checked"] = "never";
	}

	if ( user_access("administer news feeds") ) {
		variables["last_checked"] = "<a href=\""+ url("admin/content/aggregator") +"\">"+ variables["last_checked"] + "</a>";
	}
}

void template_preprocess_aggregator_item( map <string, string> &variables ) 
{
	map <string, string> item = variables;

	variables["feed_url"] = check_url( item["link"] );
	variables["feed_title"] = check_plain( item["title"] );
	variables["content"] = aggregator_filter_xss( item["description"] );

	variables["source_url"] = "";
	variables["source_title"] = "";
	if ( isset(item["ftitle"]) && isset(item["fid"]) ) 
	{
		variables["source_url"] = url("aggregator/sources/"+item["fid"]);
		variables["source_title"] = check_plain(item["ftitle"]);
	}
	if ( date("Ymd", item["timestamp"]) == date("Ymd") ) 
	{
		variables["source_date"] = format_interval(time() - intval(item["timestamp"]) ) + " ago";
	}
	else 
	{
		variables["source_date"] = format_date( item["timestamp"] );
	}
}

void aggregator_feed_items_load( vector <map <string,string> > &items, int fid, int limit ) 
{
	map <string, string> item;

	if(DB_TYPE==1)
	{
		REDIS_RES * result;
		result = redis_pager_fields( redis_arg("SORT aggregator_item:fid:%d BY aggregator_item:*->timestamp DESC BY # DESC", fid), 
					"GET aggregator_item:*->", "#iid,fid,title,link,author,description,timestamp,guid", limit);
		while( redis_fetch_fields( result, item ) ) {
			items.push_back( item );
		}
	}
	if(DB_TYPE==2)
	{
		string sql = "SELECT * FROM aggregator_item WHERE fid = "+ str(fid) + " ORDER BY timestamp DESC, iid DESC";
		MYSQL_RES * result = pager_query(sql, limit);
		while ( db_fetch(result, item) ) {		
			items.push_back( item );
		}
	}

	return;
}

string _aggregator_page_list(vector <map <string, string> > items, string op, string feed_source = "") 
{
	string	output;

	output = feed_source;
	for( size_t i=0; i< items.size(); i++ )
	{
		template_preprocess_aggregator_item( items[i] );
		output += theme( "aggregator_item", items[i] );
	}
	map <string, string> content_item;
	content_item["content"] = output;
	content_item["pager"] = theme_pager();
	output = theme( "aggregator_wrapper", content_item );

	return output;
}

string aggregator_page_source()
{
	map <string, string> feed;

	aggregator_feed_load( feed, arg(2) );

	set_page_title( check_plain( feed["title"] ) );

	template_preprocess_aggregator_feed_source( feed );

	string feed_source = theme("aggregator_feed_source", feed);

	vector <map <string, string> > items;

	aggregator_feed_items_load( items, intval(feed["fid"]), 20 );

	return _aggregator_page_list( items, "", feed_source);
}

void aggregator_cron()
{
	map <string, string> feed;
	MYSQL_RES * result = db_querya("SELECT * FROM aggregator_feed WHERE checked+refresh<%d", str(time()).c_str() );
	while ( db_fetch( result, feed ) ) 
	{
		aggregator_refresh( feed );
	}
}
