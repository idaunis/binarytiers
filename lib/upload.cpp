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
#include "upload.h"

#include "gd/gd.h"

string form_items( list < pair<int, string> > &order, map<string, map<string,string> > &form );

void upload_hooks()
{
	_MAP_DESC ( upload, "Allows users to upload and attach files to content." );
	_MAP_VER  ( upload, "1.3.1" );
	_MAP_PERM ( upload, "upload files, view uploaded files" );
	_MAP_HOOK ( upload, schema );
	_MAP_HOOK ( upload, controllers );
	_MAP_HOOK ( upload, update );
	_MAP_HOOK ( upload, insert );
	_MAP_HOOK ( upload, load );
	_MAP_HOOK ( upload, delete );
	_MAP_HOOK ( upload, form_alter );
}

void upload_schema()
{
	map <string, map<string, string> > upload;

	upload["#spec"]["name"] = "upload";
	upload["#spec"]["description"] = "Stores uploaded file information and table associations.";
    upload["fid"]["type"] = "int";
    upload["fid"]["unsigned"] = _TRUE;
    upload["fid"]["not null"] = _TRUE;
    upload["fid"]["default"] = "0";
    upload["fid"]["description"] = "Primary Key: The files.fid.";
	upload["nid"]["type"] = "int";
    upload["nid"]["unsigned"] = _TRUE;
    upload["nid"]["not null"] = _TRUE;
    upload["nid"]["default"] = "0";
    upload["nid"]["description"] = "The {node}.nid associated with the uploaded file.";
    upload["vid"]["type"] = "int";
    upload["vid"]["unsigned"] = _TRUE;
    upload["vid"]["not null"] = _TRUE;
    upload["vid"]["default"] = "0";
    upload["vid"]["description"] = "Primary Key: The {node}.vid associated with the uploaded file.";
	upload["description"]["type"] = "varchar";
    upload["description"]["length"] = "255";
    upload["description"]["not null"] = _TRUE;
    upload["description"]["default"] = "";
    upload["description"]["description"] = "Description of the uploaded file.";
    upload["list"]["type"] = "int";
    upload["list"]["unsigned"] = _TRUE;
    upload["list"]["not null"] = _TRUE;
    upload["list"]["default"] = "0";
    upload["list"]["size"] = "tiny";
    upload["list"]["description"] = "Whether the file should be visibly listed on the node: yes(1) or no(0).";
	upload["weight"]["type"] = "int";
    upload["weight"]["not null"] = _TRUE;
    upload["weight"]["default"] = "0";
    upload["weight"]["size"] = "tiny";
    upload["weight"]["description"] = "Weight of this upload in relation to other uploads in this node.";
	upload["#primary key"]["vid"] = "vid";
	upload["#primary key"]["fid"] = "fid";
    upload["#indexes"]["fid"] = "fid";
    upload["#indexes"]["nid"] = "nid";
	schema.push_back( upload );
}

void upload_controllers()
{
	map <string, string> item;

	item["path"] = "upload/js";
	item["callback"] = _MAP( upload_js );
	item["type"] = str( MENU_CALLBACK );
	item["access arguments"] = "upload files";
	poke( controllers, item );
}

string bt_to_json( string str )
{
	string	out;
	str = addslashes( str );

	for (int i=0;i<(int) strlen(str.c_str());i++)
	{
		if(str[i]=='<') out += "\\x3c";
		else if(str[i]=='>') out += "\\x3e";
		else if(str[i]=='&') out += "\\x26";
		else if(str[i]=='\n') out += "\\n";
		else if(str[i]=='\r') out += "\\r";
		else out += str[i];
	}
	return out;
}

string bt_to_json( map <string, string> item )
{
	string out = "{";
	for( map <string, string>::iterator curr = item.begin(), end = item.end();  curr != end;  curr++ )
	{
		if(curr!=item.begin()) out += ",";
		out += "\""+bt_to_json( curr->first )+"\"";
		out += ":";
		out += "\""+bt_to_json( curr->second )+"\"";
	}
	out += "}";

	return out;
}

string bt_render( map <string, map<string,string> > &form )
{
	string	out;
	map <string, map<string,string> >::iterator curr, end;

	list < pair<int, string> > order;
	list < pair<int, string> >::iterator curr2, end2;

	int pos;

	for( curr = form.begin(), end = form.end();  curr != end;  curr++ )
	{
		if(!form[curr->first]["#weight"].empty())
			pos = atoi( form[curr->first]["#weight"].c_str() );
		else
			pos = 0;

		if(form[curr->first]["#table"].empty())
			order.push_back( make_pair( pos, curr->first ) );
	}

	order.sort();
	
	out += form_items( order, form );

	return out;
}

void _upload_form( map <string, map<string,string> > &form )
{
	form["upload"]["#type"] = "file";
	form["upload"]["#title"] = "Attach new file";
	form["upload"]["#fieldset"] = "attachments";
	form["upload"]["#description"] = "The maximum upload size is 1 MB. Only files with the following extensions may be uploaded: jpg jpeg gif png txt doc xls pdf ppt pps odt ods odp.";
	form["upload"]["#weight"] = "3";

	form["attach"]["#type"] = "submit";
	form["attach"]["#value"] = "Attach";
	form["attach"]["#name"] = "attach";
	form["attach"]["#fieldset"] = "attachments";
	form["attach"]["#weight"] = "4";
}

string basename( string filename )
{
	size_t cut;

	if( (cut = filename.find_first_of("\\/")) != filename.npos )
		return filename.substr(cut+1);
	else
		return filename;
}

string dirname( string filename )
{
	size_t cut;

	if( (cut = filename.find_first_of("\\/")) != filename.npos )
		return filename.substr(0, cut);
	else
		return filename;
}

bool file_set_status(map <string, string> &file, int status)
{
	if(DB_TYPE==1)
	{
		if( redis_int("HEXISTS files:%d status", intval(file["fid"]) ) )
		{
			redis_command("HSET files:%d status %d", intval(file["fid"]), status);
			return true;
		}
	}
	if(DB_TYPE==2)
	{
		if( db_querya("UPDATE files SET status = %d WHERE fid = %d", str(status).c_str(), file["fid"].c_str() ) ) 
		{
			file["status"] = str(status);
			return true;
		}
	}
	return false;
}

string file_create_filename( string base, string directory )
{
	string	dest = directory + "/" + base;
	string	ext = "";
	string	name;

	if ( file_exists(dest) ) 
	{
		size_t pos;
		if( (pos = base.find_last_of('.')) != base.npos ) {
			name = base.substr(0, pos);
			ext = base.substr(pos);
		} else {
			name = base;
		}

		int counter = 0;
		do {			
			dest = directory + "/" + name + "_" + str( counter ) + ext;
			counter ++;
		} while( file_exists(dest) );
	}
	return dest;
}

string file_destination(string destination, int replace) 
{	  
	string name, directory;
	if ( file_exists( destination ) )
	{
		switch (replace) 
		{
			case FILE_EXISTS_RENAME:
				name = basename( destination );
				directory = dirname( destination );
				destination = file_create_filename( name, directory );
				break;

			case FILE_EXISTS_ERROR:
				set_page_message("The selected file "+destination+" could not be copied, because a file by that name already exists in the destination.", "error");
				return "";
		}
	}

	return destination;
}

bool move_uploaded_file(string oldname, string newname)
{
	if( rename( oldname.c_str(), newname.c_str() ) == 0 )
		return true;
	else
		return false;

	/*
	char* buffer;
	if( (buffer = _getcwd( NULL, 0 )) != NULL )
	{
		file["destination"] = string( buffer );
		free(buffer);
	}
	FILE *fp = fopen(file["destination"].c_str(), "wb");
	if(fp)
	{
		fprintf(fp, "asd");
		fclose(fp);
	}
	*/
}

string file_create_url(string path)
{
	return BASE_URL + string("/") + path;
}

void _upload_correct_size( string file, int &file_w, int &file_h )
{	
	gdImagePtr im;
	FILE *fp;

	fp = fopen(file.c_str(), "rb");
	if (!fp) {
		print("cannot open\n");
		return;
	}

	size_t pos = file.find_last_of( "." );
	if( pos == string::npos) return;

	string ext = strtolower( file.substr( pos ) );
	if( ext == string(".jpg") || ext == string(".jpeg") )
		im = gdImageCreateFromJpeg( fp );
	else if( ext == string(".gif") )
		im = gdImageCreateFromGif( fp );
	else if( ext == string(".png") )
		im = gdImageCreateFromPng( fp );
	else
	{
		fclose(fp);
		return;
	}

	fclose(fp);

	file_w = gdImageSX( im );
	file_h = gdImageSY( im );

	if( file_w > 640 || file_h > 1200 )
	{
		int dst_w, dst_h;

		if( file_w > file_h )
		{
			dst_w = 640;
			dst_h = (int) ( dst_w * (float) file_h / (float) file_w );
		}
		else
		{
			dst_h = 1200;
			dst_w = (int) ( dst_h * (float) file_w / (float) file_h );
		}

		file_w = dst_w;
		file_h = dst_h;

		gdImagePtr dst_im = gdImageCreateTrueColor( dst_w, dst_h);
		gdImageAlphaBlending( dst_im, 0 );
		gdImageCopyResampled ( dst_im, im, 0, 0, 0, 0, dst_im->sx, dst_im->sy, im->sx, im->sy );
		//gdImageCopyResized( dst_im, im, 0, 0, 0, 0, dst_w, dst_h, gdImageSX( im ), gdImageSY( im ) );

		fp = fopen(file.c_str(), "wb");
		if( ext == string(".jpg") || ext == string(".jpeg") )
			gdImageJpeg(dst_im, fp, 90);
		else if( ext == string(".gif") )
			gdImageGif(dst_im, fp);
		else if( ext == string(".png") )
			gdImagePng(dst_im, fp);

		fclose(fp);
		gdImageDestroy(dst_im);
	}

	gdImageDestroy(im);
}

bool file_save_upload(map <string, string> &file, string source, string validators, string dest, int replace)
{
	if( isset( _FILES[source]["filename"] ) )
	{
		string ext;
		size_t pos;
		if( (pos = _FILES[source]["filename"].find_last_of('.')) != string::npos ) {
			ext = _FILES[source]["filename"].substr(pos);
		}

		file["filename"] = user["uid"] + "_" + microtime() + ext;
		file["filemime"] = _FILES[source]["type"];
		file["filesize"] = _FILES[source]["size"];
		file["destination"] = file_destination( dest + "/" + file["filename"], replace );
		file["filepath"] = file["destination"];

		trim( file["filemime"] );

		// Move uploaded files from BT upload_tmp_dir to the CMS temporary directory.
		if( !move_uploaded_file( _FILES[source]["tempname"], file["filepath"] ) )
		{
			return false;
		}

		int w = 0, h = 0;

		_upload_correct_size( file["filepath"], w, h );
		file["width"] = str( w );
		file["height"] = str( h );
		file["filesize"] = str( filesize( file["filepath"] ) );

		// If we made it this far it's safe to record this file in the database.
		file["uid"] = user["uid"];
		file["status"] = str( FILE_STATUS_TEMPORARY );
		file["timestamp"] = str( time() );

		if(DB_TYPE==1)
		{
			file["fid"] = redis_str("INCR files:ids");
			redis_command("SADD files %d", intval(file["fid"]) );
			redis_command_fields(redis_arg("HMSET files:%d", intval(file["fid"]) ), "",
				"uid,filename,filepath,filemime,filesize,width,height,status,timestamp", "%d,%s,%s,%s,%s,%d,%d,%d,%d",
				intval( file["uid"] ),
				file["filename"].c_str(),
				file["filepath"].c_str(),
				file["filemime"].c_str(),
				file["filesize"].c_str(),
				intval( file["width"] ),
				intval( file["height"] ),
				intval( file["status"] ),
				intval( file["timestamp"] ) );
		}
		if(DB_TYPE==2)
		{
			db_querya("INSERT INTO files (uid, filename, filepath, filemime, filesize, width, height, status, timestamp) VALUES(%d, '%s', '%s', '%s', %d, %d, %d, %d, %d)",
				file["uid"].c_str(),
				file["filename"].c_str(),
				file["filepath"].c_str(),
				file["filemime"].c_str(),
				file["filesize"].c_str(),
				file["width"].c_str(),
				file["height"].c_str(),
				file["status"].c_str(),
				file["timestamp"].c_str() );
			file["fid"] = db_last_insert_id();
		}

		return true;
	}
	return false;
}

string upload_js()
{
	//string out = "{ \"status\": true, \"data\": \"&lt;div class=\"messages error\"&gt;\nValidation error, please try again. If this error persists, please contact the site administrator.&lt;/div&gt;\n\" }";
	string out = "{ \"status\": true, \"data\": \"\" }";
	string validators = "";

	map <string, string> js;
	map <string, map<string,string> > form;
	map <string, string> file;
		
	_upload_form( form );

	form["upload-attachments"]["#type"] = "table";
	form["upload-attachments"]["#header"] = "Delete,List,Description,Size";
	form["upload-attachments"]["#weight"] = "2";
	form["upload-attachments"]["#fieldset"] = "attachments";

	size_t	total = intval(_POST["upload-total"]);
	size_t	i = 0;
	string	description;

	while( i<total )
	{
		description = file_create_url( _POST["upload-data2"+str(i)] );

		form["upload-delete"+str(i)]["#type"] = "checkbox";
		form["upload-delete"+str(i)]["#table"] = "upload-attachments";
		form["upload-delete"+str(i)]["#checked"] = _POST["upload-delete"+str(i)];
		form["upload-delete"+str(i)]["#value"] = "1";
		form["upload-list"+str(i)]["#type"] = "checkbox";
		form["upload-list"+str(i)]["#table"] = "upload-attachments";
		form["upload-list"+str(i)]["#checked"] = _POST["upload-list"+str(i)];
		form["upload-list"+str(i)]["#value"] = "1";
		form["upload-description"+str(i)]["#type"] = "textfield";
		form["upload-description"+str(i)]["#value"] = _POST["upload-description"+str(i)];
		form["upload-description"+str(i)]["#description"] = description;
		form["upload-description"+str(i)]["#table"] = "upload-attachments";
		
		form["upload-size"+str(i)]["#type"] = "markup";
		form["upload-size"+str(i)]["#table"] = "upload-attachments";
		form["upload-size"+str(i)]["#value"] = _POST["upload-data1"+str(i)];
		
		form["upload-weight"+str(i)]["#type"] = "hidden";
		form["upload-weight"+str(i)]["#table"] = "upload-attachments";
		form["upload-weight"+str(i)]["#value"] = _POST["upload-weight"+str(i)];
		form["upload-weight"+str(i)]["#merge"] = _TRUE;
		form["upload-data1"+str(i)]["#type"] = "hidden";
		form["upload-data1"+str(i)]["#table"] = "upload-attachments";
		form["upload-data1"+str(i)]["#value"] = _POST["upload-data1"+str(i)];
		form["upload-data1"+str(i)]["#merge"] = _TRUE;
		form["upload-data2"+str(i)]["#type"] = "hidden";
		form["upload-data2"+str(i)]["#table"] = "upload-attachments";
		form["upload-data2"+str(i)]["#value"] = _POST["upload-data2"+str(i)];
		form["upload-data2"+str(i)]["#merge"] = _TRUE;
		form["upload-data3"+str(i)]["#type"] = "hidden";
		form["upload-data3"+str(i)]["#table"] = "upload-attachments";
		form["upload-data3"+str(i)]["#value"] = _POST["upload-data3"+str(i)];
		form["upload-data3"+str(i)]["#merge"] = _TRUE;

		form["upload-attachments"]["#row"+str(i)] = "upload-delete"+str(i)+",upload-list"+str(i)+",upload-description"+str(i)+",upload-weight"+str(i)+",upload-data1"+str(i)+",upload-data2"+str(i)+",upload-data3"+str(i)+",upload-size"+str(i);

		i++;
	}

	if ( user_access("upload files") )
	{
		if( file_save_upload( file, "upload", validators, file_directory_path() ) )
		{
			description = file_create_url( file["destination"] );

			file["list"] = variable_get("upload_list_default", "1");
					
			form["upload-delete"+str(i)]["#type"] = "checkbox";
			form["upload-delete"+str(i)]["#table"] = "upload-attachments";
			form["upload-delete"+str(i)]["#value"] = "1";
			form["upload-list"+str(i)]["#type"] = "checkbox";
			form["upload-list"+str(i)]["#table"] = "upload-attachments";
			form["upload-list"+str(i)]["#checked"] = file["list"];
			form["upload-list"+str(i)]["#value"] = "1";
			form["upload-description"+str(i)]["#type"] = "textfield";
			form["upload-description"+str(i)]["#value"] = file["filename"];
			form["upload-description"+str(i)]["#description"] = description;
			form["upload-description"+str(i)]["#table"] = "upload-attachments";
			form["upload-size"+str(i)]["#type"] = "markup";
			form["upload-size"+str(i)]["#table"] = "upload-attachments";
			form["upload-size"+str(i)]["#value"] = format_size( file["filesize"] );
			
			form["upload-weight"+str(i)]["#type"] = "hidden";
			form["upload-weight"+str(i)]["#table"] = "upload-attachments";
			form["upload-weight"+str(i)]["#value"] = "1";
			form["upload-weight"+str(i)]["#merge"] = _TRUE;
			form["upload-data1"+str(i)]["#type"] = "hidden";
			form["upload-data1"+str(i)]["#table"] = "upload-attachments";
			form["upload-data1"+str(i)]["#value"] = format_size( file["filesize"] );
			form["upload-data1"+str(i)]["#merge"] = _TRUE;
			form["upload-data2"+str(i)]["#type"] = "hidden";
			form["upload-data2"+str(i)]["#table"] = "upload-attachments";
			form["upload-data2"+str(i)]["#value"] = file["destination"];
			form["upload-data2"+str(i)]["#merge"] = _TRUE;
			form["upload-data3"+str(i)]["#type"] = "hidden";
			form["upload-data3"+str(i)]["#table"] = "upload-attachments";
			form["upload-data3"+str(i)]["#value"] = file["fid"];
			form["upload-data3"+str(i)]["#merge"] = _TRUE;

			form["upload-attachments"]["#row"+str(i)] = "upload-delete"+str(i)+",upload-list"+str(i)+",upload-description"+str(i)+",upload-weight"+str(i)+",upload-data1"+str(i)+",upload-data2"+str(i)+",upload-data3"+str(i)+",upload-size"+str(i);
			total ++;
		}
	}

	form["upload-total"]["#type"] = "hidden";
	form["upload-total"]["#value"] = str(total);
	form["upload-total"]["#fieldset"] = "attachments";
	
	out = bt_render( form );
	
	js["status"] = "true";
	js["data"] = out;

	print ( bt_to_json( js ) );
	return "";
}

void upload_form_alter()
{
	if( cur_form_id == "add-page" ) {

		string js = "<script type=\"text/javascript\">\n\
<!--//--><![CDATA[//><!--\n\
	var ahah_settings = { \"edit-attach\": { \"url\": \"/upload/js\", \"event\": \"mousedown\", \"keypress\": true, \"wrapper\": \"attach-wrapper\", \"selector\": \"#edit-attach\", \"effect\": \"none\", \"method\": \"replace\", \"progress\": { \"type\": \"bar\", \"message\": \"Please wait...\" }, \"button\": { \"attach\": \"Attach\" } } };\n\
	//--><!]]>\n\
	</script>\n\
	";

		add_js("etc/ahah.js");
		add_js("etc/jquery.form.js");
		add_js("etc/progress.js");

		cur_form["attachments"]["#type"] = "fieldset";
		cur_form["attachments"]["#title"] = "File attachments";
		cur_form["attachments"]["#description"] = "Changes made to the attachments are not permanent until you save this post. The first \"listed\" file will be included in RSS feeds.";
		cur_form["attachments"]["#collapsible"] = _TRUE;
		cur_form["attachments"]["#collapsed"] = _FALSE;
		cur_form["attachments"]["#weight"] = "20";
		cur_form["attachments"]["#prefix"] = js+"<div class=\"attachments\">";
		cur_form["attachments"]["#suffix"] = "</div>";
		// Wrapper for fieldset contents (used by ahah.js).
		cur_form["attachments"]["#wrapper_prefix"] = "<div id=\"attach-wrapper\">";
		cur_form["attachments"]["#wrapper_suffix"] = "</div>";		

		_upload_form( cur_form );
		
		map <string, string> file;
		size_t	i = 0;
		string	description;

		for( i = 0; i<cur_files.size(); i++ )
		{
			file = cur_files[i];

			description = file_create_url( file["filepath"] );

			cur_form["upload-delete"+str(i)]["#type"] = "checkbox";
			cur_form["upload-delete"+str(i)]["#table"] = "upload-attachments";
			cur_form["upload-delete"+str(i)]["#value"] = "1";

			cur_form["upload-list"+str(i)]["#type"] = "checkbox";
			cur_form["upload-list"+str(i)]["#table"] = "upload-attachments";
			cur_form["upload-list"+str(i)]["#checked"] = file["list"];
			cur_form["upload-list"+str(i)]["#value"] = "1";
			
			cur_form["upload-description"+str(i)]["#type"] = "textfield";
			cur_form["upload-description"+str(i)]["#value"] = file["description"];
			cur_form["upload-description"+str(i)]["#description"] = description;
			cur_form["upload-description"+str(i)]["#table"] = "upload-attachments";
			
			cur_form["upload-size"+str(i)]["#type"] = "markup";
			cur_form["upload-size"+str(i)]["#table"] = "upload-attachments";
			cur_form["upload-size"+str(i)]["#value"] = format_size( file["filesize"] );
			
			cur_form["upload-weight"+str(i)]["#type"] = "hidden";
			cur_form["upload-weight"+str(i)]["#table"] = "upload-attachments";
			cur_form["upload-weight"+str(i)]["#value"] = file["weight"];
			cur_form["upload-weight"+str(i)]["#merge"] = _TRUE;

			cur_form["upload-data1"+str(i)]["#type"] = "hidden";
			cur_form["upload-data1"+str(i)]["#table"] = "upload-attachments";
			cur_form["upload-data1"+str(i)]["#value"] = format_size( file["filesize"] );
			cur_form["upload-data1"+str(i)]["#merge"] = _TRUE;

			cur_form["upload-data2"+str(i)]["#type"] = "hidden";
			cur_form["upload-data2"+str(i)]["#table"] = "upload-attachments";
			cur_form["upload-data2"+str(i)]["#value"] = file["filepath"];
			cur_form["upload-data2"+str(i)]["#merge"] = _TRUE;

			cur_form["upload-data3"+str(i)]["#type"] = "hidden";
			cur_form["upload-data3"+str(i)]["#table"] = "upload-attachments";
			cur_form["upload-data3"+str(i)]["#value"] = file["fid"];
			cur_form["upload-data3"+str(i)]["#merge"] = _TRUE;

			cur_form["upload-attachments"]["#row"+str(i)] = "upload-delete"+str(i)+",upload-list"+str(i)+",upload-description"+str(i)+",upload-weight"+str(i)+",upload-data1"+str(i)+",upload-data2"+str(i)+",upload-data3"+str(i)+",upload-size"+str(i);
		}

		if(i > 0)
		{
			// If has attachments, lets create the table
			cur_form["upload-attachments"]["#type"] = "table";
			cur_form["upload-attachments"]["#header"] = "Delete,List,Description,Size";
			cur_form["upload-attachments"]["#weight"] = "2";
			cur_form["upload-attachments"]["#fieldset"] = "attachments";

			cur_form["upload-total"]["#type"] = "hidden";
			cur_form["upload-total"]["#value"] = str( i );
			cur_form["upload-total"]["#fieldset"] = "attachments";
		}
	}
}

int _upload_build_node( vector < map<string,string> > &files )
{	
	map	<string,string>	file;
	size_t	total = intval(_POST["upload-total"]);
	size_t	i = 0;	
	
	while( i<total )
	{
		string fid = _POST["upload-data3"+str(i)];

		if( _POST["upload-delete"+str(i)] == _TRUE )
			file["remove"] = _TRUE;
		else
			file["remove"] = _FALSE;

		file["list"] = isset(_POST["upload-list"+str(i)]) ? _TRUE : _FALSE;
		file["description"] = _POST["upload-description"+str(i)];
		file["weight"] = _POST["upload-weight"+str(i)];
		file["fid"] = fid;
		
		i++;

		files.push_back( file );
	}

	return (int) total;
}

void upload_load()
{
	vector < map <string,string> > files;
	map <string, string> file;	

	string s3host = "";
	if( model_exists("s3files") ) s3host = variable_get("s3files_url", "");

	if(DB_TYPE==1)
	{
		REDIS_RES *result = redis_query_fields( redis_arg("SORT upload:%d:%d BY upload:*->weight", intval(cur_node["nid"]), intval(cur_node["vid"]) ), "GET upload:*->", "#fid,nid,vid,list,description,weight");
		while( redis_fetch_fields( result, file ) )
		{
			if( redis_multi(file, "HGETALL files:%d", intval(file["fid"])) )
			{
				if( isset(s3host) ) {
					if(file["status"] == "2") file["filepath"] = s3host + file["filepath"];
				}
				files.push_back( file );
			}			
		}
		
	}	
	if(DB_TYPE==2)
	{		
		MYSQL_RES *result = db_querya("SELECT * FROM files f INNER JOIN upload r ON f.fid = r.fid WHERE r.vid = %d ORDER BY r.weight, f.fid", cur_node["vid"].c_str() );
		while( db_fetch( result, file ) )
		{
			if( isset(s3host) ) {
				if(file["status"] == "2") file["filepath"] = s3host + file["filepath"];
			}
			files.push_back( file );
		}
	}

	cur_files = files;
}

void upload_insert()
{
	upload_save();
}

void upload_update()
{
	upload_save();	
}

void upload_delete()
{
	if(DB_TYPE==1)
	{
		string fid;
		REDIS_RES *result = redis_query("SMEMBERS upload:%d:%d", intval(cur_node["nid"]), intval(cur_node["vid"]) );
		while( redis_fetch( result, fid ) ) {
			redis_command("DEL upload:%d", intval(fid) );
			redis_command("DEL files:%d", intval(fid) );
			redis_command("SREM files %d", intval(fid) );
		}
		redis_command("DEL upload:%d:%d", intval(cur_node["nid"]), intval(cur_node["vid"]) );
	}
	if(DB_TYPE==2)
	{
		db_querya("DELETE FROM files WHERE fid IN (SELECT fid FROM upload WHERE nid=%d)", cur_node["nid"].c_str() );
		db_querya("DELETE FROM upload WHERE nid=%d", cur_node["nid"].c_str() );
	}
}

void upload_save()
{
	vector < map <string,string> > files;
	map <string, string> node = cur_node;

	// We have to use _POST variable since the values that we want are not builded by form_alter
	if( _upload_build_node( files ) > 0 )
	{
		if(DB_TYPE==1)
		{
			redis_command("DEL upload:%d:%d", intval(node["nid"]), intval(node["vid"]) );
			for( size_t i = 0; i < files.size(); i++ )
			{
				if( files[i]["remove"] == _FALSE )
				{
					redis_command("SADD upload:%d:%d %d", intval(node["nid"]), intval(node["vid"]), intval(files[i]["fid"]) );
					redis_command_fields(redis_arg("HMSET upload:%d", intval(files[i]["fid"]) ), "",
						"nid,vid,list,description,weight", "%d,%d,%d,%s,%d",
						intval( node["nid"] ),
						intval( node["vid"] ),
						intval( files[i]["list"] ),
						files[i]["description"].c_str(),
						intval( files[i]["weight"]) );
					file_set_status( files[i], FILE_STATUS_PERMANENT );
				}
			}
		}
		if(DB_TYPE==2)
		{
			db_querya("DELETE FROM upload WHERE nid=%d AND vid=%d", node["nid"].c_str(), node["vid"].c_str() );
			for( size_t i = 0; i < files.size(); i++ )
			{
				if( files[i]["remove"] == _FALSE )
				{
					db_querya("INSERT INTO upload (fid, nid, vid, list, description, weight) VALUES(%d, %d, %d, %d, '%s', %d)",
						files[i]["fid"].c_str(),
						node["nid"].c_str(),
						node["vid"].c_str(),
						files[i]["list"].c_str(),
						files[i]["description"].c_str(),
						files[i]["weight"].c_str() );
					file_set_status( files[i], FILE_STATUS_PERMANENT );
				}
			}
		}

	}
}
