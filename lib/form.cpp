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

string form_label( map <string, string> *item )
{
	string out;

	if( !(*item)["#title"].empty() )
	{
		out += "<label for=\""+(*item)["#name"]+"\">"+(*item)["#title"]+": ";

		if( (*item)["#required"] == _TRUE )
			out += "<span class=\"form-required\" title=\"This field is required.\">*</span>";

		out += "</label>\n";
	}

	return out;
}

string form_file( map <string, string> *item)
{
	string out;
	string classes = "form-file";
		
	if( (*item)["#required"] == _TRUE ) classes += " required";
	if( (*item)["#error"] == _TRUE ) classes += " error";

	out += form_label( item );
	
	out += "<input type=\"file\" name=\""+(*item)["#name"]+"\"";
	if( !(*item)["#size"].empty() ) out += " size=\""+(*item)["#size"]+"\"";
	if( !classes.empty() ) out += " class=\""+classes+"\"";
	out += "/>\n";
	
	if( !(*item)["#description"].empty() ) out +=	"<div class=\"description\">"+(*item)["#description"]+"</div>";
	
	return out;
}

string form_checkbox( map <string, string> *item )
{
	vector <string> options;
	string out;
	string classes = "form-checkbox";
	string label_classes = "option";
	string value = (*item)["#value"];
	string visible_value;

	add_js("etc/tableselect.js");
	add_js("etc/tableheader.js");
		
	if( (*item)["#required"] == _TRUE ) {classes += " required";label_classes += " required";}
	if( (*item)["#error"] == _TRUE ) {classes += " error";label_classes += " error";}

	/*
	if( isset( (*item)["#options"] ) )
	{
		explode(options, (*item)["#options"], ',');
		visible_value = options[ intval( value ) ];
	}
	*/

	if( isset( (*item)["#title"] ) )
	{
		visible_value = (*item)["#title"];
	}
	
	out += "<div class=\"form-item\">\n";
	out += "<label";
	if( !label_classes.empty() ) out += " class=\""+label_classes+"\"";
	out += "><input type=\"checkbox\" name=\""+(*item)["#name"]+"\"";
	out += " value=\""+ value +"\"";

	if( (*item)["#checked"] == _TRUE ) out += " checked";
	if( !(*item)["#disabled"].empty() ) out += " disabled";
	
	if( !classes.empty() ) out += " class=\""+classes+"\"";
	out += ">"+visible_value;
	out += "</label></div>\n";
	return out;
}

string form_select( map <string, string> *item )
{
	vector <pair <string, string> > options;
	string out;
	string classes = "form-select";
	vector <string> value;
	size_t	cut, len;
	
	if( (cut = (*item)["#value"].find("#array[")) != string::npos && cut == 0) // is an array
	{
		len = intval( (*item)["#value"].substr(7) );
		for( int i=0; i<(int)len; i++) {
			value.push_back( (*item)["#value["+str(i)+"]"] );
		}
	}
	else
	{
		value.push_back( (*item)["#value"] );
	}
		
	if( (*item)["#required"] == _TRUE ) classes += " required";
	if( (*item)["#error"] == _TRUE ) classes += " error";

	out += form_label( item );

	// explode(options, (*item)["#options"], ',');

	unserialize_array( (*item)["#options"], options );

	out += "<select id=\""+(*item)["#name"]+"\" name=\""+(*item)["#name"]+"\"";
	if( (*item)["#multiple"] == _TRUE ) out += " multiple=\"multiple\"";
	if( isset( (*item)["#size"] ) ) out += " size=\""+(*item)["#size"]+"\"";
	if( !(*item)["#attributes"].empty() ) out += " "+(*item)["#attributes"];
	if( !classes.empty() ) out += " class=\""+classes+"\"";
	out += ">";

	for( size_t i = 0; i < options.size(); i++ )
	{
		vector <pair <string, string> > sub_options;
		unserialize_array( options[i].second, sub_options );

		if( sub_options.size() > 0 ) // Has Group of Options
		{
			out += "<optgroup label=\""+options[i].first+"\">";
			for( size_t j = 0; j < sub_options.size(); j++ )
			{
				out += "<option value=\""+htmlencode(sub_options[j].first)+"\"";
				for( size_t k = 0; k<value.size(); k++)	{
					if( sub_options[j].first == value[k] ) out += " selected";
				}
				out += ">"+htmlencode(sub_options[j].second)+"</option>";
			}
			out += "</optgroup>";
		}
		else
		{
			out += "<option value=\""+htmlencode(options[i].first)+"\"";
			for( size_t j = 0; j<value.size(); j++)	{
				if( options[i].first == value[j] ) out += " selected";
			}
			out += ">"+htmlencode(options[i].second)+"</option>";
		}
	}
	
	out += "</select>";

	if( !(*item)["#description"].empty() ) out +=	"<div class=\"description\">"+(*item)["#description"]+"</div>";
	
	return out;
}

string form_weight( map <string, string> *item )
{
	vector <pair <string, string> > options;

	for(int i=-10;i<=10;i++) {
		options.insert( options.end(), make_pair(str(i),str(i)) );
	}

	if( (*item)["#value"].empty() ) (*item)["#value"] = "0"; // Set Default

	(*item)["#options"] = serialize_array( options );
    (*item)["#multiple"] = _FALSE;

	return form_select( item );
}

string form_checkboxes( map <string, string> *item )
{
	vector <pair <string, string> > options;
	string out;
	string classes = "form-select";
	vector <string> value;
	vector <string> disabled;

	unnest( item, "#value", value);
	unnest( item, "#disabled", disabled);
		
	if( (*item)["#required"] == _TRUE ) classes += " required";
	if( (*item)["#error"] == _TRUE ) classes += " error";

	out += form_label( item );

	unserialize_array( (*item)["#options"], options );

//	out += "<select name=\""+(*item)["#name"]+"\"";

	out += "<div class=\"form-checkboxes\">\n";

	for( size_t i = 0; i < options.size(); i++ )
	{
		out += "<div class=\"form-item\"><label class=\"option\"><input type=\"checkbox\" name=\""+(*item)["#name"]+"\" value=\""+htmlencode(options[i].first)+"\"";
		for( size_t j = 0; j<value.size(); j++)	{
			if( options[i].first == value[j] ) out += " checked=\"checked\"";
		}
		for( size_t j = 0; j<disabled.size(); j++)	{
			if( options[i].first == disabled[j] ) out += " disabled=\"disabled\"";
		}
		out += "/>"+htmlencode(options[i].second)+"</label></div>\n";
	}

	out += "</div>\n";
	
	return out;
}

string form_radio( map <string, string> *item )
{
	vector <string> options;
	string out;
	string classes = "form-radio";
	string value = (*item)["#value"];
	string visible_value;

	if( (*item)["#required"] == _TRUE ) classes += " required";
	if( (*item)["#error"] == _TRUE ) classes += " error";

	if( isset( (*item)["#options"] ) )
	{	
		explode(options, (*item)["#options"], ',');
		visible_value = options[ intval( value ) ];
	}
	
	out += "<div class=\"form-item\">\n";
	out += "<label class=\"option\"><input type=\"radio\" name=\""+(*item)["#name"]+"\"";
	out += " value=\""+ value +"\"";

	if( (*item)["#checked"] == _TRUE ) {
		out += " checked=\"checked\"";
	}

	if( !classes.empty() ) out += " class=\""+classes+"\"";
	out += ">"+visible_value;
	out += "</label></div>\n";

	return out;
}

string form_radios( map <string, string> *item)
{
	vector <string> options;
	string out;
	string classes = "form-radio";
		
	if( (*item)["#required"] == _TRUE ) classes += " required";
	if( (*item)["#error"] == _TRUE ) classes += " error";

	out += form_label( item );

	explode(options, (*item)["#options"], ',');

	for(size_t i = 0; i< options.size(); i++)
	{
		out += "<div class=\"form-item\">\n";
		out += "<label class=\"option\"><input type=\"radio\" name=\""+(*item)["#name"]+"\"";
		out += " value=\""+ str(i) +"\"";

		if( !(*item)["#value"].empty() ) {
			if( (*item)["#value"] == str(i) ) out += " checked=\"checked\"";
		}

		if( !classes.empty() ) out += " class=\""+classes+"\"";
		out += ">"+options[i];
		out += "</label></div>\n";
	}	
	
	if( !(*item)["#description"].empty() ) out +=	"<div class=\"description\">"+(*item)["#description"]+"</div>";

	return out;
}

string form_textfield( map <string, string> *item)
{
	string out;
	string classes = isset2( (*item), "#class" ) ? (*item)["#class"] : "form-text";

	if( isset( (*item)["#autocomplete_path"] ) ) {
		add_js("etc/autocomplete.js");
		classes += " form-autocomplete";
		out += "<input class=\"autocomplete\" type=\"hidden\" id=\""+(*item)["#name"]+"-autocomplete\" value=\""+url((*item)["#autocomplete_path"])+"\" disabled=\"disabled\" />";
	}

	if( (*item)["#required"] == _TRUE ) classes += " required";
	if( (*item)["#error"] == _TRUE ) classes += " error";

	if( (*item)["#maxlength"].empty() ) (*item)["#maxlength"] = "255";
	if( (*item)["#size"].empty() ) (*item)["#size"] = "60";

	out += form_label( item );
	
	out += "<input type=\"text\" id=\""+(*item)["#name"]+"\" name=\""+(*item)["#name"]+"\"";
	if( !(*item)["#attributes"].empty() ) out += " "+(*item)["#attributes"];
	if( !(*item)["#disabled"].empty() ) out += " disabled";
	if( !(*item)["#maxlength"].empty() ) out += " maxlength=\""+(*item)["#maxlength"]+"\"";
	if( !(*item)["#size"].empty() ) out += " size=\""+(*item)["#size"]+"\"";
	if( !(*item)["#value"].empty() ) out += " value=\""+htmlencode( (*item)["#value"] )+"\"";
	if( !classes.empty() ) out += " class=\""+classes+"\"";
	out += "/>\n";
	
	if( !(*item)["#description"].empty() ) out +=	"<div class=\"description\">"+(*item)["#description"]+"</div>";

	return out;
}

string form_hidden( map <string, string> *item)
{
	string out;
	
	out += "<input type=\"hidden\" name=\""+(*item)["#name"]+"\"";
	if( !(*item)["#value"].empty() ) out += " value=\""+(*item)["#value"]+"\"";
	out += "/>\n";
	
	return out;
}

string form_password( map <string, string> *item)
{
	string out;
	string classes = isset2( (*item), "#class" ) ? (*item)["#class"] : "form-text";
		
	if( (*item)["#required"] == _TRUE ) classes += " required";
	if( (*item)["#error"] == _TRUE ) classes += " error";

	out += form_label( item );
	
	out += "<input type=\"password\" name=\""+(*item)["#name"]+"\"";
	if( !(*item)["#attributes"].empty() ) out += " "+(*item)["#attributes"];
	if( !(*item)["#size"].empty() ) out += " size=\""+(*item)["#size"]+"\"";
	if( !(*item)["#value"].empty() ) out += " value=\""+(*item)["#value"]+"\"";
	if( !classes.empty() ) out += " class=\""+classes+"\"";
	out += "/>\n";
	
	if( !(*item)["#description"].empty() ) out +=	"<div class=\"description\">"+(*item)["#description"]+"</div>";

	return out;
}

string form_password_confirm( map <string, string> *item)
{
	string out;
	string classes = "form-text";
		
	if( (*item)["#required"] == _TRUE ) classes += " required";
	if( (*item)["#error"] == _TRUE ) classes += " error";

	out += form_label( item );
	
	out += "<div class=\"container-inline\">\n";
	
	out += "<div class=\"form-item\">\n";
	out += "<input type=\"password\" name=\""+(*item)["#name"]+"1\"";
	if( !(*item)["#size"].empty() ) out += " size=\""+(*item)["#size"]+"\"";
	if( !(*item)["#value"].empty() ) out += " value=\""+(*item)["#value"]+"\"";
	if( !classes.empty() ) out += " class=\""+classes+"\"";
	out += "/>\n";
	out += "</div>\n";

	out += "<div class=\"form-item\">\n";
	out += "<input type=\"password\" name=\""+(*item)["#name"]+"2\"";
	if( !(*item)["#size"].empty() ) out += " size=\""+(*item)["#size"]+"\"";
	if( !(*item)["#value"].empty() ) out += " value=\""+(*item)["#value"]+"\"";
	if( !classes.empty() ) out += " class=\""+classes+"\"";
	out += "/>\n";
	out += "</div>\n";
	
	out += "</div>\n";
	
	if( !(*item)["#description"].empty() ) out +=	"<div class=\"description\">"+(*item)["#description"]+"</div>";

	return out;
}

string form_textarea( map <string, string> *item)
{
	string out;
	string classes = "form-textarea";

	if( (*item)["#required"] == _TRUE ) classes += " required";
	if( (*item)["#resizable"] == _TRUE ) classes += " resizable";
	if( (*item)["#error"] == _TRUE ) classes += " error";
	if( (*item)["#cols"].empty() ) (*item)["#cols"] = "60";
	if( (*item)["#rows"].empty() ) (*item)["#rows"] = "5";

	out += form_label( item );

	add_js("etc/textarea.js");

	out += "<textarea name=\""+(*item)["#name"]+"\"";
	if( !(*item)["#cols"].empty() ) out += " cols=\""+(*item)["#cols"]+"\"";
	if( !(*item)["#rows"].empty() ) out += " rows=\""+(*item)["#rows"]+"\"";
	if( !(*item)["#attributes"].empty() ) out += " "+(*item)["#attributes"];
	if( !classes.empty() ) out += " class=\""+classes+"\"";
	out += ">\n";
	if( !(*item)["#value"].empty() ) out += (*item)["#value"];
	out += "</textarea>\n";
	
	if( !(*item)["#description"].empty() ) out +=	"<div class=\"description\">"+(*item)["#description"]+"</div>";
	
	return out;
}

string form_button( map <string, string> *item)
{
	string out;
	string classes = "form-button";

	out += "<input type=\"button\" name=\""+(*item)["#name"]+"\" id=\""+(*item)["#name"]+"\"";
	if( !(*item)["#value"].empty() ) out += " value=\""+(*item)["#value"]+"\"";
	if( !(*item)["#attributes"].empty() ) out += " "+(*item)["#attributes"];
	if( !classes.empty() ) out += " class=\""+classes+"\"";
	out += "/>\n";

	return out;
}

string form_submit( map <string, string> *item)
{
	string out;
	string classes = "form-submit";

	out += "<input type=\"submit\" name=\""+(*item)["#name"]+"\" id=\"edit-"+(*item)["#name"]+"\"";
	if( !(*item)["#value"].empty() ) out += " value=\""+(*item)["#value"]+"\"";
	if( !(*item)["#attributes"].empty() ) out += " "+(*item)["#attributes"];
	if( !classes.empty() ) out += " class=\""+classes+"\"";
	out += "/>\n";

	return out;
}

string form_markup( map <string, string> *item)
{
	string out;

	out += (*item)["#value"];

	return out;
}

string form_fieldset( map <string, string> *item, map<string, map<string,string> > &form)
{
	string	attributes;
	string	out;
	map <string, map<string,string> >::iterator curr, end;
	
	list < pair<int, string> > order;
	list < pair<int, string> >::iterator curr2, end2;
	int pos;

	attributes = "";
	if( (*item)["#collapsible"]== _TRUE ) attributes += " collapsible";
	if( (*item)["#collapsed"]== _TRUE ) attributes += " collapsed";
	if( !attributes.empty() ) attributes = " class=\""+attributes+"\"";

	add_js("etc/collapse.js");
	   
	out = "<fieldset"+attributes+">\n";

	if( !(*item)["#title"].empty() ) out += "<legend>"+(*item)["#title"]+"</legend>\n";
	if( !(*item)["#description"].empty() ) out += "<div>"+(*item)["#description"]+"</div>\n";

	for( curr = form.begin(), end = form.end();  curr != end;  curr++ )
	{
		if(!form[curr->first]["#weight"].empty())
			pos = atoi( form[curr->first]["#weight"].c_str() );
		else
			pos = 0;

		if(form[curr->first]["#fieldset"] == (*item)["#name"])
		{
			order.push_back( make_pair( pos, curr->first ) );
		}
	}

	order.sort();

	if( !(*item)["#wrapper_prefix"].empty() ) out += (*item)["#wrapper_prefix"];

	if( isset( (*item)["#theme"] ) ) // If the fieldset has custom theme lets call it
	{
		cur_order = order;
		cur_form = form;
		out += 	_CALLBACKS[ (*item)["#theme"] ]();
	}
	else
	{
		out += form_items( order, form );
	}

	if( !(*item)["#wrapper_suffix"].empty() ) out += (*item)["#wrapper_suffix"];
	
	out += "</fieldset>\n";

	return out;
}

string form_table( map <string, string> *item, map<string, map<string,string> > &form)
{
	vector <vector <map <string, string> > > rows;
	vector <map <string, string> > row;
	map <string, string> cell;
	vector <string> header;
	vector <string> row_items;

	size_t i,j;

	explode( header, (*item)["#header"], ',' );
	i = 0;
	while( 1>0 )
	{
		string rowname = "#row"+str( i );
		if( !isset( (*item)[rowname] ) ) break;
		i++;

		row_items.clear();
		explode( row_items, (*item)[rowname], ',' );

		for(j=0; j<row_items.size(); j++)
		{	
			if( isset( form[ row_items[j] ]["#merge"] ) )
			{
				cell["data"] += form_render( row_items[j], form );
			}
			else
			{
				cell["data"] += form_render( row_items[j], form );
				cell["attributes"] = form[ row_items[j] ]["#attributes"];
				poke( row, cell );
			}
		}
		poke( rows, row );
	}

	return theme_table( header, rows, (*item)["#attributes"] );
}

void form_alter( string form_name, map<string, map<string,string> > &form )
{
	map<string, map<string,string> > backup = cur_form;

	// Find the _form_alter Hooks
	size_t cut;
	map <string, void (*)(void)>::iterator hook, hook_end;
	for( hook = _HOOKS.begin(), hook_end = _HOOKS.end();  hook != hook_end;  hook++ )
	{	
		if( (cut = hook->first.find( "_form_alter" )) != string::npos && (cut == hook->first.size() - 11) )
		{
			cur_form_id = form_name;
			cur_form = form;
			hook->second(); // call _form_alter
			form = cur_form;
		}
	}

	cur_form = backup;
}

string get_form(string form_name, map<string, map<string,string> > &form, string action)
{
	string	out;
	map <string, map<string,string> >::iterator curr, end;

	list < pair<int, string> > order;
	list < pair<int, string> >::iterator curr2, end2;
	int pos;

	form_alter( form_name, form );

	if( load_form_only )
	{	
		cur_form = form;
		return "";
	}
	
	if( validate_form )
	{
		for( curr = form.begin(), end = form.end();  curr != end;  curr++ )
		{
			if( !cur_form[curr->first]["#error"].empty() )
				form[curr->first]["#error"] = cur_form[curr->first]["#error"];
			
			form[curr->first]["#value"] = cur_form[curr->first]["#value"];

			size_t cut, len;
			if( (cut = form[curr->first]["#value"].find("#array[")) != string::npos && cut == 0 ) // is an array
			{	
				len = intval( form[curr->first]["#value"].substr(7) );
				for(int i=0; i<(int) len; i++) {
					form[curr->first]["#value["+str(i)+"]"] = cur_form[curr->first]["#value["+str(i)+"]"];
				}
			}

			if( !cur_form[curr->first]["#checked"].empty() )
				form[curr->first]["#checked"] = cur_form[curr->first]["#checked"];
		}
	}
	// action=\"\"

	if( isset( action ) )
		out = "<form name=\""+form_name+"\" method=\"post\" action=\""+action+"\" accept-charset=\"UTF-8\" enctype=\"multipart/form-data\">\n";
	else
		out = "<form name=\""+form_name+"\" method=\"post\" accept-charset=\"UTF-8\" enctype=\"multipart/form-data\">\n";

	//out+= "<div class=\"node-form\">\n";
	//out+= "<div class=\"standard\">\n";


	for( curr = form.begin(), end = form.end();  curr != end;  curr++ )
	{
		if(!form[curr->first]["#weight"].empty())
			pos = atoi( form[curr->first]["#weight"].c_str() );
		else
			pos = 0;

		if(form[curr->first]["#fieldset"].empty() && form[curr->first]["#table"].empty())
			order.push_back( make_pair( pos, curr->first ) );
	}

	order.sort();

	out += form_items( order, form );

	//out += "</div>\n";
	//out += "</div>\n";

	out += "</form>\n";

	return out;
}

void form_prepare(string form_name, map<string, map<string,string> > &form )
{
	map <string, map<string,string> >::iterator curr, end;
	
	list < pair<int, string> > order;
	list < pair<int, string> >::iterator curr2, end2;

	form_alter( form_name, form );

	if( load_form_only )
	{	
		cur_form = form;
		return;
	}
	
	if( validate_form )
	{
		for( curr = form.begin(), end = form.end();  curr != end;  curr++ )
		{
			if( !cur_form[curr->first]["#error"].empty() )
				form[curr->first]["#error"] = cur_form[curr->first]["#error"];
			
			form[curr->first]["#value"] = cur_form[curr->first]["#value"];

			size_t cut, len;
			if( (cut = form[curr->first]["#value"].find("#array[")) != string::npos && cut == 0 ) // is an array
			{	
				len = intval( form[curr->first]["#value"].substr(7) );
				for(int i=0; i<(int) len; i++) {
					form[curr->first]["#value["+str(i)+"]"] = cur_form[curr->first]["#value["+str(i)+"]"];
				}
			}

			if( !cur_form[curr->first]["#checked"].empty() )
				form[curr->first]["#checked"] = cur_form[curr->first]["#checked"];
		}
	}
	
	return;
}

string form_custom_elements( string type, map <string, string> *item )
{
	map <string, void (*)(void)>::iterator curr3, end3;

	for( curr3 = _HOOKS.begin(), end3 = _HOOKS.end();  curr3 != end3;  curr3++ )
	{
		if( curr3->first.find( "_elements" ) == curr3->first.size() - 9 )
		{
			_ELEMENTS.clear();

			curr3->second(); // Call elements function initializing _ELEMENTS

			map <string, map<string,string> >::iterator curr, end;
			map <string, string>::iterator curr2, end2;

			for( curr = _ELEMENTS.begin(), end = _ELEMENTS.end();  curr != end;  curr++ )
			{
				if( curr->first == type )
				{
					for( curr2 = curr->second.begin(), end2 = curr->second.end();  curr2 != end2;  curr2++ )
					{			
						(*item)[curr2->first] = curr2->second;
					}

					if( isset( (*item)["#process"] ) )
					{
						curr->second = *item;
						
						_CALLBACKS[ (*item)["#process"] ]();

						*item = curr->second;
/*
						for( curr2 = curr->second.begin(), end2 = curr->second.end();  curr2 != end2;  curr2++ )
						{			
							(*item)[curr2->first] = curr2->second;
						}*/
					}
				}
			}

		}
	}

	return "";
}

string form_items( list < pair<int, string> > &order, map<string, map<string,string> > &form )
{
	list < pair<int, string> >::iterator curr2, end2;
	string out;

	for( curr2 = order.begin(), end2 = order.end();  curr2 != end2;  curr2++ )
	{
		out += form_render( curr2->second, form );
	}

	return out;
}

string form_render(string item,  map<string, map<string,string> > &form)
{
	string out;

	if( isset( form[item]["#type"] ) ) 
	{		
		if( form[item]["#type"] == "submit" )
		{
			if( !isset(form[item]["#name"]) ) form[item]["#name"] = "op";
		}
		else
		{
			if( !isset(form[item]["#name"]) ) form[item]["#name"] = item;
		}

			form_custom_elements( form[item]["#type"], &form[item] );

			if( isset ( form[item]["#prefix"] ) ) out += form[item]["#prefix"];

			if( form[item]["#type"] != "submit" && form[item]["#type"] != "button" && form[item]["#type"] != "markup" && form[item]["#type"] != "hidden" && form[item]["#type"] != "radios") out += "<div class=\"form-item\">\n";

			if( form[item]["#type"] == "fieldset") out += form_fieldset( &form[item], form );
			if( form[item]["#type"] == "hidden") out += form_hidden( &form[item] );
			if( form[item]["#type"] == "table") out += form_table( &form[item], form );
			if( form[item]["#type"] == "file") out += form_file( &form[item] );
			if( form[item]["#type"] == "textfield") out += form_textfield( &form[item] );
			if( form[item]["#type"] == "password") out += form_password( &form[item] );
			if( form[item]["#type"] == "password_confirm") out += form_password_confirm( &form[item] );
			if( form[item]["#type"] == "textarea") out += form_textarea( &form[item] );
			if( form[item]["#type"] == "radio") out += form_radio( &form[item] );
			if( form[item]["#type"] == "checkbox") out += form_checkbox( &form[item] );
			if( form[item]["#type"] == "checkboxes") out += form_checkboxes( &form[item] );
			if( form[item]["#type"] == "select") out += form_select( &form[item] );
			if( form[item]["#type"] == "weight") out += form_weight( &form[item] );
			if( form[item]["#type"] == "radios") out += form_radios( &form[item] );
			if( form[item]["#type"] == "markup") out += form_markup( &form[item] );
			if( form[item]["#type"] == "button") out += form_button( &form[item] );
			if( form[item]["#type"] == "submit") out += form_submit( &form[item] );

			if( form[item]["#type"] != "submit" && form[item]["#type"] != "button" && form[item]["#type"] != "markup" && form[item]["#type"] != "hidden" && form[item]["#type"] != "radios") out += "</div>\n";

			if( isset ( form[item]["#suffix"] ) ) out += form[item]["#suffix"];
	}

	return out;
}

void form_set_error(string field, string message)
{
	cur_form[field]["#error"] = _TRUE;

	set_page_message( message, "error" );
}

bool form_validate( map<string, map<string,string> > &form )
{
	map <string, map<string,string> >::iterator curr, end;
	string out;

	bool is_ok = true;

	for( curr = form.begin(), end = form.end();  curr != end;  curr++ )
	{
		if( inarray( form[curr->first], "#type") )
		{
			if( form[curr->first]["#type"] == "password_confirm" )
			{
				if( _POST[curr->first+"1"] != _POST[curr->first+"2"] )
				{
					form_set_error(curr->first, "Passwords don't match."); 
					is_ok = false;
				}
				else
				{
					form[curr->first]["#value"] = _POST[curr->first+"1"];
				}
			}
			else if( inarray( form[curr->first], "#required" ) && form[curr->first]["#required"] == _TRUE )
			{
				if(_POST[curr->first].empty())
				{
					if( form[curr->first]["#required_msg"].empty() )
						form_set_error(curr->first, form[curr->first]["#title"] + " field is required."); 
					else
						form_set_error(curr->first, form[curr->first]["#required_msg"] ); 
					is_ok = false;
				}
			}

			string variable;

			if( inarray( form[curr->first], "#name") ) variable = form[curr->first]["#name"];
			else variable = curr->first;

			if( inarray( _POST, variable) )
			{
				if(form[curr->first]["#type"] == "radio" || form[curr->first]["#type"] == "checkbox")
				{
					if( form[curr->first]["#value"] == _POST[ variable ] )
					{
						form[curr->first]["#checked"] = _TRUE;
					}
				}
				else
				{	
					vector <string> values;
					vector <string> disabled;

					unnest( &_POST, variable, values );
					unnest( &form[curr->first], "#disabled", disabled ); // If we have disabled values we have to add those values only for display
					for(size_t j=0; j<disabled.size(); j++)
						values.push_back( disabled[j] );
					nestclear( &form[curr->first], "#value" ); // Clear current values in the form
					nest( values, &form[curr->first], "#value" );
				}
			}
			else
			{
				// En el cas que la variable no estigui en el POST

				if(form[curr->first]["#type"] == "radio" || form[curr->first]["#type"] == "checkbox")
				{
					form[curr->first]["#checked"] = _FALSE;
				}

			}

		}
	}
	
	validate_form = true;

	return is_ok;
}
