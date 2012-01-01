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

#include "../../lib/core.h"

void site_hooks();
string site_page_theme( map <string, string> item );
string site_node_theme( map <string, string> item );
string site_username_theme( map <string, string> node );
string site_node_submitted_theme( map <string, string> node );
string site_aggregator_feed_source_theme( map <string, string> item );
string site_aggregator_item_theme( map <string, string> item );
string site_aggregator_wrapper_theme( map <string, string> item );

string site_error_theme( map <string, string> item );
string site_denied_theme( map <string, string> item );
