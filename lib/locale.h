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

#define LANGUAGE_LTR	"0"
#define LANGUAGE_RTL	"1"

void locale_hooks();
void locale_schema();
void locale_controllers();
string locale_admin_language();
bool locale_admin_language_validate();
string locale_admin_language_submit();

string locale_add_language();
bool locale_add_language_validate();
string locale_add_language_submit();

string locale_edit_language();
bool locale_edit_language_validate();
string locale_edit_language_submit();

string locale_locale( string source, string langcode );
string locale_get_name( string langcode );

string locale_admin_translate();
string locale_download();
string locale_translate_search();

string locale_translate_edit();
string locale_translate_delete();
bool locale_translate_edit_validate();
string locale_translate_edit_submit();

string locale_change();
vector <map <string, string> > * language_list();
