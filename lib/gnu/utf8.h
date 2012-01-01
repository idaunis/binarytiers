/* 
 * File:   utf8.h
 * Author: Pontus Östlund <spam@poppa.se>
 *
 * UTF-8 encoding/decoding functions from/to ISO-8859-1 and US-ASCII.
 * The encoding/decoding functions here are taken from xml.c in PHP
 *
 * Copyright (c) 1999 - 2009 The PHP Group. All rights reserved.
 * Copyright (c) 2009        Pontus Östlund <spam@poppa.se>
 */

#ifndef _UTF8_H
#define	_UTF8_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef char XML_Char;
typedef struct {
  XML_Char        *name;
  char            (*decoding_function)(unsigned short);
  unsigned short  (*encoding_function)(unsigned char);
} xml_encoding;

#ifdef __cplusplus	//added by Jim Howard so that these functions can be called from c++
extern "C" 
{
#endif
char        *utf8_encode       (const char *in);
char        *utf8_decode       (const char *in);
char *xml_utf8_decode   (const XML_Char *, int, int *, const XML_Char *);
char *xml_utf8_encode   (const char *s, int len, int *newlen,
                                const XML_Char *encoding);
void *emalloc           (size_t size);
void *erealloc          (void* ptr, size_t size);
#ifdef __cplusplus	//added by Jim Howard so that these functions can be called from c++
}
#endif
#endif	/* _UTF8_H */
