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

void user_hooks();
void user_schema();
void user_controllers();
void user_help();
string user_view();
string user_edit();
string user_edit_submit();
bool user_edit_validate();
string user_register();
string user_register_submit();
bool user_register_validate();
string user_admin();
bool user_admin_validate();
string user_admin_submit();
string user_logout();
string user_admin_roles();
bool user_admin_roles_validate();
string user_admin_roles_submit();
string user_admin_roles_edit();
bool user_admin_roles_edit_validate();
string user_admin_roles_edit_submit();
string user_admin_permissions();
void user_login_by_uid(string uid);
string user_login(void);
string user_login_submit(void);
bool user_login_validate(void);
bool user_is_authenticated(string arguments = string("") );
bool user_is_not_authenticated(string);
void user_load( map <string, string> &account );
bool user_access(string arguments);
string user_autoverify();
