/*
** Copyright (C) 2011 Uselabs and/or its subsidiary(-ies).
** All rights reserved. Unauthorized distribution prohibited.
*/

#include "example.h"

void example_hooks()
{
	_MAP_DESC( example, "Model Example" );
	_MAP_VER ( example, "1.0.1" );
	_MAP_HOOK( example, controllers );
}

void example_controllers()
{
	map <string, string> item;

	item["path"] = "testpage";
	item["title"] = "My Model Example";
	item["callback"] = _MAP( example_testpage );
	item["type"] = str( MENU_LOCAL_TASK );
	poke( controllers, item );
}

string example_testpage()
{
	string out = "\
	<p>This is an example</p>\n\
	";
	return out;
}