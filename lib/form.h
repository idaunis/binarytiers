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

extern bool load_form_only;
extern bool validate_form;
extern map<string, map<string,string> > cur_form;

string form_render(string item,  map<string, map<string,string> > &form);
string form_items( list < pair<int, string> > &order, map<string, map<string,string> > &form );
string get_form(string form_name, map<string, map<string,string> > &form, string action = string("") );
void form_prepare(string form_name, map<string, map<string,string> > &form );
bool form_validate( map<string, map<string,string> > &form );
void form_set_error(string field, string message);
string form_select( map <string, string> *item );
string form_textfield( map <string, string> *item);
