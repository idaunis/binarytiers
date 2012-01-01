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

void taxonomy_hooks();
void taxonomy_schema();
void taxonomy_help();
void taxonomy_controllers();
void taxonomy_form_alter();
void taxonomy_delete();
void taxonomy_insert();
void taxonomy_update();
void taxonomy_load();
string taxonomy_link( map <string,string> &node );
string taxonomy_add_vocabulary();
string taxonomy_add_vocabulary_submit();
bool taxonomy_add_vocabulary_validate();
string taxonomy_admin();
string taxonomy_edit_vocabulary();
string taxonomy_edit_vocabulary_submit();
bool taxonomy_edit_vocabulary_validate();
string taxonomy_overview_terms();
string taxonomy_add_term();
string taxonomy_add_term_submit();
bool taxonomy_add_term_validate();
int taxonomy_save_term( map<string,string> &form_values );
string taxonomy_edit_term();
bool taxonomy_edit_term_validate();
string taxonomy_edit_term_submit();
string taxonomy_term_page();
string taxonomy_autocomplete();
void taxonomy_get_terms(string vid, map <string, map<string,string> > &terms);
void taxonomy_get_terms(string vid, vector <map <string, string> > &terms);
void taxonomy_get_term( string tid, map <string, string> &term );
string taxonomy_rss();
string taxonomy_rss_list();
string taxonomy_rss_story();
vector <pair <string, string> > taxonomy_make_tree(string parent, vector <map <string, string> > &terms);
