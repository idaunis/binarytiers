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

#ifndef _MSC_VER
	#include <sys/socket.h>
	#include <sys/types.h>
	#include <netdb.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#define closesocket	close
#endif

char sendmail_buf[BUFSIZ+1];
struct hostent *sendmail_hp, *gethostbyname();

void send_socket( int sock, const char *s )
{
	send( sock, s, strlen(s), 0);
}

int read_socket( int sock )
{	
    return recv(sock, sendmail_buf, BUFSIZ, 0);
}

int sendmail( string from, string to, string subject, string message )
{
	int sock;
	struct sockaddr_in server;

	size_t ini, end;

	ini = from.find("<");
	if( ini == string::npos ) ini = 0;
	else ini ++;
	end = from.find(">", ini);
	if( end == string::npos ) end = from.length();

	string from_id = from.substr( ini, end-ini );

	ini = to.find("<");
	if( ini == string::npos ) ini = 0;
	else ini ++;
	end = to.find(">", ini);
	if( end == string::npos ) end = to.length();

	string to_id = to.substr( ini, end-ini );

	if(from.length()==0 || to.length()==0) return 1;

	message = str_replace("\r\n.","\r\n..", message); // you can start a line with a period but you have to send two periods instead.

	string sdate = date("%#c\r\n", str(time()) );
	subject += "\r\n";
	message += "\r\n";
	from += "\r\n";
	to += "\r\n";

	sock = socket(AF_INET, SOCK_STREAM, 0);
	server.sin_family = AF_INET;
	sendmail_hp = gethostbyname( SENDMAIL_HOST );
	if ( sendmail_hp == (struct hostent *) 0)
	{
		print( string("unknown host ")+ SENDMAIL_HOST );
		quit(0);
	}
	
	memcpy((char *) &server.sin_addr, (char *) sendmail_hp->h_addr, sendmail_hp->h_length);
	server.sin_port = htons(25); /* SMTP PORT */
	if (connect(sock, (struct sockaddr *) &server, sizeof server)==-1)
	{
		print("connecting stream socket");
		quit(0);
	}

	/*
S: 220 smtp.example.com ESMTP Postfix
C: HELO relay.example.org
S: 250 Hello relay.example.org, I am glad to meet you
C: MAIL FROM:<bob@example.org>
S: 250 Ok
C: RCPT TO:<alice@example.com>
S: 250 Ok
C: RCPT TO:<theboss@example.com>
S: 250 Ok
C: DATA
S: 354 End data with <CR><LF>.<CR><LF>
C: From: "Bob Example" <bob@example.org>
C: To: Alice Example <alice@example.com>
C: Cc: theboss@example.com
C: Date: Tue, 15 Jan 2008 16:02:43 -0500
C: Subject: Test message
C:
C: Hello Alice.
C: This is a test message with 5 header fields and 4 lines in the message body.
C: Your friend,
C: Bob
C: .
S: 250 Ok: queued as 12345
C: QUIT
S: 221 Bye


*/
	
	read_socket( sock ); /* SMTP Server logon string */
	send_socket( sock, "HELO BinaryTiers\r\n"); /* introduce ourselves */
	read_socket( sock ); /*Read reply */
	send_socket( sock, "MAIL FROM: "); 
	send_socket( sock, from_id.c_str());
	send_socket( sock, "\r\n");
	read_socket( sock ); /* Sender OK */
	send_socket( sock, "VRFY ");
	send_socket( sock, from_id.c_str());
	send_socket( sock, "\r\n");
	read_socket( sock ); // Sender OK */
	send_socket( sock, "RCPT TO: "); /*Mail to*/
	send_socket( sock, to_id.c_str());
	send_socket( sock, "\r\n");
	read_socket( sock ); // 250 OK*/
	send_socket( sock, "DATA\r\n");// body to follow*/
	read_socket( sock ); // 354 End data with <CR><LF>.<CR><LF>*/
	send_socket( sock, "From: ");
	send_socket( sock, from.c_str());
	send_socket( sock, "To: ");
	send_socket( sock, to.c_str());
	send_socket( sock, "Date: ");
	send_socket( sock, sdate.c_str());
	send_socket( sock, "Subject: ");
	send_socket( sock, subject.c_str());
	send_socket( sock, message.c_str());
	send_socket( sock, ".\r\n");
	read_socket( sock ); 
	send_socket( sock, "QUIT\r\n"); /* quit */
	read_socket( sock ); // log off */
	
	closesocket( sock );	

	return 0;
}
