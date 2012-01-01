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

#include "admin.h"

void admin_hooks()
{
	_MAP_THEME_HOOK( admin, page_theme );
	_MAP_THEME_HOOK( admin, node_theme );
	_MAP_THEME_HOOK( admin, aggregator_feed_source_theme );
	_MAP_THEME_HOOK( admin, aggregator_item_theme );
	_MAP_THEME_HOOK( admin, aggregator_wrapper_theme );
}

string admin_page_theme( map <string, string> item )
{
	string out;

	if( isset(item["is_front"]) )
	{		
		item["title"] = "Welcome to BinaryTiers Management";		
		item["content"] = "<p>Please select an option from the left menu</p>";
	}

	out = "\
<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\
<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\"\" xml:lang=\"\">\n\
<head>\n\
<base href=\""+string( BASE_URL )+"/\" />\n\
<link rel=\"stylesheet\" type=\"text/css\" media=\"all\" href=\"etc/binarytiers.css\" />\n\
<link rel=\"stylesheet\" type=\"text/css\" media=\"all\" href=\"themes/admin/style.css\" />\n\
<link rel=\"shortcut icon\" type=\"image/x-icon\" href=\"themes/admin/favicon.ico\" />\n\
"+stored_head+"\
<title>"+ item["head_title"] +"</title>\n\
</head>\n\
<body>\n\
<div style=\"font-size:14px; color: #959595; height: 72px; background: #e7e7e7; border-bottom: 1px solid #bbb;\">\n\
	<div style=\"position: absolute; top:13px; left:27px; \"><img src=\"themes/admin/img/logo.png\"></div>\n\
	<div style=\"float: right; line-height:70px; margin-right:60px\">"+string( BASE_URL ).substr(7)+" // <span style=\"color:#161616\">"+user["name"]+"</span></div>\n\
</div>\n\
<table border=\"0\" cellpadding=\"0\" cellspacing=\"0\" id=\"content\">\n\
	<tr>\n\
	";
if( !item["sidebar-left"].empty() ) out += "<td id=\"sidebar-left\">"+ item["sidebar-left"] +"</td>\n";
out += "\
		<td valign=\"top\">\n\
		<div id=\"main\">\n\
		"+ item["breadcrumb"] +"\
		<h1 class=\"title\">"+ item["title"] +"</h1>\n\
		<div class=\"tabs\">"+ item["tabs"] +"</div>\n\
		<div class=\"help\">"+ item["help"] +"</div>\n\
		"+ item["messages"] +"\n\
		"+ item["content"] +"\n\
		</div>\n\
		</td>\n\
	</tr>\n\
</table>\n\
</body>\n\
</html>\n\
";
	return (out);
}

string admin_node_theme( map <string, string> item )
{
	string out;

	out += "<div class=\"node" + (item["sticky"]==_TRUE?string(" sticky"):string("")) + ((item["status"]==_FALSE)?string(" node-unpublished"):string("")) + "\">";
    if (isset(item["picture"])) {
      out += item["picture"];
    }

    if ( item["page"]=="0" ) {
		out += "<h2 class=\"title\"><a href=\"" + item["node_url"] + "\">"+ item["title"] +"</a></h2>";
	}
    
	out += "<span class=\"submitted\">" + item["submitted"] + "</span>\n";
	if (isset(item["terms"])) {
		out += "<div class=\"taxonomy\">" + item["terms"] + "</div>\n";
	}
    out += "<div class=\"content\">" + item["content"] + "</div>\n";

	if (isset(item["links"])) { out += "<div class=\"links\">&raquo; "+ item["links"] + "</div>\n"; }
	
	out += "</div>\n";
	
	return out;
}

string admin_aggregator_feed_source_theme( map <string, string> item )
{
	string out;
	out ="\
<div class=\"feed-source\">\n\
  "+item["source_icon"]+"\n\
  "+item["source_image"]+"\n\
  <div class=\"feed-description\">\n\
    "+item["source_description"]+"\n\
  </div>\n\
  <div class=\"feed-url\">\n\
    <em>URL:</em> <a href=\""+item["source_url"]+"\">"+item["source_url"]+"</a>\n\
  </div>\n\
  <div class=\"feed-updated\">\n\
    <em>Updated:</em> "+item["last_checked"]+"\n\
  </div>\n\
</div>\
";
	return out;
}

string admin_aggregator_item_theme( map <string, string> item )
{
	string out;
	out ="\
<div class=\"feed-item\">\n\
  <h3 class=\"feed-item-title\">\n\
    <a style=\"color:#00A4DA\" href=\"" + item["feed_url"] + "\">"+ item["feed_title"] + "</a>\n\
  </h3>\n\
\n\
  <div class=\"feed-item-meta\" style=\"color:#75704A;\">\n\
    <span class=\"feed-item-date\" style=\"color:#B80102\">"+ item["source_date"] +"</span>\n";
if ( isset(item["source_url"]) ) { out +="\n\
    	- <a style=\"color:#75704A;\" href=\"" + item["source_url"] + "\" class=\"feed-item-source\">"+ item["source_title"] + "</a>\n";
}
out +="\n\
  </div>\n\
\n";
if ( isset(item["content"]) ) { out +="\
  <div class=\"feed-item-body\">\n\
    "+ item["content"]+ "\n\
  </div>\n";
  }
out +="\n\
\n";
if ( isset(item["categories"]) ) { out +="\
  <div class=\"feed-item-categories\">\n\
    Categories: \n\
  </div>\n";
	}
out +="\n\
</div>\n\
";
	return out;	
}

string admin_aggregator_wrapper_theme( map <string, string> item )
{
	string out;
	
	out += "\
<div id=\"aggregator\">\n\
  "+item["content"]+"\n\
  "+item["pager"]+"\n\
</div>\n";

	return out;
}
