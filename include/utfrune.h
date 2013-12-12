/*
MIT/X Consortium License

© 2012 Connor Lane Smith <cls@lubutu.com>
© 2013 Beoran <beoran@gmail.com> for later modifications.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

*/

#ifndef UTFRUNE_H
#define UTFRUNE_H

#include <stddef.h>

typedef int Rune;

enum {
	UTFmax   = 6,       /* maximum bytes per rune */
	Runeself = 0x80,    /* rune and utf are equal (<) */
	Runemax  = 0x10FFFF /* maximum rune value */
};

extern Rune Rune_error; /* decoding error in UTF-8 */

int rune_tochar(char * s, const Rune * r);
int rune_utflen(const char * s);
int rune_fromchar(Rune * r, const char * s);
int rune_fromcharn(Rune * r, const char * s, size_t size);
int rune_len(const Rune r);
size_t rune_nlen(const Rune *r , size_t size);
int rune_full(const char * r, size_t size);


char *utf_ecpy(char * s1, char * s2, const char * s3);
size_t utf_len(const char * s);
size_t utf_nlen(const char * s, size_t size);
char *utf_rune(const char * s , Rune r);
char *utf_rrune(const char * s, Rune r);
char *utf_utf(const char * s1, const char * s2);

#endif
