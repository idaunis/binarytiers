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

#ifndef __BTPROFILE__
#define __BTPROFILE__

void profile_schema();
void profile_hooks();
void profile_controllers();
void profile_load_profile(map <string, string> &account);
string profile_admin();
string profile_edit();
bool profile_edit_validate();
string profile_edit_submit();
string profile_delete();
void profile_save_profile(map <string, string> &account);
string profile_get_field( string field );
void profile_save_value( string field, string value, string uid = "" );
string profile_get_value( string field, string uid = "" );

#endif
