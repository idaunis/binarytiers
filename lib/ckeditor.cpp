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
#include "ckeditor.h"

void ckeditor_hooks()
{
	_MAP_DESC ( ckeditor, "The CKEditor Javascript HTML WYSIWYG editor." );
	_MAP_VER  ( ckeditor, "1.0.1" );

	_MAP_HOOK ( ckeditor, elements );
}

void ckeditor_elements()
{		
	_ELEMENTS["textarea"]["#process"] = _MAP( ckeditor_process_textarea );
}

string ckeditor_process_textarea()
{
	string name = _ELEMENTS["textarea"]["#name"];

	if( _ELEMENTS["textarea"]["#rows"].empty() ) _ELEMENTS["textarea"]["#rows"] = "5";

	int rows = intval( _ELEMENTS["textarea"]["#rows"] );
	if(rows > 0 && rows <= 5) return "";

	int pixels = rows * 32;
	if( _ELEMENTS["textarea"]["#ckdisable"] == _TRUE ) return "";
	if( !isset(_ELEMENTS["textarea"]["#ckheight"]) ) _ELEMENTS["textarea"]["#ckheight"] = str(pixels)+"px";;

	_ELEMENTS["textarea"]["#resizable"] = _FALSE;

	add_js( "etc/ckeditor/ckeditor.js" );

	// addLoadEvent( function() {\n\

	string js = "<script type=\"text/javascript\">\n\
	$(document).ready(function() {\n\
		CKEDITOR.replace( '"+name+"',\n\
		{\n\
		toolbar : [ [ 'Font','FontSize','Bold', 'Italic', 'Underline', 'Strike','-','Link','Print','SpellChecker', '-', 'NumberedList','BulletedList','-','Outdent','Indent','Blockquote','-','Source' ] ],\n\
			enterMode	: Number( 1 ),\n\
			shiftEnterMode	: Number( 2 ),\n\
			skin : 'v2',\n\
			height : '"+_ELEMENTS["textarea"]["#ckheight"]+"'\n\
		});\n\
	} );\n\
	</script>\n\
	";

	_ELEMENTS["textarea"]["#prefix"] = js;
	
	return "";
}
