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


#include "utfrune.h"
#include <string.h>

#define MIN(x,y)  ((x) < (y) ? (x) : (y))

#define UTFSEQ(x) ((((x) & 0x80) == 0x00) ? 1 /* 0xxxxxxx */ \
                 : (((x) & 0xC0) == 0x80) ? 0 /* 10xxxxxx */ \
                 : (((x) & 0xE0) == 0xC0) ? 2 /* 110xxxxx */ \
                 : (((x) & 0xF0) == 0xE0) ? 3 /* 1110xxxx */ \
                 : (((x) & 0xF8) == 0xF0) ? 4 /* 11110xxx */ \
                 : (((x) & 0xFC) == 0xF8) ? 5 /* 111110xx */ \
                 : (((x) & 0xFE) == 0xFC) ? 6 /* 1111110x */ \
                                          : 0 )

#define BADRUNE(x) ((x) < 0 || (x) > Runemax \
                || ((x) & 0xFFFE) == 0xFFFE \
                || ((x) >= 0xD800 && (x) <= 0xDFFF) \
                || ((x) >= 0xFDD0 && (x) <= 0xFDEF))

int Rune_error = 0xFFFD;

/* Converts the rune and stores it at the location s points to. 
 * Needs up to 4 bytes fater s to store everything. */
int
rune_tochar(char *s, const Rune *p)
{
    Rune r = *p;

    switch(rune_len(r)) {
    case 1: /* 0aaaaaaa */
        s[0] = r;
        return 1;
    case 2: /* 00000aaa aabbbbbb */
        s[0] = 0xC0 | ((r & 0x0007C0) >>  6); /* 110aaaaa */
        s[1] = 0x80 |  (r & 0x00003F);        /* 10bbbbbb */
        return 2;
    case 3: /* aaaabbbb bbcccccc */
        s[0] = 0xE0 | ((r & 0x00F000) >> 12); /* 1110aaaa */
        s[1] = 0x80 | ((r & 0x000FC0) >>  6); /* 10bbbbbb */
        s[2] = 0x80 |  (r & 0x00003F);        /* 10cccccc */
        return 3;
    case 4: /* 000aaabb bbbbcccc ccdddddd */
        s[0] = 0xF0 | ((r & 0x1C0000) >> 18); /* 11110aaa */
        s[1] = 0x80 | ((r & 0x03F000) >> 12); /* 10bbbbbb */
        s[2] = 0x80 | ((r & 0x000FC0) >>  6); /* 10cccccc */
        s[3] = 0x80 |  (r & 0x00003F);        /* 10dddddd */
        return 4;
    default:
        return 0; /* error */
    }
}

int
rune_fromchar(Rune *p, const char *s) {
    return rune_fromcharn(p, s, UTFmax);
}

/* Returns how many UTF-8 bytes are needed to construct the next unicode Rune 
 * to the character string s points to, or 0 if *s is not pointing to a valid UTF-8 
 * character start. 
 */
int rune_utflen(const char * s) {
  if (!s) return 0; 
  return UTFSEQ((*s));
}


/* Converts up to UTFmax or len characters of s into the unicode rune p. 
 * Returns the amount of characters needed for the conversion, or 0 on error.
 */
int
rune_fromcharn(Rune *p, const char *s, size_t len) {
    unsigned int i, n;
    Rune r;

    if(len == 0) /* can't even look at s[0] */
        return 0;

    switch((n = UTFSEQ(s[0]))) {
    case 1: r = s[0];        break; /* 0xxxxxxx */
    case 2: r = s[0] & 0x1F; break; /* 110xxxxx */
    case 3: r = s[0] & 0x0F; break; /* 1110xxxx */
    case 4: r = s[0] & 0x07; break; /* 11110xxx */
    case 5: r = s[0] & 0x03; break; /* 111110xx */
    case 6: r = s[0] & 0x01; break; /* 1111110x */
    default: /* invalid sequence */
        *p = Rune_error;
        return 1;
    }
    /* add values from continuation bytes */
    for(i = 1; i < MIN(n, len); i++)
        if((s[i] & 0xC0) == 0x80) {
            /* add bits from continuation byte to rune value
             * cannot overflow: 6 byte sequences contain 31 bits */
            r = (r << 6) | (s[i] & 0x3F); /* 10xxxxxx */
        }
        else { /* expected continuation */
            *p = Rune_error;
            return i;
        }

    if(i < n) /* must have reached len limit */
        return 0;

    /* reject invalid or overlong sequences */
    if(BADRUNE(r) || rune_len(r) < (int)n)
        r = Rune_error;

    *p = r;
    return n;
}

int
rune_len(Rune r)
{
    if(BADRUNE(r))
        return 0; /* error */
    else if(r <= 0x7F)
        return 1;
    else if(r <= 0x07FF)
        return 2;
    else if(r <= 0xFFFF)
        return 3;
    else
        return 4;
}

size_t
rune_nlen(const Rune *p, size_t len)
{
    size_t i, n = 0;

    for(i = 0; i < len; i++)
        n += rune_len(p[i]);
    return n;
}

/* Returns nonzero if s points to an UTF-8 encoding of a unicode character. */
int
rune_full(const char *s, size_t len)
{
    Rune r;

    return rune_fromcharn(&r, s, len) > 0;
}



char *
utf_ecpy(char *to, char *end, const char *from)
{
    Rune r = -1;
    size_t i, n;

    /* seek through to find final full rune */
    for(i = 0; r != '\0' && (n = rune_fromcharn(&r, &from[i], end - &to[i])); i += n)
        ;
    memcpy(to, from, i); /* copy over bytes up to this rune */

    if(i > 0 && r != '\0')
        to[i] = '\0'; /* terminate if unterminated */
    return &to[i];
}

size_t
utf_len(const char *s)
{
    const char *p = s;
    size_t i;
    Rune r;

    for(i = 0; *p != '\0'; i++)
        p += rune_fromchar(&r, p);
    return i;
}

size_t
utf_nlen(const char *s, size_t len)
{
    const char *p = s;
    size_t i;
    Rune r;
    int n;

    for(i = 0; (n = rune_fromcharn(&r, p, len-(p-s))) && r != '\0'; i++)
        p += n;
    return i;
}

char *
utf_rune(const char *s, Rune r)
{
    if(r < Runeself) {
        return strchr(s, r);
    }
    else if(r == Rune_error) {
        Rune r0;
        int n;

        for(; *s != '\0'; s += n) {
            n = rune_fromchar(&r0, s);
            if(r == r0)
                return (char *)s;
        }
    }
    else {
        char buf[UTFmax+1];
        int n;

        if(!(n = rune_tochar(buf, &r)))
            return NULL;
        buf[n] = '\0';
        return strstr(s, buf);
    }
    return NULL;
}

char *
utf_rrune(const char *s, Rune r)
{
    const char *p = NULL;
    Rune r0;
    int n;

    if(r < Runeself)
        return strrchr(s, r);

    for(; *s != '\0'; s += n) {
        n = rune_fromchar(&r0, s);
        if(r == r0)
            p = s;
    }
    return (char *)p;
}

char *
utf_utf(const char *s, const char *t)
{
    const char *p, *q;
    Rune r0, r1, r2;
    int n, m;

    for(rune_fromchar(&r0, t); (s = utf_rune(s, r0)); s++) {
        for(p = s, q = t; *q && *p; p += n, q += m) {
            n = rune_fromchar(&r1, p);
            m = rune_fromchar(&r2, q);
            if(r1 != r2)
                break;
        }
        if(!*q)
            return (char *)s;
    }
    return NULL;
}
