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

#ifndef __BTNode__
#define __BTNode__

void node_hooks();
void node_schema();
void node_controllers();
void node_node_info();
string node_add();
void node_delete( string nid );
string node_admin();
bool node_admin_validate();
string node_admin_submit();

string node_page_add();
bool node_page_add_validate();
string node_page_add_submit();

string node_page_edit();
bool node_page_edit_validate();
string node_page_edit_submit();

string node_page_view();

bool node_load(map <string,string> &node, string nid);
string node_view( map <string,string> &node );

void node_save( map <string,string>	&node );

string node_filter_form();
string node_update_options_form();

void node_filters( map <string, map<string,string> > &filter, vector <string> &options );

void node_preprocess_template( map <string,string>	&node );

#endif
