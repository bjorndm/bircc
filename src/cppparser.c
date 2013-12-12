
#include "bstrlib.h"
#include "raku.h"
#include "slre.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct RaParseRule_ RaParseRule;
typedef struct RaParser_    RaParser;
  
/* Function type that gets a character of input from the input stream. */
typedef int RaGetc(void * input);

typedef int RaParseHelper(RaParser * parser, int first_c);

typedef int RaParseAction(RaParser * parser, RaParseRule * rule, int ch);

struct RaParseRule_ {
  int             start_state;
  const char *    set;
  int             next_state;
  int             flags;
  RaParseAction * action;
};


enum RaParserRuleFlags {
  RARU_STORE  = 1 << 1,
  RARU_IGNORE = 1 << 2,
};

enum RaParserState {
  RAPS_BEGIN       ,
  RAPS_NUMBER      ,
  RAPS_WORD        ,
  RAPS_FLOATING    ,
  RAPS_DQSTRING    ,
  RAPS_SQSTRING    ,
  RAPS_BQSTRING    ,
  RAPS_BLOCK       ,
  RAPS_LINECOMMENT ,
  RAPS_BLOCKCOMMENT,  
  RAPS_DONE
};





RaParseRule  raku_parsing_rules[] = {
  { RAPS_BEGIN       , "\t " , RAPS_BEGIN       , RARU_IGNORE, NULL },
  { RAPS_BEGIN       , "({[" , RAPS_BLOCK       , RARU_IGNORE, NULL },
  { RAPS_BEGIN       , "#"   , RAPS_LINECOMMENT , RARU_IGNORE, NULL },
  { RAPS_LINECOMMENT , "{"   , RAPS_BLOCKCOMMENT, RARU_IGNORE, NULL },
  { RAPS_BLOCKCOMMENT, "}"   , RAPS_BEGIN       , RARU_IGNORE, NULL },
  { RAPS_BEGIN       , NULL  , RAPS_WORD        , RARU_IGNORE, NULL }, 
  { RAPS_BLOCK, "({[", RAPS_BLOCK, RARU_IGNORE, NULL }
};

struct RaParser_ {
  Raku        * raku;
  RaValue     * current;
  RaValue     * root;
  int           line;
  int           col;
  char        * filename;
  char        * program;
  int           program_size;  
  int           suspend;
  int           state;
  void        * input;
  int           ch;
  int           ch_prev;
  RaGetc      * get;
  bstring       buffer;
  RaParseHelper * helpers[256];  
  RaParseRule  *  rules;
};

RaParser * raparser_init_helpers(RaParser * self);

/* Initialize parser with given input and getc function. */
RaParser * raparser_init(RaParser * self, void * input, RaGetc * get) {
  if (!self) return NULL;
  self->buffer = bfromcstralloc(128, "");
  if(!self->buffer) {
                 return NULL;
  }
  self->line   =  1;
  self->col    =  1;
  self->get    = get;
  self->input  = input;
  self->ch_prev= -1;
  self->ch     = -1;
  self->state  = -1;
  self->suspend = FALSE;
  // raparser_init_helpers(self);
  return self;
}

/* Cleans up parser. */
RaParser * raparser_done(RaParser * self) {
  if (!self) return NULL;
  bdestroy(self->buffer);
  self->line    =  1;
  self->col     =  1;
  self->get     = NULL;
  self->input   = NULL;
  self->ch_prev = -1;
  self->ch      = -1;
  self->state   = -1;
  self->suspend = FALSE;
  return self;
}

/* Wrapper for fgetc. */
int raparser_fgetc(void * data) {
  return fgetc((FILE*) data);
}


/* Gets the next character, fills in ch and updates line and column. 
  If self->suspend is true, does not actually get the character, but returns 
  the last-gotten character in stead, and unsets suspend. 
 */
int raparser_getnextchar(RaParser * self) {
  int ch;
  
  if (self->suspend) {
    self->suspend = FALSE;
    return self->ch;
  }
  ch = self->get(self->input);
  self->col++;
  /* Newline, unless preceded by a cr gives us a new line. 
   *CR alone or followed by \n too. */
  if (((ch == '\n') && (self->ch_prev != '\r')) || (ch == '\r') ) {
    self->line++;
    self->col = 1;
  }
  self->ch_prev = self->ch;
  self->ch      = ch;
  // NEED THIS? if (ch == EOF) { xxx }
  return self->ch;
}

/* Appends cheracter ch to the parser's buffer. */
void raparser_append(RaParser * parser, int ch) {
  bformata(parser->buffer, "%c", ch);
}


int raparser_parse_step(RaParser * self) {
  int first = raparser_getnextchar(self);
  
} 

/*
 * Need to c-escape this re: 
#define PARSER_RE_STRING \"(\\.|[^\"])*\"
*/


static int parse_program(RaParser * self) {}

/* Parses the given program using recursive descent with. */
int raparser_parse(RaParser * self, char * program, int size) {
  self->program         = program;
  self->program_size    = size;
  // self->program_index   = 0;
  return 0;
}



/* Parsing helpers. */









