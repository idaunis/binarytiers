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
#include "database.h"

void db_type_map( map<string, string> &map ) 
{
	map["varchar:normal"] = "VARCHAR";
	map["char:normal"] = "CHAR";

	map["text:tiny"] = "TINYTEXT";
	map["text:small"] = "TINYTEXT";
	map["text:medium"] = "MEDIUMTEXT";
	map["text:big"] = "LONGTEXT";
	map["text:normal"] = "TEXT";

	map["serial:tiny"] = "TINYINT";
	map["serial:small"] = "SMALLINT";
	map["serial:medium"] = "MEDIUMINT";
	map["serial:big"] = "BIGINT";
	map["serial:normal"] = "INT";

	map["int:tiny"] = "TINYINT";
	map["int:small"] = "SMALLINT";
	map["int:medium"] = "MEDIUMINT";
	map["int:big"] = "BIGINT";
	map["int:normal"] = "INT";

	map["float:tiny"] = "FLOAT";
	map["float:small"] = "FLOAT";
	map["float:medium"] = "FLOAT";
	map["float:big"] = "DOUBLE";
	map["float:normal"] = "FLOAT";

	map["numeric:normal"] = "DECIMAL";

	map["blob:big"] = "LONGBLOB";
	map["blob:normal"] = "BLOB";

	map["datetime:normal"] = "DATETIME";
}

void _db_process_field( map<string, string> &field )
{
	map<string, string>	map;

	if (!isset(field["size"])) {
		field["size"] = "normal";
	}

	// Set the correct database-engine specific datatype.
	if (!isset(field["mysql_type"])) {
		db_type_map( map );
		field["mysql_type"] = map[field["type"] + ":" + field["size"]];
	}

	if (field["type"] == "serial") {
		field["auto_increment"] = _TRUE;
	}
}

string _db_create_field_sql(string name, map<string, string> spec) 
{
	string	sql;

	_db_process_field( spec );
    
	sql = "`" + name + "` " + spec["mysql_type"];

	if ( (spec["type"] == "varchar" || spec["type"] == "char" || spec["type"] == "text") && isset(spec["length"]) ) {
		sql += "(" + spec["length"] + ")";
	}
	else if (isset(spec["precision"]) && isset(spec["scale"])) {
		sql += "(" + spec["precision"] + ", " + spec["scale"] + ")";
	}

	if (!spec["unsigned"].empty()) {
		sql += " unsigned";
	}

	if (spec["not null"] == _TRUE) {
		sql += " NOT NULL";
	}

	if (!spec["auto_increment"].empty()) {
		sql += " auto_increment";
	}

	if (inarray( spec, "default")) {
		if ( !is_numeric(spec["default"]) ) {
		  spec["default"] = "\"" + spec["default"] + "\"";
		}
		sql += " DEFAULT " + spec["default"];
	}

	if (spec["not null"].empty() && !isset(spec["default"])) {
		sql += " DEFAULT NULL";
	}

	return sql;
}

string _db_create_key_sql( vector <string> &fields ) 
{
	vector <string> ret;
	
	for(size_t i=0; i<fields.size(); i++)
	{
		vector <string> field;
		explode( field, fields[i], '|' );

		if (field.size()==2) {
			ret.push_back( field[0] + "(" + field[1] + ")" );
		}
		else {
			ret.push_back( field[0] );
		}
	}
	return implode(ret, ", ");
}

string _db_create_key_sql( map <string, string> &map_fields ) 
{
	vector <string> fields;
	for( map <string,string>::iterator i = map_fields.begin(), end = map_fields.end();  i != end;  i++ )
	{
		fields.push_back( i->second );
	}

	return( _db_create_key_sql( fields ) );
}

void _db_create_keys_sql(map <string, map<string, string> > &spec, vector <string> &keys) 
{  
	if (!spec["#primary key"].empty()) 
	{
		keys.push_back( "PRIMARY KEY (" + _db_create_key_sql(spec["#primary key"]) + ")" );
	}
	if (!spec["#unique keys"].empty()) 
	{
		for( map <string,string>::iterator i = spec["#unique keys"].begin(), end = spec["#unique keys"].end();  i != end;  i++ )
		{
			vector <string>	fields;
			explode( fields, i->second );
			keys.push_back( "UNIQUE KEY " + i->first + " (" + _db_create_key_sql( fields ) + ")" );
		}
	}

	if (!spec["#indexes"].empty()) 
	{
		for( map <string,string>::iterator i = spec["#indexes"].begin(), end = spec["#indexes"].end();  i != end;  i++ )
		{
			vector <string>	fields;
			explode( fields, i->second );
			keys.push_back( "INDEX " + i->first + " (" + _db_create_key_sql( fields ) + ")" );
		}
	}
}

string db_create_table_sql( map <string, map<string, string> > &table ) 
{
	string sql;

	string name = table["#spec"]["name"];

	if( table["#spec"]["mysql_suffix"].empty() )
	{
		table["#spec"]["mysql_suffix"] = "/*!40100 DEFAULT CHARACTER SET UTF8 */";
	}

	sql = "CREATE TABLE "+ name + " (\n";
	
	for( map <string, map<string,string> >::iterator i = table.begin(), end = table.end();  i != end;  i++ )
	{
		if(i->first.find_first_of('#') == string::npos)  //is Field
		{
			sql += _db_create_field_sql(i->first, table[i->first] ) + ", \n";
		}
	}

	// Process keys & indexes.
	vector <string> keys;
	_db_create_keys_sql(table, keys);
	if (keys.size()>0) 
	{
		sql += implode(keys, ", \n") + ", \n";
	}

	// Remove the last comma and space.
	sql = sql.substr(0, sql.length()-3) + "\n) ";

	sql += table["#spec"]["mysql_suffix"];

	return sql;
}
