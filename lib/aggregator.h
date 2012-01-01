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

void aggregator_hooks();
void aggregator_controllers();
void aggregator_schema();

string aggregator_admin_overview();
void aggregator_refresh(map <string, string> &feed);
void aggregator_save_item(map <string, string> edit);

string aggregator_admin_overview();

string aggregator_add_feed();
string aggregator_add_feed_submit();
bool aggregator_add_feed_validate();

string aggregator_edit_feed();
string aggregator_edit_feed_submit();
bool aggregator_edit_feed_validate();

string aggregator_admin_refresh_feed();
string aggregator_admin_remove_feed();
string aggregator_page_source();

void aggregator_remove( map <string, string> feed );
void aggregator_cron();
