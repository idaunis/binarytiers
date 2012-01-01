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

#define FILE_EXISTS_RENAME		0
#define FILE_EXISTS_REPLACE		1
#define FILE_EXISTS_ERROR		2

#define FILE_STATUS_TEMPORARY	0
#define FILE_STATUS_PERMANENT	1

void upload_hooks();
void upload_schema();
void upload_controllers();
void upload_form_alter();
string upload_js();
void upload_insert();
void upload_update();
void upload_save();
void upload_delete();
void upload_load();
void _upload_correct_size( string file, int &file_w, int &file_h );
