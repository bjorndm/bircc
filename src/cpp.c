#include <stdio.h>

#include "chic/inli.h"
#include "chic/bstrlib.h"
#include "chic/bstraux.h"


/*
1.
5)
Physical source file multibyte characters are mapped, in an implementation-
defined manner, to the source character set (introducing new-line characters for
end-of-line indicators) if necessary. Trigraph sequences are replaced by
corresponding single-character internal representations.
Implementations shall behave as if these separate phases occur, even though many are typically folded
together in practice. Source files, translation units, and translated translation units need not
necessarily be stored as files, nor need there be any one-to-one correspondence between these entities
and any external representation. The description is conceptual only, and does not specify any
particular implementation.
§5.1.1.2
Environment
9ISO/IEC 9899:TC3
Committee Draft — Septermber 7, 2007
WG14/N1256
2. Each instance of a backslash character (\) immediately followed by a new-line
  character is deleted, splicing physical source lines to form logical source lines.
 Only the last backslash on any physical source line shall be eligible for being part
of such a splice. A source file that is not empty shall end in a new-line character,
which shall not be immediately preceded by a backslash character before any such
splicing takes place.
3. The source file is decomposed into preprocessing tokens6) and sequences of
  white-space characters (including comments). A source file shall not end in a
 partial preprocessing token or in a partial comment. Each comment is replaced by
one space character. New-line characters are retained. Whether each nonempty
sequence of white-space characters other than new-line is retained or replaced by
one space character is implementation-defined.
4. Preprocessing directives are executed, macro invocations are expanded, and
  _Pragma unary operator expressions are executed. If a character sequence that
 matches the syntax of a universal character name is produced by token
concatenation (6.10.3.3), the behavior is undefined. A #include preprocessing
directive causes the named header or source file to be processed from phase 1
through phase 4, recursively. All preprocessing directives are then deleted.
5. Each source character set member and escape sequence in character constants and
  string literals is converted to the corresponding member of the execution character
 set; if there is no corresponding member, it is converted to an implementation-
defined member other than the null (wide) character.7)
6. Adjacent string literal tokens are concatenated.
7. White-space characters separating tokens are no longer significant. Each
  preprocessing token is converted into a token. The resulting tokens are
 syntactically and semantically analyzed and translated as a translation unit.
8. All external object and function references are resolved. Library components are
  linked to satisfy external references to functions and objects not defined in the
 current translation. All such translator output is collected into a program image
which contains information needed for execution in its execution environment.
*/

/* The preprocessor step performs steps 1 through 6. the compiler step 7, 
 * the linker step 8. */

#include "bircc/cpp.h"
#include "bircc/memory.h"

struct bircc_sourcefile_struct {
  bstring filename;
  bstring text;
  FILE  * file;
};


struct bircc_pp_token_struct {
  struct bircc_pp_token_struct * next, * prev;
  bstring           text;
  bircc_sourcefile * source;
  int               real_lineno;
  int               logical_lineno;
  int               real_colno;
  int               logical_colno;
  int               pp_type;
  int               cc_type;  
};


struct bircc_pp_token_list_struct {
  struct bircc_pp_token_struct * first, *last;
  int amount;
};


bircc_sourcefile * bircc_sourcefile_alloc() {
  return BIRCC_STRUCT_ALLOC(bircc_sourcefile);
}

bircc_sourcefile * bircc_sourcefile_free(bircc_sourcefile * me) {
  if(!me) return NULL;
  if (me->filename) { 
    bdestroy(me->filename);
    me->filename = NULL;
  }
  
  if (me->text) { 
    bdestroy(me->text);
    me->text = NULL;
  }  
  bircc_free(me);
  return NULL;
}

int bircc_sourcefile_debugprint(bircc_sourcefile * me) {
  if (!me)       return -1;
  if (!me->text) return -2;
  printf("Source file : %s Contents:\n%s\n", me->filename->data, me->text->data);
  return 0;
}


/* wrapper of fread for bread/breada */
size_t bircc_bNread(void *buff, size_t elsize, size_t nelem, void *param) {
  FILE * stream = param;
  return fread(buff, elsize, nelem, stream);
}


bircc_sourcefile * bircc_sourcefile_read(const char * filename) {
  bircc_sourcefile * me;
  printf("File name: %s\n", filename);
  if (!filename)      return NULL;
  me = bircc_sourcefile_alloc(filename);
  if (!me)            return NULL;
  me->filename = bfromcstr(filename);
  if (!me->filename)  return bircc_sourcefile_free(me);
  
  me->file     = fopen(filename, "rt");
  if (!me->file)      return bircc_sourcefile_free(me);
  me->text = bread(bircc_bNread, me->file);
  if(!me->text) {
    printf("Read error!\n");
    /** Handle read error here... :p */
  } else {
      printf("Read file !\n");
      bircc_sourcefile_debugprint(me);
  }
  return me;
}


typedef struct bircc_trigraph_row_struct {
  const char * trigraph;
  const char * replacement;
} bircc_trigraph_row;

/* Trigraph table */
bircc_trigraph_row bircc_trigraph_table[] = {
  { "\?\?=",  "#"  },
  { "\?\?/",  "\\" }, 
  { "\?\?'",  "^"  },
  { "\?\?(",  "["  },
  { "\?\?)",  "]"  },
  { "\?\?!",  "|"  },
  { "\?\?<",  "{"  },
  { "\?\?>",  "}"  }, 
  { "\?\?-",  "~"  },
  { NULL   ,    0  },
};


/* Expands trigraphs in the read-in source file. */
int bircc_sourcefile_expand_trigraphs(bircc_sourcefile * me) {
  bircc_trigraph_row * row;
  if (!me)       return -1;
  if (!me->text) return -2;
  /* use bfindreplace for now, a bit brute force, but hey... :p */
  for(row = bircc_trigraph_table; row->trigraph; row++) {
    bstring tofind  = bfromcstr(row->trigraph);
    bstring torepl = bfromcstr(row->replacement);
    while(bfindreplace(me->text, tofind, torepl, 0) > 0);
    bdestroy(tofind);
    bdestroy(torepl);
  }
  return 0;
}










